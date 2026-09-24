/* This file contains miscellaneous utility functions used throughout the assembler. */

#include <string.h>
#include <ctype.h>
#include "utils.h"

/* List of all reserved instruction and directive names */
const char *RESERVED_WORDS[] = {
    "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec",
    "jmp", "bne", "red", "prn", "jsr", "rts", "stop",
    "data", "string", "mat", "entry", "extern",
    NULL /* Marks the end of the array */
};

/* Checks if a given word is a reserved keyword */
int is_reserved_word(const char *word) {
    int i;
    if (!word) return FALSE;
    for (i = 0; RESERVED_WORDS[i] != NULL; i++) {
        if (strcmp(word, RESERVED_WORDS[i]) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

/* Removes leading and trailing whitespace from a string */
void trim_whitespace(char *str) {
    char *start, *end;

    if (str == NULL || *str == '\0') {
        return;
    }

    /* Find the first non-whitespace character */
    start = str;
    while (isspace((unsigned char)*start)) {
        start++;
    }

    /* Move the relevant part of the string to the beginning */
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    if (*str == '\0') {
        return;
    }

    /* Find the last non-whitespace character */
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }

    /* Terminate the string after the last character */
    *(end + 1) = '\0';
}
