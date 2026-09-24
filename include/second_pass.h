#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "symbol_table.h"

/*
 * Executes the second pass of the assembler.
 * This function encodes instructions and data into machine code and generates the final output files.
 */
int run_second_pass(const char* am_filename, const char* base_filename,
                    SymbolTable* symbols, int icf, int dcf);

#endif
