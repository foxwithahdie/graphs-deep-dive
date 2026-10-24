#include "tree.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// leaf node = node without child
bool is_leaf(BinaryTreeNode *node) {
  return (node->child_1 == NULL && node->child_2 == NULL && node->prev != NULL);
}

// head node = top node. should be sum of frequencies
bool is_head(BinaryTreeNode *node) {
  return ((node->child_1 != NULL || node->child_2 != NULL) &&
          node->prev == NULL);
}

static size_t find_lowest_depth(BinaryTreeNode *node, size_t depth) {
  size_t depth_value = depth + 1;
  if (node->child_1 == NULL && node->child_2 == NULL) {
    return depth_value;
  } else if (node->child_1 != NULL && node->child_2 != NULL) {
    size_t child_1_depth = find_lowest_depth(node->child_1, depth_value);
    size_t child_2_depth = find_lowest_depth(node->child_2, depth_value);
    return (child_1_depth > child_2_depth) ? child_1_depth : child_2_depth;
  } else if (node->child_1 != NULL) {
    size_t child_1_depth = find_lowest_depth(node->child_1, depth_value);
    return child_1_depth;
  } else if (node->child_2 != NULL) {
    size_t child_2_depth = find_lowest_depth(node->child_2, depth_value);
    return child_2_depth;
  }

  return depth_value;
}

// calculates the total depth of tree for huffman algorithm
size_t calculate_depth(BinaryTree *tree) {
  size_t depth = find_lowest_depth(tree->head, 0);

  tree->height = depth;

  return depth;
}

// frees memory space from unused nodes
bool free_binary_tree_node(BinaryTreeNode *node) {
  if (node->prev != NULL) {
    BinaryTreeNode *previous_node = node->prev;

    // if the pointer of the previous node exists
    if (previous_node->child_1 == node) {
      previous_node->child_1 = NULL;
    } else {
      previous_node->child_2 = NULL;
    }
  }

  if (node->child_1 != NULL) {
    BinaryTreeNode *child_1_node = node->child_1;

    child_1_node->prev = NULL;
  }

  if (node->child_2 != NULL) {
    BinaryTreeNode *child_2_node = node->child_2;

    child_2_node->prev = NULL;
  }

  free(node);

  return true;
}

static void free_node_tree(BinaryTreeNode *node) {
  if (node->child_1 == NULL && node->child_2 == NULL) {
    free_binary_tree_node(node);
  } else if (node->child_1 != NULL && node->child_2 != NULL) {
    free_node_tree(node->child_1);
    free_node_tree(node->child_2);
  } else if (node->child_1 != NULL) {
    free_node_tree(node->child_1);
  } else if (node->child_2 != NULL) {
    free_node_tree(node->child_2);
  }

  free_binary_tree_node(node);

  return;
}

bool free_binary_tree(BinaryTree *tree) {

  free_node_tree(tree->head);

  free(tree);

  return true;
}

BinaryTree *make_binary_tree(BinaryTreeNode *head, size_t height) {
  BinaryTree *tree = (BinaryTree *)malloc(sizeof(BinaryTree));

  if (head != NULL) {
    tree->head = head;
  }
  tree->height = height;

  return tree;
}

BinaryTreeNode *make_node(BinaryTreeNode *child1, BinaryTreeNode *child2,
                          BinaryTreeNode *prev, int8_t edge_value, void *data) {

  // allocating space for BinaryTreeNode type pointer; casted from void type
  BinaryTreeNode *tree_node = (BinaryTreeNode *)malloc(sizeof(BinaryTreeNode));

  if (child1 != NULL) {
    tree_node->child_1 = child1;
  }

  if (child2 != NULL) {
    tree_node->child_2 = child2;
  }

  if (prev != NULL) {
    tree_node->prev = prev;
  }

  tree_node->edge_value = edge_value;

  tree_node->data = data;

  return tree_node;
}