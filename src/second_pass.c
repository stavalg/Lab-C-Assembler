/* This file implements the second pass of the assembler. */

#include "second_pass.h"
#include "parser.h"
#include "encoder.h"
#include "output_generator.h"
#include "utils.h"
#include "instruction_info.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Helper function to parse an operand string and extract its main part and any registers */
static void parse_operand(const char* operand_str, char* main_part, int* reg1, int* reg2) {
    char* bracket_open;
    if (!operand_str) return;

    bracket_open = strchr(operand_str, '[');
    if (bracket_open) {
        strncpy(main_part, operand_str, bracket_open - operand_str);
        main_part[bracket_open - operand_str] = '\0';
        sscanf(bracket_open, "[r%d][r%d]", reg1, reg2);
    } else {
        strcpy(main_part, operand_str);
    }
}

/* Executes the second pass of the assembler */
int run_second_pass(const char* am_filename, const char* base_filename,
                    SymbolTable* symbols, int icf, int dcf) {
    
    unsigned int* instruction_image = (unsigned int*)calloc(icf - 100, sizeof(unsigned int));
    unsigned int* data_image = (unsigned int*)calloc(dcf, sizeof(unsigned int));
    ExternUsage *externals_head = NULL, *externals_tail = NULL;
    
    int ic = 100, dc = 0;
    int line_number = 0;
    int error_found = 0;
    FILE* file;
    char line[MAX_LINE_LENGTH + 2];
    const unsigned int MASK_10_BIT = 0x3FF;

    if (!instruction_image || !data_image) { return 1; }
    file = fopen(am_filename, "r");
    if (!file) { free(instruction_image); free(data_image); return 1; }

    while (fgets(line, sizeof(line), file)) {
        char line_copy[MAX_LINE_LENGTH + 2], *current_pos, *token;
        line_number++;
        strcpy(line_copy, line);

        current_pos = line_copy;
        trim_whitespace(current_pos);

        if (current_pos[0] == '\0' || current_pos[0] == ';') continue;
        
        if (strchr(current_pos, ':')) { current_pos = strchr(current_pos, ':') + 1; }
        
        trim_whitespace(current_pos);
        token = strtok(current_pos, " \t\n\r");
        if (!token) continue;

        if (token[0] == '.') {
            /* Handle all data directives */
            if (strcmp(token, ".entry") == 0) {
                token = strtok(NULL, " \t\n\r,");
                if (token) trim_whitespace(token);
                if (!token || !mark_symbol_as_entry(symbols, token)) {
                    error_found = 1;
                }
            } else if (strcmp(token, ".data") == 0) {
                char *num_token = strtok(NULL, " \t,");
                while (num_token) {
                    trim_whitespace(num_token);
                    if (strlen(num_token) > 0) data_image[dc++] = (unsigned int)atoi(num_token) & MASK_10_BIT;
                    num_token = strtok(NULL, " \t,");
                }
            } else if (strcmp(token, ".string") == 0) {
                char* str_content = strtok(NULL, "\"");
                if (str_content) {
                    int i;
                    for (i = 0; i < strlen(str_content); i++) data_image[dc++] = str_content[i];
                }
                data_image[dc++] = '\0';
            } else if (strcmp(token, ".mat") == 0) {
                char* mat_part = strtok(NULL, "\n\r");
                char* values_part = strrchr(mat_part, ']');
                if (values_part && (*(values_part + 1) != '\0')) {
                    char* num_token = strtok(values_part + 1, " \t,");
                    while (num_token) {
                        trim_whitespace(num_token);
                        if (strlen(num_token) > 0) data_image[dc++] = (unsigned int)atoi(num_token) & MASK_10_BIT;
                        num_token = strtok(NULL, " \t,");
                    }
                }
            }
        } else {
            /* Handle an instruction line */
            const InstructionInfo* info;
            char* operands_part;
            char *src_str = NULL, *dest_str = NULL;
            AddressingMode src_mode = NONE, dest_mode = NONE;
            int ic_offset;
            int extra_word_idx;

            info = get_instruction_by_name(token);
            operands_part = strtok(NULL, "\n\r");
            ic_offset = ic - 100;
            extra_word_idx = 1;

            if (operands_part) {
                if (info->num_operands == 1) {
                    dest_str = operands_part;
                } else if (info->num_operands == 2) {
                    char* comma = strchr(operands_part, ',');
                    if (comma) { 
                        *comma = '\0'; 
                        src_str = operands_part; 
                        dest_str = comma + 1; 
                    }
                }
            }
            if(src_str) trim_whitespace(src_str);
            if(dest_str) trim_whitespace(dest_str);
            if(src_str) src_mode = get_addressing_mode(src_str);
            if(dest_str) dest_mode = get_addressing_mode(dest_str);
            
            instruction_image[ic_offset] = encode_instruction_first_word(info, src_mode, dest_mode);

            if (src_mode == REGISTER && dest_mode == REGISTER) {
                instruction_image[ic_offset + extra_word_idx] = encode_two_registers(atoi(src_str + 1), atoi(dest_str + 1));
            } else {
                /* Encode source operand first */
                if (src_mode != NONE) {
                    char main_part[31]; int r1=0, r2=0;
                    parse_operand(src_str, main_part, &r1, &r2);
                    if (src_mode == IMMEDIATE) {
                        instruction_image[ic_offset + extra_word_idx++] = encode_immediate_operand(atoi(main_part + 1)).word;
                    } else if (src_mode == DIRECT || src_mode == INDEX) {
                        EncodedWord encoded = encode_label_operand(main_part, symbols);
                        instruction_image[ic_offset + extra_word_idx] = encoded.word;
                        if (encoded.is_external) {
                            ExternUsage* nu = (ExternUsage*)malloc(sizeof(ExternUsage));
                            strcpy(nu->name, main_part); nu->address = ic + extra_word_idx; nu->next = NULL;
                            if (!externals_head) externals_head = externals_tail = nu;
                            else { externals_tail->next = nu; externals_tail = nu; }
                        }
                        extra_word_idx++;
                        if (src_mode == INDEX) {
                            instruction_image[ic_offset + extra_word_idx++] = encode_index_registers(r1, r2);
                        }
                    } else if (src_mode == REGISTER) {
                        instruction_image[ic_offset + extra_word_idx++] = encode_two_registers(atoi(main_part + 1), 0);
                    }
                }
                /* Encode destination operand second */
                if (dest_mode != NONE) {
                    char main_part[31]; int r1=0, r2=0;
                    parse_operand(dest_str, main_part, &r1, &r2);
                    if (dest_mode == IMMEDIATE) {
                        instruction_image[ic_offset + extra_word_idx++] = encode_immediate_operand(atoi(main_part + 1)).word;
                    } else if (dest_mode == DIRECT || dest_mode == INDEX) {
                        EncodedWord encoded = encode_label_operand(main_part, symbols);
                        instruction_image[ic_offset + extra_word_idx] = encoded.word;
                        if (encoded.is_external) {
                            ExternUsage* nu = (ExternUsage*)malloc(sizeof(ExternUsage));
                            strcpy(nu->name, main_part); nu->address = ic + extra_word_idx; nu->next = NULL;
                            if (!externals_head) externals_head = externals_tail = nu;
                            else { externals_tail->next = nu; externals_tail = nu; }
                        }
                        extra_word_idx++;
                        if (dest_mode == INDEX) {
                           instruction_image[ic_offset + extra_word_idx++] = encode_index_registers(r1, r2);
                        }
                    } else if (dest_mode == REGISTER) {
                        instruction_image[ic_offset + extra_word_idx++] = encode_two_registers(0, atoi(main_part + 1));
                    }
                }
            }
            ic += calculate_instruction_words(src_mode, dest_mode);
        }
    }
    fclose(file);
    
    if (!error_found) {
        write_output_files(base_filename, icf, dcf, instruction_image, data_image, symbols, externals_head);
    }
    
    free(instruction_image); free(data_image);
    while (externals_head) {
        ExternUsage* temp = externals_head;
        externals_head = externals_head->next;
        free(temp);
    }
    return error_found;
}
