#include "char_list_list.h"

char cll_index(const char_list_list_t* c, unsigned int idx) {
  if (!c || c->length < 1) {
    return -1;
  }

  unsigned int sum = 0;
  unsigned int list_idx = 0;
  for (; list_idx < c->length; ++list_idx) {
    sum += c->lists[list_idx]->count;
    if (idx < sum) {
      break;
    }
  }
  if (idx >= sum) {
    return -1;
  }

  char_list_t const* const target_list = c->lists[list_idx];
  const unsigned int base = sum - target_list->count;
  const unsigned int target_idx = idx - base;

  return target_list->chars[target_idx];
}

int cll_length(const char_list_list_t* c) {
  if (!c) {
    return -1;
  }

  int length = 0;
  for (unsigned int i = 0; i < c->length; ++i) {
    length += c->lists[i]->count;
  }
  return length;
}
