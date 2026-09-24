#ifndef INSTRUCTION_INFO_H
#define INSTRUCTION_INFO_H

/* Enum for the different operand addressing modes */
typedef enum {
    NONE = 0,       /* For instructions with no operands */
    IMMEDIATE = 1,  /* An immediate value, like #5 */
    DIRECT = 2,     /* A direct address, like a LABEL */
    REGISTER = 4,   /* A register, like r3 */
    INDEX = 8       /* Index addressing, like M1[r2][r7] */
} AddressingMode;

/* Struct to hold all static information about an instruction */
typedef struct {
    const char* name;           /* The instruction's name (e.g., "mov") */
    int opcode;                 /* The instruction's opcode */
    int num_operands;           /* The number of operands it takes */
    int allowed_src_modes;      /* Bitmask of allowed modes for the source operand */
    int allowed_dest_modes;     /* Bitmask of allowed modes for the destination operand */
} InstructionInfo;

/* Gets the info for an instruction by its name */
const InstructionInfo* get_instruction_by_name(const char* name);

#endif /* INSTRUCTION_INFO_H */
