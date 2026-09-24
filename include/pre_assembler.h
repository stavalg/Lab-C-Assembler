#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

#include "macro_table.h"

/*
 * Executes the pre-assembler stage on a source file.
 * This function reads a .as file, expands all macros, and writes the result to a .am file.
 */
MacroTable* run_pre_assembler(const char *source_filename, int *error_found);

#endif /* PRE_ASSEMBLER_H */
