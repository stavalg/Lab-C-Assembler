/* This file contains the logic for generating all the final output files:
 * .ob (object file), .ent (entries file), and .ext (externals file). */

#include "output_generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Helper function to convert a 10-bit number to the special base 4 */
static void to_special_base_4(unsigned int n, char* buffer) {
    const char special_base[] = "abcd";
    int i = 4;
    buffer[5] = '\0';
    
    while (i >= 0) {
        buffer[i--] = special_base[n & 3];
        n >>= 2;
    }
}

/* The main function to write all output files */
void write_output_files(const char* base_filename, int icf, int dcf,
                        unsigned int instruction_image[], unsigned int data_image[],
                        SymbolTable* symbols, ExternUsage* externals_head) {
    FILE *ob_file, *ent_file, *ext_file;
    char filename[256];
    char base4_buffer_val[6];
    char base4_buffer_addr[6];
    int i;
    int has_entries = 0;

    /* Create .ob file */
    sprintf(filename, "%s.ob", base_filename);
    ob_file = fopen(filename, "w");
    if (!ob_file) return;

    /* Write the header (ICF and DCF) in the special base 4 format */
    to_special_base_4(icf - 100, base4_buffer_val);
    fprintf(ob_file, "%s\t", base4_buffer_val);
    to_special_base_4(dcf, base4_buffer_val);
    fprintf(ob_file, "%s\n", base4_buffer_val);

    /* Write the instruction image */
    for (i = 0; i < icf - 100; i++) {
        to_special_base_4(i + 100, base4_buffer_addr);
        to_special_base_4(instruction_image[i], base4_buffer_val);
        fprintf(ob_file, "%s\t%s\n", base4_buffer_addr, base4_buffer_val);
    }
    /* Write the data image */
    for (i = 0; i < dcf; i++) {
        to_special_base_4(icf + i, base4_buffer_addr);
        to_special_base_4(data_image[i], base4_buffer_val);
        fprintf(ob_file, "%s\t%s\n", base4_buffer_addr, base4_buffer_val);
    }
    fclose(ob_file);

    /* Check if an .ent file needs to be created */
    for (i = 0; i < symbols->count; i++) {
        if (symbols->symbols[i]->type == ENTRY_SYMBOL) {
            has_entries = 1;
            break;
        }
    }
    
    if (has_entries) {
        sprintf(filename, "%s.ent", base_filename);
        ent_file = fopen(filename, "w");
        if (!ent_file) return;
        for (i = 0; i < symbols->count; i++) {
            if (symbols->symbols[i]->type == ENTRY_SYMBOL) {
                to_special_base_4(symbols->symbols[i]->value, base4_buffer_val);
                fprintf(ent_file, "%s\t%s\n", symbols->symbols[i]->name, base4_buffer_val);
            }
        }
        fclose(ent_file);
    }
    
    /* Create .ext file if needed */
    if (externals_head) {
        ExternUsage* current = externals_head;
        sprintf(filename, "%s.ext", base_filename);
        ext_file = fopen(filename, "w");
        if (!ext_file) return;
        while (current) {
            to_special_base_4(current->address, base4_buffer_addr);
            fprintf(ext_file, "%s\t%s\n", current->name, base4_buffer_addr);
            current = current->next;
        }
        fclose(ext_file);
    }
}
