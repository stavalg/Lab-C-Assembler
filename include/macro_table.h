#ifndef MACRO_TABLE_H
#define MACRO_TABLE_H

/* A struct representing a single macro */
typedef struct Macro {
    char *name;     /* The macro's name */
    char *content;  /* The macro's content as a single string */
} Macro;

/* A struct representing the entire macro table */
typedef struct MacroTable {
    Macro **macros;     /* A dynamic array of pointers to macros */
    int count;          /* The current number of macros */
    int capacity;       /* The current capacity of the array */
} MacroTable;

/* Creates and initializes a new macro table */
MacroTable* create_macro_table();

/* Adds a new macro to the table */
int add_macro(MacroTable *table, const char *name, const char *content);

/* Finds a macro in the table by its name */
Macro* find_macro(const MacroTable *table, const char *name);

/* Frees all memory occupied by the macro table */
void destroy_macro_table(MacroTable *table);

#endif /* MACRO_TABLE_H */
