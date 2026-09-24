/* This file contains the line-parsing logic for the first pass. */

#include "parser.h"
#include "utils.h"
#include "instruction_info.h"
#include "macro_table.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

/* Forward declarations for static helper functions */
static int is_valid_label(const char* label);
static int parse_operands_first_pass(char* operands_part, const InstructionInfo* info, int* words, int line_num);
static int parse_data_directive(char* data_part, int* dc, int line_num);
static int parse_string_directive(char* string_part, int* dc, int line_num);
static int parse_mat_directive(char* mat_part, int* dc, int line_num);

/* Parses a single line from the .am file during the first pass */
int parse_line(char* line, SymbolTable* table, MacroTable* macros, int* ic, int* dc, int line_num) {
    char label_name[31] = {0};
    char* current_pos = line;
    char* token;
    char* colon_ptr;
    int error_in_line = 0;
    int label_declared = 0;

    trim_whitespace(current_pos);
    if (current_pos[0] == '\0' || current_pos[0] == ';') return 0; /* Skip empty or comment lines */
    
    colon_ptr = strchr(current_pos, ':');
    if (colon_ptr) {
        if (colon_ptr > current_pos && isspace((unsigned char)*(colon_ptr - 1))) {
            fprintf(stderr, "Line %d: Error - Invalid space before colon in label definition.\n", line_num);
            error_in_line = 1;
        }

        *colon_ptr = '\0';
        trim_whitespace(current_pos);

        if (!is_valid_label(current_pos)) {
            fprintf(stderr, "Line %d: Error - Invalid label name '%s'.\n", line_num, current_pos);
            error_in_line = 1;
        } else {
            if (find_macro(macros, current_pos)) {
                fprintf(stderr, "Line %d: Error - Label name '%s' is already used as a macro name.\n", line_num, current_pos);
                error_in_line = 1;
            } else {
                strcpy(label_name, current_pos);
                label_declared = 1;
            }
        }
        current_pos = colon_ptr + 1;
    }

    trim_whitespace(current_pos);
    token = strtok(current_pos, " \t\n\r");
    
    if (!token) {
        if (label_declared && !error_in_line) {
             fprintf(stderr, "Line %d: Error - Label '%s' is defined for an empty statement.\n", line_num, label_name);
             return 1;
        }
        return error_in_line;
    }

    if (token[0] == '.') { /* It's a directive */
        char *directive_args = strtok(NULL, "\n\r");
        if (label_declared && !error_in_line) {
            if (add_symbol(table, label_name, *dc, DATA_SYMBOL) == 0) {
                fprintf(stderr, "Line %d: Error - Label '%s' is already defined.\n", line_num, label_name);
                error_in_line = 1;
            }
        }
        if (strcmp(token, ".data") == 0) error_in_line |= parse_data_directive(directive_args, dc, line_num);
        else if (strcmp(token, ".string") == 0) error_in_line |= parse_string_directive(directive_args, dc, line_num);
        else if (strcmp(token, ".mat") == 0) error_in_line |= parse_mat_directive(directive_args, dc, line_num);
        else if (strcmp(token, ".extern") == 0) {
             if(directive_args) trim_whitespace(directive_args);
             if (!directive_args || !is_valid_label(directive_args)) {
                 fprintf(stderr, "Line %d: Error - Invalid .extern directive format.\n", line_num);
                 error_in_line = 1;
             } else if (add_symbol(table, directive_args, 0, EXTERNAL_SYMBOL) == 0) {
                 fprintf(stderr, "Line %d: Error - Label '%s' is already defined.\n", line_num, directive_args);
                 error_in_line = 1;
             }
        } else if (strcmp(token, ".entry") == 0) {
            if (!directive_args) {
                fprintf(stderr, "Line %d: Error - Missing argument for .entry directive.\n", line_num);
                error_in_line = 1;
            }
        } else {
            fprintf(stderr, "Line %d: Error - Unknown directive '%s'.\n", line_num, token);
            error_in_line = 1;
        }
    } else { /* It's an instruction */
        const InstructionInfo* info = get_instruction_by_name(token);
        int words_for_instruction = 0;
        char* operands_part = strtok(NULL, "\n\r");
        
        if (!info) {
            fprintf(stderr, "Line %d: Error - Unknown instruction name '%s'.\n", line_num, token);
            return 1;
        }
        if (label_declared && !error_in_line) {
            if (add_symbol(table, label_name, *ic, CODE_SYMBOL) == 0) {
                fprintf(stderr, "Line %d: Error - Label '%s' is already defined.\n", line_num, label_name);
                error_in_line = 1;
            }
        }
        error_in_line |= parse_operands_first_pass(operands_part, info, &words_for_instruction, line_num);
        *ic += words_for_instruction;
    }
    return error_in_line;
}

/* Checks if a label string is valid */
static int is_valid_label(const char* label) {
    int i;
    if (!label || strlen(label) == 0 || strlen(label) > 30 || !isalpha(label[0]) || is_reserved_word(label)) return 0;
    for (i = 1; label[i] != '\0'; i++) if (!isalnum(label[i])) return 0;
    return 1;
}

/* Identifies the addressing mode of a given operand string */
AddressingMode get_addressing_mode(const char* operand) {
    char label_part[31];
    char* bracket_open;
    if (!operand || operand[0] == '\0') return NONE;
    if (operand[0] == '#') return IMMEDIATE;
    if (operand[0] == 'r' && isdigit(operand[1]) && (operand[1] >= '0' && operand[1] <= '7') && operand[2] == '\0') return REGISTER;
    
    bracket_open = strchr(operand, '[');
    if (bracket_open) {
        int len = bracket_open - operand;
        if (len == 0 || len > 30) return NONE;
        strncpy(label_part, operand, len);
        label_part[len] = '\0';
        if (is_valid_label(label_part)) return INDEX;
    }
    if (is_valid_label(operand)) return DIRECT;
    return NONE;
}

/* Calculates the number of memory words required for an instruction */
int calculate_instruction_words(AddressingMode src_mode, AddressingMode dest_mode) {
    int words = 1;

    if (src_mode == REGISTER && dest_mode == REGISTER) {
        return 2;
    }

    if (src_mode != NONE) {
        if (src_mode == INDEX) words += 2;
        else words += 1;
    }

    if (dest_mode != NONE) {
        if (dest_mode == INDEX) words += 2;
        else words += 1;
    }

    return words;
}

/* Parses the operands part of a line during the first pass to validate them */
static int parse_operands_first_pass(char* operands_part, const InstructionInfo* info, int* words, int line_num) {
    char local_operands_part[MAX_LINE_LENGTH] = {0};
    char* src_str = NULL;
    char* dest_str = NULL;
    char* comma;
    AddressingMode src_mode = NONE, dest_mode = NONE;
    
    if (info->num_operands == 0) {
        if (operands_part != NULL) {
            trim_whitespace(operands_part);
            if(strlen(operands_part) > 0) {
                fprintf(stderr, "Line %d: Error - Instruction '%s' should not have any operands.\n", line_num, info->name);
                return 1;
            }
        }
        *words = 1;
        return 0;
    }
    if (!operands_part) {
        fprintf(stderr, "Line %d: Error - Missing operands for instruction '%s'.\n", line_num, info->name);
        return 1;
    }
    strcpy(local_operands_part, operands_part);

    if (info->num_operands == 1) {
        dest_str = strtok(local_operands_part, " \t\n\r");
        if (strtok(NULL, " \t\n\r") != NULL) {
            fprintf(stderr, "Line %d: Error - Too many operands for instruction '%s'.\n", line_num, info->name);
            return 1;
        }
    } else {
        comma = strchr(local_operands_part, ',');
        if (!comma) {
            fprintf(stderr, "Line %d: Error - Missing comma between operands for '%s'.\n", line_num, info->name);
            return 1;
        }
        *comma = '\0';
        src_str = local_operands_part;
        dest_str = comma + 1;
        
        trim_whitespace(src_str);
        trim_whitespace(dest_str);
        
        if (strlen(src_str) == 0 || strlen(dest_str) == 0) {
            fprintf(stderr, "Line %d: Error - Missing an operand for instruction '%s'.\n", line_num, info->name);
            return 1;
        }
    }
    if (src_str) {
        src_mode = get_addressing_mode(src_str);
        if (!(src_mode & info->allowed_src_modes)) {
            fprintf(stderr, "Line %d: Error - Invalid source operand for instruction '%s'.\n", line_num, info->name);
            return 1;
        }
    }
    if (dest_str) {
        dest_mode = get_addressing_mode(dest_str);
         if (!(dest_mode & info->allowed_dest_modes)) {
            fprintf(stderr, "Line %d: Error - Invalid destination operand for instruction '%s'.\n", line_num, info->name);
            return 1;
        }
    }
    *words = calculate_instruction_words(src_mode, dest_mode);
    return 0;
}

/* Calculates the memory size needed for a .data directive */
static int parse_data_directive(char* data_part, int* dc, int line_num) {
    char* token;
    if (!data_part) {
        fprintf(stderr, "Line %d: Error - Missing data for .data directive.\n", line_num);
        return 1;
    }
    token = strtok(data_part, " \t,");
    while (token) {
        trim_whitespace(token);
        if(strlen(token) > 0) {
            (*dc)++;
        }
        token = strtok(NULL, " \t,");
    }
    return 0;
}

/* Calculates the memory size needed for a .string directive */
static int parse_string_directive(char* string_part, int* dc, int line_num) {
    char* start, *end;
    if (!string_part) {
        fprintf(stderr, "Line %d: Error - Missing string for .string directive.\n", line_num);
        return 1;
    }
    trim_whitespace(string_part);
    start = strchr(string_part, '"');
    end = strrchr(string_part, '"');
    if (!start || start == end) {
        fprintf(stderr, "Line %d: Error - Invalid string format, missing quotes.\n", line_num);
        return 1;
    }
    *dc += (end - start);
    return 0;
}

/* Calculates the memory size needed for a .mat directive */
static int parse_mat_directive(char* mat_part, int* dc, int line_num) {
    int rows = 0, cols = 0;
    if (sscanf(mat_part, " [ %d ] [ %d ]", &rows, &cols) == 2) *dc += (rows * cols);
    else {
         fprintf(stderr, "Line %d: Error - Invalid format for .mat directive.\n", line_num);
         return 1;
    }
    return 0;
}
