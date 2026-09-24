#ifndef UTILS_H
#define UTILS_H

/* The maximum allowed length for a line in a source file */
#define MAX_LINE_LENGTH 81
/* Boolean TRUE value */
#define TRUE 1
/* Boolean FALSE value */
#define FALSE 0

/* Checks if a given word is a reserved instruction or directive name */
int is_reserved_word(const char *word);

/* Removes leading and trailing whitespace from a string, in-place */
void trim_whitespace(char *str);

#endif /* UTILS_H */
