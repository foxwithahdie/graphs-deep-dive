#include "tree.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

bool is_leaf(BinaryTreeNode *node) { return false; }

bool is_head(BinaryTreeNode *node) { return false; }

void calculate_depth(BinaryTree *tree) {}

bool free_binary_tree_node(BinaryTreeNode *node) { return false; }

bool free_binary_tree(BinaryTree *tree) { return false; }

BinaryTree *make_binary_tree(BinaryTreeNode *head, size_t height) {
  return NULL;
}

BinaryTreeNode *make_node(int8_t edge_value, void *data) { return NULL; }