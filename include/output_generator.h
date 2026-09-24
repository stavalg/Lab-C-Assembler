#ifndef OUTPUT_GENERATOR_H
#define OUTPUT_GENERATOR_H

#include "symbol_table.h"

/* A linked list node to keep track of where external labels are used */
typedef struct ExternUsage {
    char name[31];
    int address;
    struct ExternUsage* next;
} ExternUsage;


/* The main function to write all output files (.ob, .ent, .ext) */
void write_output_files(const char* base_filename, int icf, int dcf,
                        unsigned int instruction_image[], unsigned int data_image[],
                        SymbolTable* symbols, ExternUsage* externals_head);

#endif
