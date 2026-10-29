#include "morse.h"
#include <stdlib.h>
#include <string.h>

// international morse code; index 0 = A, index 25 = Z
static const char *MORSE_TABLE[26] = {
    ".-",   "-...", "-.-.", "-..",  ".",    "..-.", "--.",  "....", "..",
    ".---", "-.-",  ".-..", "--",   "-.",   "---",  ".--.", "--.-", ".-.",
    "...",  "-",    "..-",  "...-", ".--",  "-..-", "-.--", "--.."};

const char *morse_code(char letter) {
  if (letter < 'A' || letter > 'Z')
    return NULL;

  return MORSE_TABLE[letter - 'A'];
}

size_t morse_symbols(char letter) {
  const char *code = morse_code(letter);

  if (code == NULL)
    return 0;

  return strlen(code);
}

size_t morse_units(char letter) {
  const char *code = morse_code(letter);

  if (code == NULL)
    return 0;

  size_t symbols = strlen(code);
  size_t units = 0;

  // dot = 1 unit, dash = 3 units
  for (size_t i = 0; i < symbols; i++) {
    units += (code[i] == '.') ? 1 : 3;
  }

  // 1 unit gap between each dot/dash, 3 unit gap after the letter
  units += (symbols - 1) + 3;

  return units;
}
