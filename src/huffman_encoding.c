#include "huffman_encoding.h"
#include "tree.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// assumption: only letters
// assumption: all same case
size_t calculate_different_letters(const char *str) {
  uint8_t alphabet_bitstring[26];
  char alphabet[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
                       'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
                       'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

  size_t str_len = strlen(str);

  for (int i = 0; i < str_len; i++) {
    // specific character
    char sp_c = str[i]; // = *(str + 1)
    for (int j = 0; j < 26; j++) {
      // i_a indivudal alphabet character
      char i_a = alphabet[j];
      if (sp_c == i_a) {
        alphabet_bitstring[j] = 1;
      }
    }
  }

  size_t return_value = 0;

  for (int i = 0; i < 26; i++) {
    return_value += (alphabet_bitstring[i] == 1);
  }

  return return_value;
}

static uint32_t find_freq(const char *str, const char c) {

  size_t str_len = strlen(str);
  uint32_t freq = 0;
  for (int i = 0; i < str_len; i++) {
    if (c == str[i]) {
      freq++;
    }
  }

  return freq;
}

StringPair *build_string_pairs(const char *str) {
  size_t string_pair_amount = calculate_different_letters(str);
  char unique_letters[string_pair_amount];
  uint32_t frequencies[string_pair_amount];
  StringPair *string_pairs =
      (StringPair *)malloc(sizeof(StringPair) * string_pair_amount);

  size_t unique_letter_index = 0;
  for (int i = 0; i < strlen(str); i++) {
    int letters_covered = 0;
    for (int j = 0; i < string_pair_amount; j++) {
      if (str[i] == unique_letters[j]) {
        break;
      }
      letters_covered++;
    }
    if (letters_covered == string_pair_amount) {
      // letter is considered unique
      unique_letters[unique_letter_index] = str[i];
      unique_letter_index++;
    }
  }

  for (int i = 0; i < string_pair_amount; i++) {
    frequencies[i] = find_freq(str, unique_letters[i]);
  }

  for (int i = 0; i < string_pair_amount; i++) {
    string_pairs[i].frequency = frequencies[i];
    string_pairs[i].letter = unique_letters[i];
  }

  return string_pairs;
}

bool sort_string_pairs(StringPair string_pair_list[]) { return false; }

HuffmanTree *build_huffman_tree(StringPair string_pair_list[],
                                BinaryTree *tree) {
  return NULL;
}

bool free_string_pair(StringPair *string_pair) { return false; }

bool free_huffman_tree(HuffmanTree *shuffman_tree) { return false; }

uint8_t find_encoding_value(HuffmanTree *huffman_tree, const char value) {
  return 0;
}

uint64_t find_string_encoding(HuffmanTree *huffman_tree, const char *str) {
  return 0;
}
