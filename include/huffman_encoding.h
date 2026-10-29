#pragma once

#include "tree.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// one letter & how many times it shows up. internal huffman nodes use
// letter '\0' with frequency = sum of their children
typedef struct string_pair_t {
  uint32_t frequency;
  char letter;
} StringPair;

// tree = huffman tree, string_pairs = letters sorted by frequency,
// len = number of string_pairs. owns everything it points to
typedef struct huffman_tree_t {
  BinaryTree *tree;
  StringPair *string_pairs;
  // leaves[i] is the leaf node of string_pairs[i]
  BinaryTreeNode **leaves;
  size_t len;
} HuffmanTree;

// str: null terminated string of uppercase letters A-Z
// returns number of different letters in str (0-26). other characters are
// ignored
size_t calculate_different_letters(const char *str);

// string_pair_list: array to sort in place, lowest -> highest frequency
// len: number of pairs in string_pair_list
// returns false if len is 0, true otherwise
bool sort_string_pairs(StringPair string_pair_list[], size_t len);

// huffman_tree: tree from build_huffman_tree (can be NULL)
// frees the tree, its nodes, leaves, string_pairs and huffman_tree itself
// returns false if huffman_tree is NULL, true otherwise
bool free_huffman_tree(HuffmanTree *huffman_tree);

// str: null terminated string of uppercase letters A-Z only. any other
// character writes past the end of the result
// returns malloc'd array with one pair per different letter, in order of
// first appearance. length = calculate_different_letters(str)
StringPair *build_string_pairs(const char *str);

// string_pair_list: malloc'd pairs from build_string_pairs. gets sorted in
// place & is owned by the result (freed by free_huffman_tree)
// len: number of pairs in string_pair_list
// tree: empty tree from make_binary_tree(NULL, 0); head gets overwritten.
// owned by the result
// returns malloc'd huffman tree, or NULL if len is 0 (then nothing is owned)
HuffmanTree *build_huffman_tree(StringPair string_pair_list[], size_t len,
                                BinaryTree *tree);

// huffman_tree: tree from build_huffman_tree
// value: letter to look up
// code_len: out, set to number of bits in the code (0 if value is not in
// the tree)
// returns code of value in the lowest code_len bits. highest of those bits
// = edge from head, lowest = edge into the leaf
uint32_t find_encoding_value(HuffmanTree *huffman_tree, const char value,
                             uint8_t *code_len);

// huffman_tree: tree from build_huffman_tree
// str: null terminated string to encode
// bit_len: out, set to number of bits used (0 on failure)
// returns codes of str joined together, first letter in the highest bits.
// fails (returns 0) if a letter is not in the tree or the encoding needs
// more than 64 bits
uint64_t find_string_encoding(HuffmanTree *huffman_tree, const char *str,
                              size_t *bit_len);

// huffman_tree: tree from build_huffman_tree
// encoding: bits from find_string_encoding
// bit_len: number of bits in encoding to read
// out: buffer for the decoded string, needs space for bit_len + 1 chars.
// always null terminated
// returns true if every bit decoded into a letter, false if the bits lead
// to a missing edge or end partway down the tree
bool decode_string(HuffmanTree *huffman_tree, uint64_t encoding, size_t bit_len,
                   char *out);
