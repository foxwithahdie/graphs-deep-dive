#pragma once

#include "tree.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct string_pair_t {
  uint32_t frequency;
  char letter;
} StringPair;

typedef struct huffman_tree_t {
  BinaryTree *tree;
  StringPair *string_pairs;
} HuffmanTree;

size_t calculate_different_letters(const char *str);

bool sort_string_pairs(StringPair string_pair_list[], size_t len);

bool free_huffman_tree(HuffmanTree *huffman_tree);

StringPair *build_string_pairs(const char *str);

bool sort_string_pairs(StringPair string_pair_list[], size_t len);

HuffmanTree *build_huffman_tree(StringPair string_pair_list[], size_t len,
                                BinaryTree *tree);

uint8_t find_encoding_value(HuffmanTree *huffman_tree, const char value);

uint64_t find_string_encoding(HuffmanTree *huffman_tree, const char *str);
