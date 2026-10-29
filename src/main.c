#include "huffman_encoding.h"
#include "huffman_print.h"
#include "tree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// builds huffman tree for str, prints tree & code table, encodes & decodes str
static bool run_huffman(const char *str) {
  size_t len = calculate_different_letters(str);
  StringPair *string_pairs = build_string_pairs(str);

  HuffmanTree *huffman_tree =
      build_huffman_tree(string_pairs, len, make_binary_tree(NULL, 0));

  printf("string: %s\n\n", str);

  print_huffman_tree(huffman_tree);

  printf("\nletter | frequency | code\n");

  for (size_t i = 0; i < huffman_tree->len; i++) {
    uint8_t code_len = 0;
    char letter = huffman_tree->string_pairs[i].letter;
    uint32_t code = find_encoding_value(huffman_tree, letter, &code_len);

    printf("%c      | %-9u | ", letter,
           huffman_tree->string_pairs[i].frequency);
    print_bits(code, code_len);
    printf("\n");
  }

  size_t bit_len = 0;
  uint64_t encoding = find_string_encoding(huffman_tree, str, &bit_len);

  printf("encoded (%zu bits vs %zu bits fixed 8 bit): ", bit_len,
         strlen(str) * 8);
  print_bits(encoding, bit_len);
  printf("\n");

  char decoded[bit_len + 1];
  bool decoded_ok = decode_string(huffman_tree, encoding, bit_len, decoded);

  bool matches = decoded_ok && strcmp(decoded, str) == 0;

  printf("decoded: %s (%s)\n\n", decoded, matches ? "match" : "MISMATCH");

  free_huffman_tree(huffman_tree);

  return matches;
}

int main(void) {
  bool all_match = true;

  all_match &= run_huffman("ABRACADABRA");
  all_match &= run_huffman("AB");
  all_match &= run_huffman("AAAA");

  return all_match ? EXIT_SUCCESS : EXIT_FAILURE;
}
