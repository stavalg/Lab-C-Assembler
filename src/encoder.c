/* This file contains all the functions for encoding assembly parts into 10-bit machine code words. */

#include "encoder.h"
#include <stdlib.h>
#include <stdio.h>

/* Creates the first machine code word for an instruction (opcode + addressing modes) */
unsigned int encode_instruction_first_word(const InstructionInfo* info, AddressingMode src_mode, AddressingMode dest_mode) {
    unsigned int word = 0;
    word |= (info->opcode << 6);
    if (info->num_operands >= 2) {
        if (src_mode == IMMEDIATE) word |= (0 << 4);
        else if (src_mode == DIRECT) word |= (1 << 4);
        else if (src_mode == INDEX) word |= (2 << 4);
        else if (src_mode == REGISTER) word |= (3 << 4);
    }
    if (info->num_operands >= 1) {
        if (dest_mode == IMMEDIATE) word |= (0 << 2);
        else if (dest_mode == DIRECT) word |= (1 << 2);
        else if (dest_mode == INDEX) word |= (2 << 2);
        else if (dest_mode == REGISTER) word |= (3 << 2);
    }
    word |= ARE_ABSOLUTE;
    return word;
}

/* Creates the machine code word for two registers */
unsigned int encode_two_registers(int src_reg, int dest_reg) {
    unsigned int word = 0;
    word |= (src_reg & 0xF) << 6;
    word |= (dest_reg & 0xF) << 2;
    word |= ARE_ABSOLUTE;
    return word;
}

/* Creates the machine code word for an immediate number operand */
EncodedWord encode_immediate_operand(int value) {
    EncodedWord result;
    result.word = ((unsigned int)(value & 0xFF)) << 2;
    result.word |= ARE_ABSOLUTE;
    result.is_external = 0;
    return result;
}

/* Creates the machine code word for a label operand */
EncodedWord encode_label_operand(const char* label, SymbolTable* symbols) {
    EncodedWord result;
    Symbol* symbol = find_symbol(symbols, label);
    if (!symbol) {
        result.word = 0;
        result.is_external = 0;
        return result;
    }
    if (symbol->type == EXTERNAL_SYMBOL) {
        result.word = ARE_EXTERNAL;
        result.is_external = 1;
    } else {
        result.word = (symbol->value) << 2;
        result.word |= ARE_RELOCATABLE;
        result.is_external = 0;
    }
    return result;
}

/* Creates the machine code word for the registers used in index addressing */
unsigned int encode_index_registers(int row_reg, int col_reg) {
    unsigned int word = 0;
    word |= (row_reg & 0xF) << 6;
    word |= (col_reg & 0xF) << 2;
    word |= ARE_ABSOLUTE;
    return word;
}
