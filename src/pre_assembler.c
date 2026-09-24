/* This file implements the pre-assembler stage. It handles macro definition and expansion. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pre_assembler.h"
#include "macro_table.h"
#include "utils.h"

#define MACRO_CONTENT_INITIAL_SIZE 256
#define MACRO_CONTENT_GROWTH_FACTOR 2

/* Executes the pre-assembler stage on a source file */
MacroTable* run_pre_assembler(const char *source_filename, int *error_found) {
    FILE *src_file, *am_file;
    char line[MAX_LINE_LENGTH];
    char original_line[MAX_LINE_LENGTH];
    char *token;
    char current_macro_name[MAX_LINE_LENGTH];
    int in_macro_definition = FALSE;
    int line_number = 0;
    char *macro_content_buffer = NULL;
    size_t buffer_size = 0, content_len = 0;
    MacroTable *table = create_macro_table();
    char am_filename[256];

    *error_found = 0;

    if (!table) {
        fprintf(stderr, "Error: Failed to create macro table.\n");
        *error_found = 1;
        return NULL;
    }

    src_file = fopen(source_filename, "r");
    if (!src_file) {
        fprintf(stderr, "Error: Could not open source file '%s'.\n", source_filename);
        destroy_macro_table(table);
        *error_found = 1;
        return NULL;
    }

    strncpy(am_filename, source_filename, sizeof(am_filename) - 4);
    am_filename[strlen(source_filename) - 3] = '\0';
    strcat(am_filename, ".am");

    am_file = fopen(am_filename, "w");
    if (!am_file) {
        fprintf(stderr, "Error: Could not create output file '%s'.\n", am_filename);
        fclose(src_file);
        destroy_macro_table(table);
        *error_found = 1;
        return NULL;
    }

    while (fgets(line, MAX_LINE_LENGTH, src_file) != NULL) {
        line_number++;
        strcpy(original_line, line);
        trim_whitespace(line);

        if (strlen(line) == 0) {
            fputs(original_line, am_file);
            continue;
        }

        token = strtok(line, " \t\n\r");
        if (!token) {
            fputs(original_line, am_file);
            continue;
        }

        if (in_macro_definition) {
            if (strcmp(token, "mcroend") == 0) {
                if (strtok(NULL, " \t\n\r") != NULL) {
                    fprintf(stderr, "Error in file '%s' at line %d: Extraneous text after 'mcroend'.\n", source_filename, line_number);
                    *error_found = 1;
                    break;
                }
                in_macro_definition = FALSE;
                if (!add_macro(table, current_macro_name, macro_content_buffer)) {
                    fprintf(stderr, "Error: Failed to add macro '%s' to table.\n", current_macro_name);
                    *error_found = 1;
                    break;
                }
                free(macro_content_buffer);
                macro_content_buffer = NULL;
            } else {
                size_t line_len = strlen(original_line);
                if (content_len + line_len + 1 > buffer_size) {
                    size_t new_size = (buffer_size == 0) ? MACRO_CONTENT_INITIAL_SIZE : buffer_size * MACRO_CONTENT_GROWTH_FACTOR;
                    char *new_buffer = (char*)realloc(macro_content_buffer, new_size);
                    if (!new_buffer) {
                        fprintf(stderr, "Error: Memory allocation failed for macro content.\n");
                        *error_found = 1;
                        break;
                    }
                    macro_content_buffer = new_buffer;
                    buffer_size = new_size;
                }
                strcat(macro_content_buffer, original_line);
                content_len += line_len;
            }
        } else {
            if (strcmp(token, "mcro") == 0) {
                in_macro_definition = TRUE;
                token = strtok(NULL, " \t\n\r");
                if (!token) {
                    fprintf(stderr, "Error in file '%s' at line %d: Missing macro name after 'mcro'.\n", source_filename, line_number);
                    *error_found = 1;
                    break;
                }
                strcpy(current_macro_name, token);
                if (is_reserved_word(current_macro_name)) {
                    fprintf(stderr, "Error in file '%s' at line %d: Macro name '%s' is a reserved word.\n", source_filename, line_number, current_macro_name);
                    *error_found = 1;
                    break;
                }
                if (strtok(NULL, " \t\n\r") != NULL) {
                    fprintf(stderr, "Error in file '%s' at line %d: Extraneous text after macro definition.\n", source_filename, line_number);
                    *error_found = 1;
                    break;
                }
                buffer_size = MACRO_CONTENT_INITIAL_SIZE;
                macro_content_buffer = (char*)malloc(buffer_size);
                if (!macro_content_buffer) {
                    fprintf(stderr, "Error: Memory allocation failed for macro content.\n");
                    *error_found = 1;
                    break;
                }
                macro_content_buffer[0] = '\0';
                content_len = 0;
            } else {
                Macro* found_macro = find_macro(table, token);
                if (found_macro) {
                    fputs(found_macro->content, am_file);
                } else {
                    fputs(original_line, am_file);
                }
            }
        }
    }

    fclose(src_file);
    fclose(am_file);

    if (*error_found) {
        remove(am_filename);
        if (macro_content_buffer) free(macro_content_buffer);
        destroy_macro_table(table);
        return NULL;
    }

    return table;
}
