#pragma once

#include <stdlib.h>

// letter: A-Z
// returns international morse code of letter as dots & dashes (e.g. "-.-."),
// NULL if letter is not A-Z
const char *morse_code(char letter);

// letter: A-Z
// returns number of dots + dashes in letter's code, 0 if letter is not A-Z
size_t morse_symbols(char letter);

// letter: A-Z
// returns how long letter takes to send in morse timing units: dot = 1,
// dash = 3, gap between dots/dashes = 1, plus the 3 unit gap after the
// letter. 0 if letter is not A-Z
size_t morse_units(char letter);
