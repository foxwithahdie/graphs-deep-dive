#include "huffman_encoding.h"
#include "huffman_print.h"
#include "morse.h"
#include "text_input.h"
#include "tree.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// path of the book the huffman tree is built from; relative to the folder
// huffman is run from
static const char *BOOK_PATH = "texts/book.txt";

// sentences encoded with the book's tree & compared to morse code. anything
// that is not a letter is dropped before encoding
static const char *SENTENCES[] = {
    "What hath God wrought",
    "SOS",
    "The quick brown fox jumps over the lazy dog",
};

// fixed length code for 26 letters: 2^5 = 32 >= 26
enum { FIXED_BITS = 5 };

// builds huffman tree for str, prints tree & code table, encodes & decodes str
static bool run_demo(const char *str) {
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
  uint8_t *encoding = find_string_encoding(huffman_tree, str, &bit_len);

  printf("encoded (%zu bits vs %zu bits fixed 8 bit): ", bit_len,
         strlen(str) * 8);
  print_bit_buffer(encoding, bit_len);
  printf("\n");

  char decoded[bit_len + 1];
  bool decoded_ok = decode_string(huffman_tree, encoding, bit_len, decoded);

  bool matches = decoded_ok && strcmp(decoded, str) == 0;

  printf("decoded: %s (%s)\n\n", decoded, matches ? "match" : "MISMATCH");

  free(encoding);
  free_huffman_tree(huffman_tree);

  return matches;
}

// prints the letters of the alphabet that never show up in the book; those
// letters get no huffman code, so sentences with them can't be encoded
static void print_missing_letters(HuffmanTree *huffman_tree) {
  if (huffman_tree->len == 26)
    return;

  printf("letters not in the book:");

  for (char letter = 'A'; letter <= 'Z'; letter++) {
    uint8_t code_len = 0;

    find_encoding_value(huffman_tree, letter, &code_len);

    if (code_len == 0) {
      printf(" %c", letter);
    }
  }

  printf("\n\n");

  return;
}

// prints every letter of the book, most common first, with its huffman code
// & its morse code side by side
static void print_letter_table(HuffmanTree *huffman_tree,
                               size_t total_letters) {
  printf("letter  frequency  huffman code    bits  morse  symbols  units\n");

  // string_pairs is sorted lowest -> highest, so walk it backwards
  for (size_t i = huffman_tree->len; i > 0; i--) {
    StringPair *pair = &huffman_tree->string_pairs[i - 1];
    uint8_t code_len = 0;
    uint32_t code = find_encoding_value(huffman_tree, pair->letter, &code_len);

    printf("%c       %8.4f%%  ", pair->letter,
           100.0 * pair->frequency / (double)total_letters);

    print_bits(code, code_len);

    // lining up the columns after the code
    int padding = (code_len < 16) ? 16 - code_len : 1;

    printf("%*s%4u  %-5s  %7zu  %5zu\n", padding, "", code_len,
           morse_code(pair->letter), morse_symbols(pair->letter),
           morse_units(pair->letter));
  }

  printf("\n");

  return;
}

// prints the average cost of one letter for every code, weighted by how often
// each letter shows up in the book
static void print_totals(HuffmanTree *huffman_tree, size_t total_letters) {
  double huffman_bits = 0;
  double entropy = 0;
  double morse_symbol_average = 0;
  double morse_unit_average = 0;

  for (size_t i = 0; i < huffman_tree->len; i++) {
    StringPair *pair = &huffman_tree->string_pairs[i];
    double probability = pair->frequency / (double)total_letters;
    uint8_t code_len = 0;

    find_encoding_value(huffman_tree, pair->letter, &code_len);

    huffman_bits += probability * code_len;

    // entropy = -sum of p * log2(p); no code can average less than this
    entropy -= probability * log2(probability);

    morse_symbol_average += probability * morse_symbols(pair->letter);
    morse_unit_average += probability * morse_units(pair->letter);
  }

  printf("average cost of one letter:\n");
  printf("  fixed length code        %6.3f bits\n", (double)FIXED_BITS);
  printf("  huffman code             %6.3f bits\n", huffman_bits);
  printf("  entropy (lower bound)    %6.3f bits\n", entropy);
  printf("  morse dots & dashes      %6.3f symbols\n", morse_symbol_average);
  printf("  morse + gap after letter %6.3f symbols\n",
         morse_symbol_average + 1);
  printf("  morse timing             %6.3f units\n", morse_unit_average);
  printf("huffman - entropy = %.3f bits (always between 0 and 1)\n\n",
         huffman_bits - entropy);

  return;
}

// encodes sentence with the book's huffman tree & with morse code, then
// decodes the huffman bits back
static bool run_sentence(HuffmanTree *huffman_tree, const char *sentence) {
  char *letters = clean_letters(sentence);
  size_t letter_count = strlen(letters);

  printf("sentence: %s\n", sentence);
  printf("  letters: %s (%zu)\n", letters, letter_count);

  size_t bit_len = 0;
  uint8_t *encoding = find_string_encoding(huffman_tree, letters, &bit_len);

  if (encoding == NULL) {
    printf("  can't encode: it has a letter that is not in the book\n\n");
    free(letters);
    return false;
  }

  printf("  huffman: ");
  print_bit_buffer(encoding, bit_len);
  printf(" (%zu bits)\n", bit_len);

  size_t symbols = 0;
  size_t units = 0;

  // morse letters separated by a space = the gap between letters
  printf("  morse:   ");

  for (size_t i = 0; i < letter_count; i++) {
    printf("%s%s", (i > 0) ? " " : "", morse_code(letters[i]));
    symbols += morse_symbols(letters[i]);
    units += morse_units(letters[i]);
  }

  printf(" (%zu dots & dashes, %zu units)\n", symbols, units);
  printf("  fixed:   %zu bits\n", letter_count * FIXED_BITS);

  char *decoded = (char *)malloc(bit_len + 1);
  bool decoded_ok = decode_string(huffman_tree, encoding, bit_len, decoded);
  bool matches = decoded_ok && strcmp(decoded, letters) == 0;

  printf("  decoded: %s (%s)\n\n", decoded, matches ? "match" : "MISMATCH");

  free(decoded);
  free(encoding);
  free(letters);

  return matches;
}

int main(void) {
  bool all_match = true;
  char *book = read_letters(BOOK_PATH);

  // no book yet; fall back to the small examples
  if (book == NULL) {
    printf("can't open %s; put a .txt book there or change BOOK_PATH in "
           "main.c\n\n",
           BOOK_PATH);

    all_match &= run_demo("ABRACADABRA");
    all_match &= run_demo("AB");
    all_match &= run_demo("AAAA");

    return all_match ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  size_t total_letters = strlen(book);

  if (total_letters == 0) {
    printf("%s has no letters in it\n", BOOK_PATH);
    free(book);
    return EXIT_FAILURE;
  }

  size_t len = calculate_different_letters(book);
  StringPair *string_pairs = build_string_pairs(book);

  HuffmanTree *huffman_tree =
      build_huffman_tree(string_pairs, len, make_binary_tree(NULL, 0));

  printf("book: %s (%zu letters, %zu different)\n\n", BOOK_PATH,
         total_letters, len);

  print_missing_letters(huffman_tree);

  print_letter_table(huffman_tree, total_letters);

  print_totals(huffman_tree, total_letters);

  print_huffman_tree(huffman_tree);

  printf("\n");

  size_t sentence_count = sizeof(SENTENCES) / sizeof(SENTENCES[0]);

  for (size_t i = 0; i < sentence_count; i++) {
    all_match &= run_sentence(huffman_tree, SENTENCES[i]);
  }

  free_huffman_tree(huffman_tree);
  free(book);

  return all_match ? EXIT_SUCCESS : EXIT_FAILURE;
}
