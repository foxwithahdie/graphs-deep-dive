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

bool free_string_pair_list(StringPair string_pair_list[]) {
  free(string_pair_list);
  return true;
}

bool sort_string_pairs(StringPair string_pair_list[], size_t len) {
  if (len == 0)
    return false;
  for (size_t i = 0; i < len - 1; i++) {

    // Assume the current position holds
    // the minimum element
    size_t min_idx = i;

    // Iterate through the unsorted portion
    // to find the actual minimum
    for (size_t j = i + 1; j < len; j++) {
      if (string_pair_list[j].frequency < string_pair_list[min_idx].frequency) {
        // Update min_idx if a smaller element is found
        min_idx = j;
      }
    }

    // Move minimum element to its
    // correct position
    if (min_idx != i) {
      StringPair temp = string_pair_list[i];
      string_pair_list[i] = string_pair_list[min_idx];
      string_pair_list[min_idx] = temp;
    }
  }

  return true;
}

static uint32_t node_frequency(BinaryTreeNode *node) {
  return (((StringPair *)(node->data))->frequency);
}
// builds huffman tree. inputs string_pair_list (pairs of individual characters
// & frequencies), len being the number of string pairs, tree being input of
// binary tree

HuffmanTree *build_huffman_tree(StringPair *string_pair_list, size_t len,
                                BinaryTree *tree) {
  HuffmanTree *huffman_tree = (HuffmanTree *)malloc(sizeof(HuffmanTree));

  huffman_tree->tree = tree;
  huffman_tree->string_pairs = string_pair_list;

  size_t string_length = 0;
  BinaryTreeNode **list_of_leafs =
      (BinaryTreeNode **)malloc(sizeof(BinaryTreeNode *) * len);

  StringPair string_pairs_copy[len];

  for (size_t i = 0; i < len; i++) {
    string_length += string_pair_list[i].frequency;
    string_pairs_copy[i] = string_pair_list[i];
    list_of_leafs[i] =
        make_node(NULL, NULL, NULL, 0, (void *)(&string_pair_list[i]));
  }

  huffman_tree->tree->head = make_node(NULL, NULL, NULL, 0, (void *)0);

  return huffman_tree;
}

bool free_huffman_tree(HuffmanTree *huffman_tree) { return false; }

uint8_t find_encoding_value(HuffmanTree *huffman_tree, const char value) {
  return 0;
}

uint64_t find_string_encoding(HuffmanTree *huffman_tree, const char *str) {
  return 0;
}
