#include "huffman_print.h"
#include "huffman_encoding.h"
#include "tree.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

// rows of a rectangle, rows per tree level (box + connector line), and space
// between two sibling subtrees
enum { BOX_HEIGHT = 4, LEVEL_HEIGHT = 5, SUBTREE_GAP = 2 };

// unicode box drawing characters
enum {
  H_LINE = 0x2500,
  V_LINE = 0x2502,
  TOP_LEFT = 0x250C,
  TOP_RIGHT = 0x2510,
  BOTTOM_LEFT = 0x2514,
  BOTTOM_RIGHT = 0x2518,
  T_DOWN = 0x252C,
  T_UP = 0x2534,
  ROUND_TOP_LEFT = 0x256D,
  ROUND_TOP_RIGHT = 0x256E,
  ROUND_BOTTOM_RIGHT = 0x256F,
  ROUND_BOTTOM_LEFT = 0x2570
};

// where one node goes when drawn. every column is relative to the left edge
// of the node's subtree
typedef struct draw_node_t {
  struct draw_node_t *child_1;
  struct draw_node_t *child_2;
  int child_1_left;
  int child_2_left;
  // column of the node's center line
  int center;
  // width of the whole subtree
  int width;
  int box_width;
  // leaf: letter:frequency & code. internal node: frequency only
  char line_1[16];
  char line_2[40];
} DrawNode;

// grid of characters the tree is drawn on before printing
typedef struct canvas_t {
  uint32_t *cells;
  int rows;
  int cols;
} Canvas;

void print_bits(uint64_t bits, size_t len) {
  for (size_t i = len; i > 0; i--) {
    printf("%u", (unsigned)((bits >> (i - 1)) & 1));
  }

  return;
}

// width of the terminal in columns. falls back to $COLUMNS, then 80, when
// output is not a terminal (e.g. piped into a file)
static int terminal_width(void) {
  struct winsize window_size;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size) == 0 &&
      window_size.ws_col > 0) {
    return window_size.ws_col;
  }

  const char *columns = getenv("COLUMNS");

  if (columns != NULL && atoi(columns) > 0) {
    return atoi(columns);
  }

  return 80;
}

// prints one unicode character as utf-8 (box characters are 3 bytes)
static void print_utf8(uint32_t c) {
  if (c < 0x80) {
    putchar((int)c);
  } else if (c < 0x800) {
    putchar((int)(0xC0 | (c >> 6)));
    putchar((int)(0x80 | (c & 0x3F)));
  } else {
    putchar((int)(0xE0 | (c >> 12)));
    putchar((int)(0x80 | ((c >> 6) & 0x3F)));
    putchar((int)(0x80 | (c & 0x3F)));
  }

  return;
}

static int max_int(int a, int b) { return (a > b) ? a : b; }

// figures out box sizes & positions of node and everything below it.
// children are placed side by side, the node is centered above them
static DrawNode *layout_node(HuffmanTree *huffman_tree, BinaryTreeNode *node) {
  DrawNode *draw_node = (DrawNode *)calloc(1, sizeof(DrawNode));
  StringPair *pair = (StringPair *)(node->data);

  // leaf = rectangle, 2 lines of text: "│ " + text + " │"
  if (node->child_1 == NULL && node->child_2 == NULL) {
    uint8_t code_len = 0;
    uint32_t code = find_encoding_value(huffman_tree, pair->letter, &code_len);

    snprintf(draw_node->line_1, sizeof(draw_node->line_1), "%c:%u",
             pair->letter, pair->frequency);

    for (uint8_t i = 0; i < code_len; i++) {
      draw_node->line_2[i] = (char)('0' + ((code >> (code_len - 1 - i)) & 1));
    }

    draw_node->line_2[code_len] = '\0';

    int text_width = max_int((int)strlen(draw_node->line_1),
                             (int)strlen(draw_node->line_2));

    draw_node->box_width = text_width + 4;
    draw_node->width = draw_node->box_width;
    draw_node->center = draw_node->box_width / 2;

    return draw_node;
  }

  // internal node = oval, 1 line of text: "( " + frequency + " )"
  snprintf(draw_node->line_1, sizeof(draw_node->line_1), "%u",
           pair->frequency);

  draw_node->box_width = (int)strlen(draw_node->line_1) + 4;

  // huffman nodes always have child_1; child_2 is NULL with one unique letter
  draw_node->child_1 = layout_node(huffman_tree, node->child_1);
  draw_node->child_1_left = 0;

  int children_width = draw_node->child_1->width;
  int center = draw_node->child_1->center;

  if (node->child_2 != NULL) {
    draw_node->child_2 = layout_node(huffman_tree, node->child_2);
    draw_node->child_2_left = children_width + SUBTREE_GAP;

    // an odd distance between the children's center lines has no middle
    // column; one more column of space puts the parent exactly in between
    if ((draw_node->child_2_left + draw_node->child_2->center - center) % 2 !=
        0) {
      draw_node->child_2_left++;
    }

    children_width = draw_node->child_2_left + draw_node->child_2->width;

    // halfway between the two children's center lines
    center = (center + draw_node->child_2_left + draw_node->child_2->center) / 2;
  }

  // box sticks out past the left edge; move children right to make room
  int shift = 0;

  if (center - draw_node->box_width / 2 < 0) {
    shift = draw_node->box_width / 2 - center;
  }

  draw_node->child_1_left += shift;
  draw_node->child_2_left += shift;
  draw_node->center = center + shift;

  // box can also stick out past the right edge of the children
  int box_right = draw_node->center - draw_node->box_width / 2 +
                  draw_node->box_width;

  draw_node->width = max_int(children_width + shift, box_right);

  return draw_node;
}

static void free_layout(DrawNode *draw_node) {
  if (draw_node == NULL)
    return;

  free_layout(draw_node->child_1);
  free_layout(draw_node->child_2);

  free(draw_node);

  return;
}

static void canvas_put(Canvas *canvas, int row, int col, uint32_t c) {
  canvas->cells[row * canvas->cols + col] = c;

  return;
}

// writes text centered between columns left & left + width - 1
static void canvas_text(Canvas *canvas, int row, int left, int width,
                        const char *text) {
  int len = (int)strlen(text);
  int col = left + (width - len) / 2;

  for (int i = 0; i < len; i++) {
    canvas_put(canvas, row, col + i, (uint32_t)text[i]);
  }

  return;
}

// draws a horizontal box border from left to right with the given corners
static void canvas_border(Canvas *canvas, int row, int left, int right,
                          uint32_t left_corner, uint32_t right_corner) {
  for (int col = left + 1; col < right; col++) {
    canvas_put(canvas, row, col, H_LINE);
  }

  canvas_put(canvas, row, left, left_corner);
  canvas_put(canvas, row, right, right_corner);

  return;
}

// rectangle for a leaf: letter:frequency on the first line, code on the second
static void draw_rectangle(Canvas *canvas, DrawNode *draw_node, int top,
                           int center, bool has_parent) {
  int left = center - draw_node->box_width / 2;
  int right = left + draw_node->box_width - 1;

  canvas_border(canvas, top, left, right, TOP_LEFT, TOP_RIGHT);
  canvas_border(canvas, top + 3, left, right, BOTTOM_LEFT, BOTTOM_RIGHT);

  for (int row = top + 1; row <= top + 2; row++) {
    canvas_put(canvas, row, left, V_LINE);
    canvas_put(canvas, row, right, V_LINE);
  }

  canvas_text(canvas, top + 1, left + 1, draw_node->box_width - 2,
              draw_node->line_1);
  canvas_text(canvas, top + 2, left + 1, draw_node->box_width - 2,
              draw_node->line_2);

  // edge from parent comes into the top
  if (has_parent) {
    canvas_put(canvas, top, center, T_UP);
  }

  return;
}

// oval for a node without a letter: frequency inside rounded corners
static void draw_oval(Canvas *canvas, DrawNode *draw_node, int top,
                      int center, bool has_parent) {
  int left = center - draw_node->box_width / 2;
  int right = left + draw_node->box_width - 1;

  canvas_border(canvas, top, left, right, ROUND_TOP_LEFT, ROUND_TOP_RIGHT);
  canvas_border(canvas, top + 2, left, right, ROUND_BOTTOM_LEFT,
                ROUND_BOTTOM_RIGHT);

  canvas_put(canvas, top + 1, left, '(');
  canvas_put(canvas, top + 1, right, ')');

  canvas_text(canvas, top + 1, left + 1, draw_node->box_width - 2,
              draw_node->line_1);

  if (has_parent) {
    canvas_put(canvas, top, center, T_UP);
  }

  // ovals are one row shorter than rectangles; line goes down to the
  // connector row
  canvas_put(canvas, top + 2, center, T_DOWN);
  canvas_put(canvas, top + 3, center, V_LINE);

  return;
}

// line from a parent down to its children, each edge labeled with its bit.
// center_2 = -1 when there is only one child (it sits right under the parent)
static void draw_connector(Canvas *canvas, int row, int center, int center_1,
                           int center_2) {
  if (center_2 < 0) {
    canvas_put(canvas, row, center, V_LINE);
    canvas_put(canvas, row, center + 1, '0' + LEFT_NODE);

    return;
  }

  canvas_border(canvas, row, center_1, center_2, TOP_LEFT, TOP_RIGHT);

  canvas_put(canvas, row, center, T_UP);

  // labels are mirrored so both sit the same distance from their corner
  int label_0 = (center_1 + center) / 2;

  canvas_put(canvas, row, label_0, '0' + LEFT_NODE);
  canvas_put(canvas, row, center_2 - (label_0 - center_1), '0' + RIGHT_NODE);

  return;
}

// draws draw_node & everything below it. left = column of its subtree's left
// edge, depth = level in the tree (head = 0)
static void draw_tree(Canvas *canvas, DrawNode *draw_node, int left,
                      int depth, bool has_parent) {
  int top = depth * LEVEL_HEIGHT;
  int center = left + draw_node->center;

  if (draw_node->child_1 == NULL) {
    draw_rectangle(canvas, draw_node, top, center, has_parent);

    return;
  }

  draw_oval(canvas, draw_node, top, center, has_parent);

  int child_1_left = left + draw_node->child_1_left;
  int center_1 = child_1_left + draw_node->child_1->center;
  int center_2 = -1;

  draw_tree(canvas, draw_node->child_1, child_1_left, depth + 1, true);

  if (draw_node->child_2 != NULL) {
    int child_2_left = left + draw_node->child_2_left;

    center_2 = child_2_left + draw_node->child_2->center;

    draw_tree(canvas, draw_node->child_2, child_2_left, depth + 1, true);
  }

  draw_connector(canvas, top + BOX_HEIGHT, center, center_1, center_2);

  return;
}

// fallback for trees wider than the terminal: one node per line, head at the
// top left. last[i] = whether the node at depth i on the current path is the
// last child of its parent; decides if a "│" keeps going down under it
static void print_sideways_node(HuffmanTree *huffman_tree, BinaryTreeNode *node,
                                bool last[], size_t depth) {
  // drawing the lines of the nodes above this one
  for (size_t i = 1; i < depth; i++) {
    printf(last[i] ? "     " : "│    ");
  }

  // edge from parent, labeled with its bit
  if (depth > 0) {
    printf(last[depth] ? "└─%d─ " : "├─%d─ ", node->edge_value);
  }

  StringPair *pair = (StringPair *)(node->data);

  // leaf = letter:frequency [code]
  if (is_leaf(node)) {
    uint8_t code_len = 0;
    uint32_t code = find_encoding_value(huffman_tree, pair->letter, &code_len);

    printf("%c:%u [", pair->letter, pair->frequency);
    print_bits(code, code_len);
    printf("]\n");

    return;
  }

  // internal node = (sum of frequencies below it)
  printf("(%u)\n", pair->frequency);

  // child_2 is NULL when the string has one unique letter
  if (node->child_1 != NULL) {
    last[depth + 1] = (node->child_2 == NULL);
    print_sideways_node(huffman_tree, node->child_1, last, depth + 1);
  }

  if (node->child_2 != NULL) {
    last[depth + 1] = true;
    print_sideways_node(huffman_tree, node->child_2, last, depth + 1);
  }

  return;
}

bool print_huffman_tree(HuffmanTree *huffman_tree) {
  if (huffman_tree == NULL)
    return false;

  DrawNode *root = layout_node(huffman_tree, huffman_tree->tree->head);
  int columns = terminal_width();

  if (root->width > columns) {
    // height = nodes on longest path, so depth goes from 0 to height - 1
    bool last[huffman_tree->tree->height];

    printf("(tree is %d columns wide, terminal is %d; printing it sideways)\n",
           root->width, columns);

    print_sideways_node(huffman_tree, huffman_tree->tree->head, last, 0);

    free_layout(root);

    return true;
  }

  // last level only needs the box rows, no connector below it
  int levels = (int)huffman_tree->tree->height;
  Canvas canvas = {NULL, (levels - 1) * LEVEL_HEIGHT + BOX_HEIGHT,
                   root->width};

  canvas.cells =
      (uint32_t *)malloc(sizeof(uint32_t) * canvas.rows * canvas.cols);

  for (int i = 0; i < canvas.rows * canvas.cols; i++) {
    canvas.cells[i] = ' ';
  }

  draw_tree(&canvas, root, 0, 0, false);

  // left padding that centers the tree
  int padding = (columns - canvas.cols) / 2;

  for (int row = 0; row < canvas.rows; row++) {
    uint32_t *cells = &canvas.cells[row * canvas.cols];
    int end = canvas.cols;

    // trailing spaces are not printed
    while (end > 0 && cells[end - 1] == ' ') {
      end--;
    }

    printf("%*s", padding, "");

    for (int col = 0; col < end; col++) {
      print_utf8(cells[col]);
    }

    printf("\n");
  }

  free(canvas.cells);
  free_layout(root);

  return true;
}
