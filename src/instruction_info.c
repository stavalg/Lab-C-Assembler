/* This file contains the "database" of all valid assembly instructions and their properties. */

#include "instruction_info.h"
#include <string.h>

/* A static table holding all instruction definitions */
static const InstructionInfo instruction_table[] = {
    /* Two-operand instructions */
    {"mov",  0, 2, IMMEDIATE | DIRECT | REGISTER | INDEX, DIRECT | REGISTER | INDEX},
    {"cmp",  1, 2, IMMEDIATE | DIRECT | REGISTER | INDEX, IMMEDIATE | DIRECT | REGISTER | INDEX},
    {"add",  2, 2, IMMEDIATE | DIRECT | REGISTER | INDEX, DIRECT | REGISTER | INDEX},
    {"sub",  3, 2, IMMEDIATE | DIRECT | REGISTER | INDEX, DIRECT | REGISTER | INDEX},
    {"lea",  6, 2, DIRECT | INDEX,                   DIRECT | REGISTER | INDEX},

    /* One-operand instructions */
    {"not",  4, 1, NONE, DIRECT | REGISTER | INDEX},
    {"clr",  5, 1, NONE, DIRECT | REGISTER | INDEX},
    {"inc",  7, 1, NONE, DIRECT | REGISTER | INDEX},
    {"dec",  8, 1, NONE, DIRECT | REGISTER | INDEX},
    {"jmp",  9, 1, NONE, DIRECT | REGISTER},
    {"bne", 10, 1, NONE, DIRECT | REGISTER},
    {"red", 12, 1, NONE, DIRECT | REGISTER | INDEX},
    {"prn", 13, 1, NONE, IMMEDIATE | DIRECT | REGISTER | INDEX},
    {"jsr", 11, 1, NONE, DIRECT | REGISTER},

    /* Zero-operand instructions */
    {"rts", 14, 0, NONE, NONE},
    {"stop",15, 0, NONE, NONE},

    /* End of table marker */
    {NULL,  0, 0, NONE, NONE}
};

/* Gets the info for an instruction by its name */
const InstructionInfo* get_instruction_by_name(const char* name) {
    int i;
    for (i = 0; instruction_table[i].name != NULL; i++) {
        if (strcmp(name, instruction_table[i].name) == 0) {
            return &instruction_table[i];
        }
    }
    return NULL;
}
