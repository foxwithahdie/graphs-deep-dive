#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

enum { HEAD_OR_LEAF = -1, LEFT_NODE = 0, RIGHT_NODE = 1 };

typedef struct binarytree_node_t {
  struct binarytree_node_t *child_1;
  struct binarytree_node_t *child_2;
  struct binarytree_node_t *prev;
  int8_t edge_value;
  void *data;
} BinaryTreeNode;

typedef struct binarytree_t {
  struct binarytree_node_t *head;
  size_t height;
} BinaryTree;

bool is_leaf(BinaryTreeNode *node);

bool is_head(BinaryTreeNode *node);

void calculate_depth(BinaryTree *tree);

bool free_binary_tree_node(BinaryTreeNode *node);

bool free_binary_tree(BinaryTree *tree);

BinaryTree *make_binary_tree(BinaryTreeNode *head, size_t height);

BinaryTreeNode *make_node(int8_t edge_value, void *data);