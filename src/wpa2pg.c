#include <argp.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>

#include "char_list_list.h"

typedef struct {
  bool exclude_special;
  bool exclude_ambiguous;
  bool exclude_capital;
  char *exclude_extra;
} pwd_props_t;

typedef struct {
  unsigned int length;
  pwd_props_t pwd_props;
} arguments_t;

static error_t parse_opt(int key, char *arg, struct argp_state *state);
char get_random_char(const pwd_props_t *props);
void generate_password(char *buffer, size_t buffer_size,
                       const pwd_props_t *props);
void freePtr(void **p);

const char *argp_program_version = "wpa2pg 1.4.1";
const char *argp_program_bug_address = "<de.toolcreator@gmail.com>";
static const char doc[] = "WPA2 password generator";
static const char args_doc[] = "LENGTH";
static const struct argp_option options[] = {
    {"exclude-special", 's', 0, 0, "Exclude special characters"},
    {"exclude-ambiguous", 'a', 0, 0, "Exclude ambiguous characters"},
    {"exclude-capital", 'c', 0, 0, "Exclude capital letters"},
    {"exclude-extra", 'e', "CHAR_STRING", 0, "More characters to exclude"},
    {0}};
static const struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char *argv[]) {
  arguments_t args = {.length = 63,
                      .pwd_props = {.exclude_special = false,
                                    .exclude_ambiguous = false,
                                    .exclude_capital = false,
                                    .exclude_extra = NULL}};
  argp_parse(&argp, argc, argv, 0, 0, &args);

  const size_t pwd_str_size = args.length + 1;
  char *pwd_str = calloc(pwd_str_size, sizeof(char));
  if (pwd_str != NULL) {
    generate_password(pwd_str, pwd_str_size, &args.pwd_props);
    printf("%s\n", pwd_str);

    freePtr((void **)&pwd_str);
  } else {
    fprintf(stderr, "Failed to allocate memory for a password of length %lu\n",
            pwd_str_size - 1);
  }

  if (args.pwd_props.exclude_extra != NULL) {
    freePtr((void **)&args.pwd_props.exclude_extra);
  }

  return 0;
}

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  arguments_t *arguments = state->input;

  switch (key) {
  case 's':
    arguments->pwd_props.exclude_special = true;
    break;
  case 'a':
    arguments->pwd_props.exclude_ambiguous = true;
    break;
  case 'c':
    arguments->pwd_props.exclude_capital = true;
    break;
  case 'e': {
    char **extra = &arguments->pwd_props.exclude_extra;
    *extra = calloc(strlen(arg) + 1, sizeof(char));
    if (extra != NULL) {
      strcpy(*extra, arg);
    } else {
      fprintf(stderr,
              "Failed to allocate memory for extra excluded characters\n");
      return 1;
    }
    break;
  }
  case ARGP_KEY_ARG:
    if (state->arg_num >= 1) {
      argp_usage(state);
    }

    const int parsed_length = atoi(arg);
    if (parsed_length > 0) {
      arguments->length = parsed_length;
    } else {
      argp_usage(state);
    }

    break;

  case ARGP_KEY_END:
    break;

  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

char get_random_char(const pwd_props_t *props) {
  STATIC_CONST_CHAR_LIST(LOWER_CASE_UNAMBIGUOUS_LETTERS, 'a', 'b', 'c', 'd',
                         'e', 'f', 'g', 'h', 'i', 'j', 'k', 'm', 'n', 'o', 'p',
                         'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z');
  STATIC_CONST_CHAR_LIST(LOWER_CASE_AMBIGUOUS_LETTERS, 'l');
  STATIC_CONST_CHAR_LIST(UPPER_CASE_UNAMBIGUOUS_LETTERS, 'A', 'B', 'C', 'D',
                         'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'R',
                         'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z');
  STATIC_CONST_CHAR_LIST(UPPER_CASE_AMBIGUOUS_LETTERS, 'I', 'O', 'Q');
  STATIC_CONST_CHAR_LIST(UNAMBIGUOUS_DIGITS, '1', '2', '3', '4', '5', '6', '7',
                         '8', '9');
  STATIC_CONST_CHAR_LIST(AMBIGUOUS_DIGITS, '0');
  STATIC_CONST_CHAR_LIST(UNAMBIGUOUS_SPECIAL_CHARS, '!', '?', '*', '#', '_',
                         '.', '-', ',', '$', '%', '&', '/', '^', '@', '~', '=',
                         '+', ':');

  CHAR_LIST_LIST_NODE(lcUnambLets, &LOWER_CASE_UNAMBIGUOUS_LETTERS_LIST);
  CHAR_LIST_LIST_NODE(lcAmbLets, &LOWER_CASE_AMBIGUOUS_LETTERS_LIST);
  CHAR_LIST_LIST_NODE(ucUnambLets, &UPPER_CASE_UNAMBIGUOUS_LETTERS_LIST);
  CHAR_LIST_LIST_NODE(ucAmbLets, &UPPER_CASE_AMBIGUOUS_LETTERS_LIST);
  CHAR_LIST_LIST_NODE(unambDigs, &UNAMBIGUOUS_DIGITS_LIST);
  CHAR_LIST_LIST_NODE(ambDigs, &AMBIGUOUS_DIGITS_LIST);
  CHAR_LIST_LIST_NODE(unambSpecs, &UNAMBIGUOUS_SPECIAL_CHARS_LIST);

  char_list_list_t *allowed = &lcUnambLets;
  allowed = cll_add(allowed, &unambDigs);

  if (!props->exclude_special) {
    allowed = cll_add(allowed, &unambSpecs);
  }

  if (!props->exclude_ambiguous) {
    allowed = cll_add(allowed, &lcAmbLets);
    allowed = cll_add(allowed, &ambDigs);

    if (!props->exclude_capital) {
      allowed = cll_add(allowed, &ucAmbLets);
    }
  }

  if (!props->exclude_capital) {
    allowed = cll_add(allowed, &ucUnambLets);
  }

  allowed = cll_begin(allowed);
  const size_t allowed_chars_count = cll_length(allowed);
  char idx = allowed_chars_count;

  static const unsigned int MAX_ITERATIONS = 10000;
  unsigned int i = 0;
  do {
    getrandom(&idx, sizeof(idx), GRND_NONBLOCK);
  } while ((idx >= allowed_chars_count ||
            (props->exclude_extra != NULL
                 ? strchr(props->exclude_extra, cll_index(allowed, idx)) != NULL
                 : false)) &&
           ++i < MAX_ITERATIONS);

  if (i == MAX_ITERATIONS) {
    printf("Character selection is too restrictive, giving up\n");
    exit(EXIT_FAILURE);
  }

  const char c = cll_index(allowed, idx);
  return c;
}

void generate_password(char *buffer, size_t buffer_size,
                       const pwd_props_t *props) {
  buffer_size -= 1;
  for (unsigned int i = 0; i < buffer_size; ++i) {
    buffer[i] = get_random_char(props);
  }
  buffer[buffer_size] = '\0';
}

void freePtr(void **p) {
  free(*p);
  *p = NULL;
}
