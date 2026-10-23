#include "huffman_encoding.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

size_t calculate_different_letters(const char *str) { return 0; }

bool free_string_pair(StringPair *string_pair) { return false; }

bool free_huffman_tree(HuffmanTree *shuffman_tree) { return false; }

bool build_string_pairs(StringPair string_pair_list[]) { return false; }

bool sort_string_pairs(StringPair string_pair_list[]) { return false; }

HuffmanTree *build_huffman_tree(StringPair string_pair_list[],
                                BinaryTree *tree) {
  return NULL;
}

uint8_t find_encoding_value(HuffmanTree *huffman_tree, const char value) {
  return 0;
}

uint64_t find_string_encoding(HuffmanTree *huffman_tree, const char *str) {
  return 0;
}
