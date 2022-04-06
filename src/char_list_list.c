#include "char_list_list.h"

char_list_list_t* cll_begin(char_list_list_t* l) {
  while (l->prev) {
    l = l->prev;
  }
  return l;
}
char_list_list_t* cll_end(char_list_list_t* l) {
  while (l->next) {
    l = l->next;
  }
  return l;
}

char_list_list_t* cll_add(char_list_list_t* l, char_list_list_t* n) {
  char_list_list_t* end = cll_end(l);
  end->next = n;
  n->prev = end;
  n->next = NULL;
  return n;
}

char cll_index(char_list_list_t* l, unsigned int idx) {
  l = cll_begin(l);

  unsigned int sum = 0;
  while (l->list) {
    sum += l->list->count;
    if (idx < sum) {
      break;
    }

    if (l->next) {
      l = l->next;
    } else {
      break;
    }
  }

  if (idx >= sum) {
    return 0x15;  // ASCII NAK
  }

  char_list_t const* const target_list = l->list;
  const unsigned int base = sum - target_list->count;
  const unsigned int target_idx = idx - base;

  return target_list->chars[target_idx];
}

int cll_length(char_list_list_t* l) {
  l = cll_begin(l);

  unsigned int list_length = 0;
  while (l->list) {
    list_length += l->list->count;

    if (l->next) {
      l = l->next;
    } else {
      break;
    }
  }

  return list_length;
}
