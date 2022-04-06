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
} pwd_props_t;

typedef struct {
  size_t pwd_len;
  pwd_props_t pwd_props;
} args_t;

args_t *get_args(int argc, const char *argv[]);
void print_help();

char get_random_char(const pwd_props_t *props);
void generate_password(char *buffer, size_t buffer_size,
                       const pwd_props_t *props);

int main(int argc, char *argv[]) {
  args_t *args = get_args(argc, (const char **)argv);
  if (args != NULL) {
    const size_t pwd_str_size = args->pwd_len + 1;
    char *pwd_str = calloc(pwd_str_size, sizeof(char));
    if (pwd_str != NULL) {
      generate_password(pwd_str, pwd_str_size, &args->pwd_props);
      printf("%s\n", pwd_str);

      free(pwd_str);
      pwd_str = NULL;
    } else {
      printf("Failed to allocate memory for a password of length %lu\n",
             pwd_str_size - 1);
    }

    free(args);
    args = NULL;
  } else {
    print_help();
  }
  return 0;
}

args_t *get_args(int argc, const char *argv[]) {
  args_t *args = calloc(1, sizeof(args_t));
  if (args == NULL) {
    return args;
  }
  args->pwd_len = 63;

  int arg_idx = 1;
  bool error = false;
  for (; arg_idx < argc; ++arg_idx) {
    const char *arg = argv[arg_idx];
    if (arg[0] == '-') {
      const size_t arg_len = strlen(arg);
      if (arg_len > 1) {
        if (arg[1] == '-') {
          if (!strcmp(arg, "--exclude-special")) {
            args->pwd_props.exclude_special = true;
          } else if (!strcmp(arg, "--exclude-ambiguous")) {
            args->pwd_props.exclude_ambiguous = true;
          } else if (!strcmp(arg, "--exclude-capital")) {
            args->pwd_props.exclude_capital = true;
          } else if (!strcmp(arg, "--help")) {
            error = true;
          } else {
            error = true;
          }
        } else {
          for (int i = 1; i < arg_len && !error; ++i) {
            switch (arg[i]) {
              case 's':
                args->pwd_props.exclude_special = true;
                break;
              case 'a':
                args->pwd_props.exclude_ambiguous = true;
                break;
              case 'c':
                args->pwd_props.exclude_capital = true;
                break;
              default:
                error = true;
                break;
            }
          }
        }
      } else {
        error = true;
        break;
      }
    } else {
      errno = 0;
      args->pwd_len = strtoul(argv[arg_idx], NULL, 10);
      if (errno != 0 || args->pwd_len == 0) {
        error = true;
      }
      break;
    }
  }

  error = arg_idx < argc - 1 || error;
  if (error) {
    free(args);
    args = NULL;
  }

  return args;
}

void print_help() {
  printf("Usage: wpa2pg [options] [PWD_LEN]\n");
  printf("\n");
  printf("Options:\n");
  printf(
      "  -s --exclude-special"
      "\t\tExclude special / include only alphanumeric characters\n");
  printf(
      "  -a --exclude-ambiguous"
      "\tExlude ambiguous characters\n");
  printf(
      "  -c --exclude-capital"
      "\t\tExclude capital letters\n");
  printf(
      "  -h --help"
      "\t\t\tPrint this message\n");
  printf("\n");
  printf("PWD_LEN\n");
  printf("  Length of the password (default: 63)\n");
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
  do {
    getrandom(&idx, sizeof(idx), GRND_NONBLOCK);
  } while (idx >= allowed_chars_count);

  return cll_index(allowed, idx);
}

void generate_password(char *buffer, size_t buffer_size,
                       const pwd_props_t *props) {
  buffer_size -= 1;
  for (unsigned int i = 0; i < buffer_size; ++i) {
    buffer[i] = get_random_char(props);
  }
  buffer[buffer_size] = '\0';
}
