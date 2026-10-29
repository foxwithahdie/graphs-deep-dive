#pragma once

#include <stdlib.h>

// path: text file to read (e.g. a book from project gutenberg)
// keeps only the letters a-z & A-Z, lowercase made uppercase. everything
// else (spaces, punctuation, digits, non ascii) is dropped
// returns malloc'd null terminated string of A-Z, NULL if the file can't be
// opened. caller frees it
char *read_letters(const char *path);

// str: any null terminated string
// returns malloc'd copy of str with only its letters, lowercase made
// uppercase (same rules as read_letters). caller frees it
char *clean_letters(const char *str);
