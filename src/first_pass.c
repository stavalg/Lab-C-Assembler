/* This file implements the first pass of the assembler. */

#include "first_pass.h"
#include "parser.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_MEMORY 256

/*
 * This function runs the first pass of the assembler.
 * It reads the expanded .am file line by line, builds the symbol table,
 * validates syntax, and calculates final IC and DC values.
 */
int run_first_pass(const char* am_filename, SymbolTable** symbols, MacroTable* macros, int* icf, int* dcf) {
    FILE* file;
    char line[MAX_LINE_LENGTH + 2];
    int ic = 100, dc = 0;
    int line_number = 0;
    int errors_found = 0;
    int c;
    
    *symbols = create_symbol_table();
    if (!*symbols) {
        fprintf(stderr, "Fatal error: Could not create symbol table.\n");
        return 1;
    }

    file = fopen(am_filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s'.\n", am_filename);
        destroy_symbol_table(*symbols);
        *symbols = NULL;
        return 1;
    }

    while (fgets(line, sizeof(line), file)) {
        line_number++;
        
        if (strchr(line, '\n') == NULL && !feof(file)) {
            fprintf(stderr, "Line %d: Error - Line is longer than %d characters.\n", line_number, MAX_LINE_LENGTH - 1);
            errors_found = 1;
            /* Clear the rest of the oversized line from the input buffer */
            while ((c = fgetc(file)) != '\n' && c != EOF);
            continue;
        }
        
        /* Pass the line to the parser for processing */
        if (parse_line(line, *symbols, macros, &ic, &dc, line_number) != 0) {
            errors_found = 1;
        }
    }

    /* After processing all lines, perform final checks */
    if (!errors_found) {
        if ((ic - 100) + dc > TOTAL_MEMORY) {
            fprintf(stderr, "FATAL ERROR in %s: Memory limit exceeded.\n", am_filename);
            errors_found = 2; /* Special return code for fatal error */
        } else {
            /* If no errors, update data symbol addresses and finalize counters */
            update_data_symbols_value(*symbols, ic);
            *icf = ic;
            *dcf = dc;
        }
    }
    
    if (errors_found != 0) {
        destroy_symbol_table(*symbols); 
        *symbols = NULL;
    }

    fclose(file);
    return errors_found;
}
