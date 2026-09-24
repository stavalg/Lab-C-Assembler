#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "symbol_table.h"
#include "macro_table.h"

/*
 * Performs the first pass of the assembler.
 * It builds the symbol table, validates syntax, and calculates the final IC and DC values.
 */
int run_first_pass(const char* am_filename, SymbolTable** symbols, MacroTable* macros, int* icf, int* dcf);

#endif /* FIRST_PASS_H */
