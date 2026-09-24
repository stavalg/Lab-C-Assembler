/* This file implements the data structure and functions for managing the macro table. */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "macro_table.h"

#define INITIAL_CAPACITY 10

/* Creates and initializes a new macro table */
MacroTable* create_macro_table() {
    MacroTable *table = (MacroTable*)malloc(sizeof(MacroTable));
    if (!table) return NULL;

    table->macros = (Macro**)malloc(sizeof(Macro*) * INITIAL_CAPACITY);
    if (!table->macros) {
        free(table);
        return NULL;
    }

    table->count = 0;
    table->capacity = INITIAL_CAPACITY;
    return table;
}

/* Adds a new macro to the table */
int add_macro(MacroTable *table, const char *name, const char *content) {
    Macro *new_macro;
    /* Resize the array if necessary */
    if (table->count == table->capacity) {
        int new_capacity = table->capacity * 2;
        Macro **new_macros = (Macro**)realloc(table->macros, sizeof(Macro*) * new_capacity);
        if (!new_macros) return 0; /* Failure */
        table->macros = new_macros;
        table->capacity = new_capacity;
    }

    /* Create the new macro */
    new_macro = (Macro*)malloc(sizeof(Macro));
    if (!new_macro) return 0;

    new_macro->name = (char*)malloc(strlen(name) + 1);
    if (!new_macro->name) {
        free(new_macro);
        return 0;
    }
    strcpy(new_macro->name, name);

    new_macro->content = (char*)malloc(strlen(content) + 1);
    if (!new_macro->content) {
        free(new_macro->name);
        free(new_macro);
        return 0;
    }
    strcpy(new_macro->content, content);
    
    table->macros[table->count++] = new_macro;
    return 1; /* Success */
}

/* Finds a macro in the table by its name */
Macro* find_macro(const MacroTable *table, const char *name) {
    int i;
    for (i = 0; i < table->count; i++) {
        if (strcmp(table->macros[i]->name, name) == 0) {
            return table->macros[i];
        }
    }
    return NULL;
}

/* Frees all memory occupied by the macro table */
void destroy_macro_table(MacroTable *table) {
    int i;
    if (!table) return;

    for (i = 0; i < table->count; i++) {
        free(table->macros[i]->name);
        free(table->macros[i]->content);
        free(table->macros[i]);
    }
    free(table->macros);
    free(table);
}
