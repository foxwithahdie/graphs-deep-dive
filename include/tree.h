#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// edge_value of a node = bit on the edge from its parent (prev) to it.
// HEAD_OR_LEAF = node has no parent yet
enum { HEAD_OR_LEAF = -1, LEFT_NODE = 0, RIGHT_NODE = 1 };

// child_1 = 0 edge, child_2 = 1 edge, prev = parent (NULL for head).
// data is not owned by the node; freeing a node never frees data
typedef struct binarytree_node_t {
  struct binarytree_node_t *child_1;
  struct binarytree_node_t *child_2;
  struct binarytree_node_t *prev;
  int8_t edge_value;
  void *data;
} BinaryTreeNode;

// head = top node, height = set by calculate_depth
typedef struct binarytree_t {
  struct binarytree_node_t *head;
  size_t height;
} BinaryTree;

// node: node to check, must not be NULL
// returns true if node has no children and has a parent (prev != NULL).
// a lone node with no parent is not a leaf
bool is_leaf(BinaryTreeNode *node);

// node: node to check, must not be NULL
// returns true if node has no parent and at least one child
bool is_head(BinaryTreeNode *node);

// tree: tree to measure, head must not be NULL
// returns number of nodes on the longest path from head to a leaf (a head
// alone = 1). also stores the result in tree->height
size_t calculate_depth(BinaryTree *tree);

// node: node to free, must not be NULL
// unlinks node from its parent & children, then frees only node itself.
// children and data are not freed
// returns true
bool free_binary_tree_node(BinaryTreeNode *node);

// tree: tree to free, head must not be NULL
// frees every node and tree itself. data of nodes is not freed
// returns true
bool free_binary_tree(BinaryTree *tree);

// head: top node (can be NULL for an empty tree)
// height: starting height (use 0 and call calculate_depth later)
// returns malloc'd tree; free with free_binary_tree
BinaryTree *make_binary_tree(BinaryTreeNode *head, size_t height);

// child1: 0 edge child (can be NULL)
// child2: 1 edge child (can be NULL)
// prev: parent node (NULL if none)
// edge_value: LEFT_NODE, RIGHT_NODE or HEAD_OR_LEAF
// data: pointer stored in the node, not copied or owned
// returns malloc'd node. children's prev is not set; caller links them
BinaryTreeNode *make_node(BinaryTreeNode *child1, BinaryTreeNode *child2,
                          BinaryTreeNode *prev, int8_t edge_value, void *data);
