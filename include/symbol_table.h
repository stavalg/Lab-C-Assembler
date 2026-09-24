#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

/* Possible types for a symbol in the table */
typedef enum {
    CODE_SYMBOL,     /* Label for a code instruction */
    DATA_SYMBOL,     /* Label for a data directive */
    EXTERNAL_SYMBOL, /* Label defined as external (.extern) */
    ENTRY_SYMBOL     /* Label defined as an entry point (.entry) */
} SymbolType;

/* Data structure for a single symbol (label) */
typedef struct Symbol {
    char name[31];  /* The name of the symbol */
    int value;      /* The symbol's memory address */
    SymbolType type;/* The symbol's type */
} Symbol;

/* Structure for the entire symbol table */
typedef struct SymbolTable {
    Symbol** symbols;   /* A dynamic array of pointers to symbols */
    int count;          /* The current number of symbols */
    int capacity;       /* The current capacity of the array */
} SymbolTable;

/* Creates and initializes a new symbol table */
SymbolTable* create_symbol_table();

/* Adds a new symbol to the table */
int add_symbol(SymbolTable* table, const char* name, int value, SymbolType type);

/* Searches for a symbol in the table by name */
Symbol* find_symbol(const SymbolTable* table, const char* name);

/* Frees all memory allocated for the symbol table */
void destroy_symbol_table(SymbolTable* table);

/* Updates the addresses of data symbols after the instruction count is finalized */
void update_data_symbols_value(SymbolTable* table, int icf);

/* Prints the content of the symbol table to the console */
void print_symbol_table(const SymbolTable* table);

/* Marks a given symbol in the table as an entry point */
int mark_symbol_as_entry(SymbolTable* table, const char* name);

#endif /* SYMBOL_TABLE_H */
