#ifndef CHAR_LIST_LIST_H
#define CHAR_LIST_LIST_H

#include <stdlib.h>

#define STATIC_CONST_CHAR_LIST(name, ...)                            \
  static char const name[] = {__VA_ARGS__};                          \
  static size_t const name##_COUNT = sizeof(name) / sizeof(name[0]); \
  static char_list_t const name##_LIST = {.chars = name, .count = name##_COUNT}

#define CHAR_LIST_LIST_NODE(name, l) \
  char_list_list_t name = {.list = (l), .prev = NULL, .next = NULL}

typedef struct char_list_s {
  char const* const chars;
  size_t const count;
} char_list_t;

typedef struct char_list_list_s {
  char_list_t const* const list;
  struct char_list_list_s* next;
  struct char_list_list_s* prev;
} char_list_list_t;

char_list_list_t* cll_begin(char_list_list_t* l);
char_list_list_t* cll_end(char_list_list_t* l);
char_list_list_t* cll_add(char_list_list_t* l, char_list_list_t* n);
char cll_index(char_list_list_t* l, unsigned int idx);
int cll_length(char_list_list_t* l);

#endif
