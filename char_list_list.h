#ifndef CHAR_LIST_LIST_H
#define CHAR_LIST_LIST_H

#include <stdlib.h>

#define STATIC_CONST_CHAR_LIST(n, ...)                      \
  static const char n[] = {__VA_ARGS__};                    \
  static const size_t n##_COUNT = sizeof(n) / sizeof(n[0]); \
  static const char_list_t n##_LIST = {.chars = n, .count = n##_COUNT}

#define STATIC_CONST_CHAR_LIST_LIST(n, l, ...)                 \
  static char_list_t const* const n##_ARRAY[] = {__VA_ARGS__}; \
  static const char_list_list_t n = {.lists = n##_ARRAY, .length = l}

typedef struct {
  char const* const chars;
  const size_t count;
} char_list_t;

typedef struct {
  char_list_t const* const* const lists;
  const size_t length;
} char_list_list_t;

char cll_index(const char_list_list_t* c, unsigned int idx);
int cll_length(const char_list_list_t* c);

#endif
