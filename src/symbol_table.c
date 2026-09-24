/* This file implements the data structure and functions for managing the symbol table. */

#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_CAPACITY 10

/* Creates and initializes a new symbol table */
SymbolTable* create_symbol_table() {
    SymbolTable* table = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!table) return NULL;
    
    table->symbols = (Symbol**)malloc(INITIAL_CAPACITY * sizeof(Symbol*));
    if (!table->symbols) {
        free(table);
        return NULL;
    }
    
    table->count = 0;
    table->capacity = INITIAL_CAPACITY;
    return table;
}

/* Frees all memory allocated for the symbol table */
void destroy_symbol_table(SymbolTable* table) {
    int i;
    if (!table) return;
    for (i = 0; i < table->count; i++) {
        free(table->symbols[i]);
    }
    free(table->symbols);
    free(table);
}

/* Adds a new symbol to the table */
int add_symbol(SymbolTable* table, const char* name, int value, SymbolType type) {
    Symbol* new_symbol;
    if (find_symbol(table, name)) {
        return 0; /* Symbol already exists */
    }

    if (table->count == table->capacity) {
        /* Expand table if full */
        int new_capacity = table->capacity * 2;
        Symbol** new_list = (Symbol**)realloc(table->symbols, new_capacity * sizeof(Symbol*));
        if (!new_list) return -1; /* Malloc failure */
        table->symbols = new_list;
        table->capacity = new_capacity;
    }

    new_symbol = (Symbol*)malloc(sizeof(Symbol));
    if (!new_symbol) return -1;

    strcpy(new_symbol->name, name);
    new_symbol->value = value;
    new_symbol->type = type;

    table->symbols[table->count++] = new_symbol;
    return 1; /* Success */
}

/* Searches for a symbol in the table by name */
Symbol* find_symbol(const SymbolTable* table, const char* name) {
    int i;
    for (i = 0; i < table->count; i++) {
        if (strcmp(table->symbols[i]->name, name) == 0) {
            return table->symbols[i];
        }
    }
    return NULL;
}

/* Updates the addresses of data symbols after the instruction count is finalized */
void update_data_symbols_value(SymbolTable* table, int icf) {
    int i;
    for (i = 0; i < table->count; i++) {
        if (table->symbols[i]->type == DATA_SYMBOL) {
            table->symbols[i]->value += icf;
        }
    }
}

/* Prints the content of the symbol table to the console */
void print_symbol_table(const SymbolTable* table) {
    int i;
    const char* type_str;

    if (!table || table->count == 0) {
        printf("Symbol table is empty.\n");
        return;
    }

    printf("\n--- Symbol Table ---\n");
    printf("----------------------------------------\n");
    printf("| %-20s | %-7s | %-8s |\n", "Symbol Name", "Value", "Type");
    printf("----------------------------------------\n");

    for (i = 0; i < table->count; i++) {
        switch (table->symbols[i]->type) {
            case CODE_SYMBOL:     type_str = "Code"; break;
            case DATA_SYMBOL:     type_str = "Data"; break;
            case EXTERNAL_SYMBOL: type_str = "External"; break;
            case ENTRY_SYMBOL:    type_str = "Entry"; break;
            default:              type_str = "Unknown"; break;
        }
        printf("| %-20s | %-7d | %-8s |\n", table->symbols[i]->name, table->symbols[i]->value, type_str);
    }
    printf("----------------------------------------\n\n");
} 

/* Marks a given symbol in the table as an entry point */
int mark_symbol_as_entry(SymbolTable* table, const char* name) {
    Symbol* symbol = find_symbol(table, name);
    if (symbol) {
        /* An external symbol cannot be an entry point */
        if (symbol->type == EXTERNAL_SYMBOL) {
            return 0;
        }
        symbol->type = ENTRY_SYMBOL;
        return 1;
    }
    return 0; /* Symbol not found */
}
