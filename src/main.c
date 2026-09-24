/* This is the main entry point for the assembler program. It handles command-line
 * arguments and controls the main assembly workflow: pre-assembler, first pass,
 * and second pass. */

#include "pre_assembler.h"
#include "first_pass.h"
#include "second_pass.h"
#include "symbol_table.h"
#include "macro_table.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* The main function of the assembler */
int main(int argc, char *argv[]) {
    int i;
    int print_symbols_flag = 0;
    int first_file_index = 1;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s [--print-symbols] file1.as file2.as ...\n", argv[0]);
        return 1;
    }

    /* Check for the optional --print-symbols flag */
    if (strcmp(argv[1], "--print-symbols") == 0) {
        print_symbols_flag = 1;
        first_file_index = 2;
        if (argc < 3) {
            fprintf(stderr, "Error: No files provided after '--print-symbols' flag.\n");
            return 1;
        }
    }

    /* Process each file provided on the command line */
    for (i = first_file_index; i < argc; i++) {
        char am_filename[256], base_filename[256], *dot_pos;
        int pass1_result;
        int pre_assembler_error = 0;
        MacroTable* macros = NULL;
        SymbolTable* symbols = NULL;
        int icf = 0, dcf = 0;

        /* Basic validation for .as file extension */
        if (strlen(argv[i]) < 4 || strcmp(argv[i] + strlen(argv[i]) - 3, ".as") != 0) {
            fprintf(stderr, "Skipping file '%s': Invalid filename or extension.\n", argv[i]);
            continue;
        }

        printf("Processing file: %s\n", argv[i]);

        /* Generate the base and .am filenames */
        strcpy(base_filename, argv[i]);
        dot_pos = strrchr(base_filename, '.');
        if(dot_pos) *dot_pos = '\0';
        sprintf(am_filename, "%s.am", base_filename);

        /* Stage 1: Pre-assembler */
        macros = run_pre_assembler(argv[i], &pre_assembler_error);
        if (pre_assembler_error) {
            fprintf(stderr, "-> Pre-assembler failed for '%s'.\n\n", argv[i]);
            continue;
        }
        printf("-> Pre-assembler successful. '%s' created.\n", am_filename);

        /* Stage 2: First Pass */
        pass1_result = run_first_pass(am_filename, &symbols, macros, &icf, &dcf);
        if (pass1_result != 0) {
            fprintf(stderr, "-> First pass failed for '%s'.\n\n", argv[i]);
            destroy_macro_table(macros);
            continue;
        }
        
        if (print_symbols_flag) { print_symbol_table(symbols); }

        /* Stage 3: Second Pass */
        if (run_second_pass(am_filename, base_filename, symbols, icf, dcf) != 0) {
            fprintf(stderr, "-> Second pass failed for '%s'.\n\n", argv[i]);
        } else {
            printf("-> Successfully created output files for '%s'.\n\n", argv[i]);
        }
        
        /* Cleanup for the current file */
        destroy_symbol_table(symbols);
        destroy_macro_table(macros);
    }
    return 0;
}
