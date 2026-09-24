#ifndef PARSER_H
#define PARSER_H

#include "symbol_table.h"
#include "instruction_info.h"
#include "macro_table.h"

/* Identifies the addressing mode of a given operand string */
AddressingMode get_addressing_mode(const char* operand);

/* Calculates the number of memory words required for an instruction */
int calculate_instruction_words(AddressingMode src, AddressingMode dest);

/* Parses a single line from the .am file during the first pass */
int parse_line(char* line, SymbolTable* table, MacroTable* macros, int* ic, int* dc, int line_num);

#endif /* PARSER_H */
