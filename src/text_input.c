#include "text_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// returns c as an uppercase letter, '\0' if c is not an ascii letter.
// plain ranges instead of isalpha so locale & utf-8 bytes never count
static char to_letter(int c) {
  if (c >= 'A' && c <= 'Z') {
    return (char)c;
  } else if (c >= 'a' && c <= 'z') {
    return (char)(c - 'a' + 'A');
  }

  return '\0';
}

char *read_letters(const char *path) {
  FILE *file = fopen(path, "rb");

  if (file == NULL)
    return NULL;

  // file size = most letters the result can have
  fseek(file, 0, SEEK_END);

  long file_size = ftell(file);

  rewind(file);

  if (file_size < 0) {
    fclose(file);
    return NULL;
  }

  char *letters = (char *)malloc((size_t)file_size + 1);
  size_t letter_count = 0;
  int c;

  while ((c = fgetc(file)) != EOF && letter_count < (size_t)file_size) {
    char letter = to_letter(c);

    if (letter != '\0') {
      letters[letter_count] = letter;
      letter_count++;
    }
  }

  letters[letter_count] = '\0';

  fclose(file);

  return letters;
}

char *clean_letters(const char *str) {
  size_t str_len = strlen(str);
  char *letters = (char *)malloc(str_len + 1);
  size_t letter_count = 0;

  for (size_t i = 0; i < str_len; i++) {
    char letter = to_letter((unsigned char)str[i]);

    if (letter != '\0') {
      letters[letter_count] = letter;
      letter_count++;
    }
  }

  letters[letter_count] = '\0';

  return letters;
}
