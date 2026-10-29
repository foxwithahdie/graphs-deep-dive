#include "huffman_encoding.h"
#include "tree.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// assumption: only letters
// assumption: all same case

size_t calculate_different_letters(const char *str) {
  uint8_t alphabet_bitstring[26] = {0};
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
    for (int j = 0; j < unique_letter_index; j++) {
      // letter was already seen
      if (str[i] == unique_letters[j]) {
        break;
      }
      letters_covered++;
    }
    if (letters_covered == unique_letter_index) {
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

// makes parent node of left & right. parent frequency = sum of children.
// right can be NULL (only happens when string has one unique letter)
static BinaryTreeNode *merge_nodes(BinaryTreeNode *left,
                                   BinaryTreeNode *right) {
  StringPair *sum_pair = (StringPair *)malloc(sizeof(StringPair));

  sum_pair->frequency = node_frequency(left);

  if (right != NULL) {
    sum_pair->frequency += node_frequency(right);
  }

  // '\0' = internal node, not a real letter
  sum_pair->letter = '\0';

  BinaryTreeNode *parent =
      make_node(left, right, NULL, HEAD_OR_LEAF, (void *)sum_pair);

  // lower frequency goes on the 0 edge, higher on the 1 edge
  left->prev = parent;

  left->edge_value = LEFT_NODE;

  if (right != NULL) {
    right->prev = parent;

    right->edge_value = RIGHT_NODE;
  }

  return parent;
}

// inserts node into forest; keeps forest sorted lowest -> highest frequency.
// forest needs space for count + 1 nodes
static void insert_sorted(BinaryTreeNode **forest, size_t count,
                          BinaryTreeNode *node) {
  size_t i = count;

  // shifting heavier nodes right until the spot for node is found
  while (i > 0 && node_frequency(forest[i - 1]) > node_frequency(node)) {
    forest[i] = forest[i - 1];
    i--;
  }

  forest[i] = node;

  return;
}

// builds huffman tree. inputs string_pair_list (pairs of individual characters
// & frequencies), len being the number of string pairs, tree being input of
// binary tree. tree is built bottom up: start with a forest of leaves, merge
// the two lowest frequency nodes until one node (the head) is left

HuffmanTree *build_huffman_tree(StringPair *string_pair_list, size_t len,
                                BinaryTree *tree) {
  if (len == 0)
    return NULL;

  HuffmanTree *huffman_tree = (HuffmanTree *)malloc(sizeof(HuffmanTree));

  huffman_tree->tree = tree;
  huffman_tree->string_pairs = string_pair_list;
  huffman_tree->len = len;

  // sort before making leaves; leaves point into string_pair_list
  sort_string_pairs(string_pair_list, len);

  BinaryTreeNode **list_of_leafs =
      (BinaryTreeNode **)malloc(sizeof(BinaryTreeNode *) * len);

  // forest = nodes not merged yet. acts as a min priority queue
  BinaryTreeNode **forest =
      (BinaryTreeNode **)malloc(sizeof(BinaryTreeNode *) * len);

  for (size_t i = 0; i < len; i++) {
    list_of_leafs[i] = make_node(NULL, NULL, NULL, HEAD_OR_LEAF,
                                 (void *)(&string_pair_list[i]));
    forest[i] = list_of_leafs[i];
  }

  huffman_tree->leaves = list_of_leafs;

  size_t forest_count = len;

  // one unique letter still needs an edge, so its code is "0"
  if (len == 1) {
    forest[0] = merge_nodes(forest[0], NULL);
  }

  while (forest_count > 1) {
    // forest is sorted, so the two lowest frequencies are at the front
    BinaryTreeNode *parent = merge_nodes(forest[0], forest[1]);

    // removing the two merged nodes from the front of forest
    for (size_t i = 2; i < forest_count; i++) {
      forest[i - 2] = forest[i];
    }

    forest_count -= 2;

    insert_sorted(forest, forest_count, parent);

    forest_count++;
  }

  huffman_tree->tree->head = forest[0];

  calculate_depth(huffman_tree->tree);

  free(forest);

  return huffman_tree;
}

// frees StringPair made for internal nodes. leaf data belongs to string_pairs
static void free_internal_data(BinaryTreeNode *node) {
  if (node == NULL || is_leaf(node))
    return;

  free_internal_data(node->child_1);
  free_internal_data(node->child_2);

  free(node->data);

  return;
}

bool free_huffman_tree(HuffmanTree *huffman_tree) {
  if (huffman_tree == NULL)
    return false;

  free_internal_data(huffman_tree->tree->head);

  free_binary_tree(huffman_tree->tree);

  free(huffman_tree->leaves);

  free_string_pair_list(huffman_tree->string_pairs);

  free(huffman_tree);

  return true;
}

// returns code of value; code_len = number of bits in code (0 if value is not
// in tree). highest bit = edge from head, lowest bit = edge into leaf
uint32_t find_encoding_value(HuffmanTree *huffman_tree, const char value,
                             uint8_t *code_len) {
  BinaryTreeNode *node = NULL;

  *code_len = 0;

  // finding the leaf that holds value
  for (size_t i = 0; i < huffman_tree->len; i++) {
    if (huffman_tree->string_pairs[i].letter == value) {
      node = huffman_tree->leaves[i];
      break;
    }
  }

  if (node == NULL)
    return 0;

  uint32_t code = 0;

  // walking up from leaf to head using prev; each edge adds one bit
  while (node->prev != NULL) {
    code |= ((uint32_t)node->edge_value) << *code_len;
    (*code_len)++;
    node = node->prev;
  }

  return code;
}

// returns encoding of whole str; bit_len = number of bits used (0 on failure).
// fails when a letter is not in tree or the encoding needs more than 64 bits
uint64_t find_string_encoding(HuffmanTree *huffman_tree, const char *str,
                              size_t *bit_len) {
  uint64_t encoding = 0;

  *bit_len = 0;

  size_t str_len = strlen(str);

  for (size_t i = 0; i < str_len; i++) {
    uint8_t code_len = 0;
    uint32_t code = find_encoding_value(huffman_tree, str[i], &code_len);

    if (code_len == 0 || *bit_len + code_len > 64) {
      *bit_len = 0;
      return 0;
    }

    // shifting earlier codes left to make room for this code
    encoding = (encoding << code_len) | code;
    *bit_len += code_len;
  }

  return encoding;
}

// decodes bit_len bits of encoding into out. out needs space for bit_len + 1
// chars. no separators needed since huffman codes are prefix free
bool decode_string(HuffmanTree *huffman_tree, uint64_t encoding, size_t bit_len,
                   char *out) {
  BinaryTreeNode *head = huffman_tree->tree->head;
  BinaryTreeNode *node = head;
  size_t out_index = 0;

  // reading highest bit first; 0 = child_1, 1 = child_2
  for (size_t i = bit_len; i > 0; i--) {
    uint8_t bit = (encoding >> (i - 1)) & 1;

    node = (bit == RIGHT_NODE) ? node->child_2 : node->child_1;

    // bits lead to an edge that does not exist
    if (node == NULL) {
      out[out_index] = '\0';
      return false;
    }

    // reached a letter; start again from head
    if (is_leaf(node)) {
      out[out_index] = ((StringPair *)(node->data))->letter;
      out_index++;
      node = head;
    }
  }

  out[out_index] = '\0';

  // leftover bits that never reached a leaf
  return (node == head);
}
