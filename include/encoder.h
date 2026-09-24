#ifndef ENCODER_H
#define ENCODER_H

#include "instruction_info.h"
#include "symbol_table.h"

/* The ARE (Absolute, Relocatable, External) bits, just defined as numbers */
#define ARE_ABSOLUTE    0
#define ARE_RELOCATABLE 2
#define ARE_EXTERNAL    1

/* A helper struct to pass around an encoded word and info about it */
typedef struct {
    unsigned int word;
    int is_external; /* A flag to track if this word is for an external symbol */
} EncodedWord;

/* Creates the first machine code word for an instruction (opcode + addressing modes) */
unsigned int encode_instruction_first_word(const InstructionInfo* info, AddressingMode src_mode, AddressingMode dest_mode);

/* Creates the machine code word for two registers */
unsigned int encode_two_registers(int src_reg, int dest_reg);

/* Creates the machine code word for an immediate number operand (#5) */
EncodedWord encode_immediate_operand(int value);

/* Creates the machine code word for a label operand (like LOOP) */
EncodedWord encode_label_operand(const char* label, SymbolTable* symbols);

/* Creates the machine code word for the registers used in index addressing */
unsigned int encode_index_registers(int row_reg, int col_reg);

#endif
