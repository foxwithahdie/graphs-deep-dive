#pragma once

#include "huffman_encoding.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// bits: bits to print
// len: number of lowest bits to print, highest of them first
void print_bits(uint64_t bits, size_t len);

// huffman_tree: tree from build_huffman_tree (can be NULL)
// draws the tree top down, centered in the terminal. letters are rectangles
// (letter:frequency on top, code below), nodes without a letter are ovals
// with their frequency. edges are labeled with their bit. if the tree is
// wider than the terminal it is printed sideways instead
// returns false if huffman_tree is NULL, true otherwise
bool print_huffman_tree(HuffmanTree *huffman_tree);
