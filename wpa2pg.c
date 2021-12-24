#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>

typedef struct {
  bool exclude_special;
  bool exclude_ambiguous;
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
    if (pwd_str_size > 1) {
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
    } else {
      printf("Invalid password length: %s\n", argv[1]);
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
          } else if (!strcmp(arg, "--help")) {
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
      "  -h --help"
      "\t\t\tPrint this message\n");
  printf("\n");
  printf("PWD_LEN\n");
  printf("  Length of the password (default: 63)\n");
}

char get_random_char(const pwd_props_t *props) {
  static const char ALL_CHARS[] = {
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
      'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B',
      'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
      'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3',
      '4', '5', '6', '7', '8', '9', '!', '?', '*', '#', '_', '.', '-', ',',
      '$', '%', '&', '/', '^', '@', '~', '=', '+', ':'};
  static const size_t ALL_CHARS_COUNT =
      sizeof(ALL_CHARS) / sizeof(ALL_CHARS[0]);

  static const char NO_SPECIAL_CHARS[] = {
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
      'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
      'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  static const size_t NO_SPECIAL_CHARS_COUNT =
      sizeof(NO_SPECIAL_CHARS) / sizeof(NO_SPECIAL_CHARS[0]);

  static const char NO_AMBIGUOUS_CHARS[] = {
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'm', 'n',
      'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A',
      'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P',
      'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3',
      '4', '5', '6', '7', '8', '9', '!', '?', '*', '#', '_', '.', '-',
      ',', '$', '%', '&', '/', '^', '@', '~', '=', '+', ':'};
  static const size_t NO_AMBIGUOUS_CHARS_COUNT =
      sizeof(NO_AMBIGUOUS_CHARS) / sizeof(NO_AMBIGUOUS_CHARS[0]);

  static const char NO_AMBIGUOUS_NO_SPECIAL_CHARS[] = {
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'm', 'n', 'o', 'p',
      'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E',
      'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'R', 'S', 'T', 'U', 'V', 'W',
      'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  static const size_t NO_AMBIGUOUS_NO_SPECIAL_CHARS_COUNT =
      sizeof(NO_AMBIGUOUS_NO_SPECIAL_CHARS) /
      sizeof(NO_AMBIGUOUS_NO_SPECIAL_CHARS[0]);

  const char *allowed_chars = NULL;
  size_t allowed_chars_count = 0;
  if (props->exclude_special && props->exclude_special) {
    allowed_chars = NO_AMBIGUOUS_NO_SPECIAL_CHARS;
    allowed_chars_count = NO_AMBIGUOUS_NO_SPECIAL_CHARS_COUNT;
  } else if (props->exclude_special) {
    allowed_chars = NO_SPECIAL_CHARS;
    allowed_chars_count = NO_SPECIAL_CHARS_COUNT;
  } else if (props->exclude_ambiguous) {
    allowed_chars = NO_AMBIGUOUS_CHARS;
    allowed_chars_count = NO_AMBIGUOUS_CHARS_COUNT;
  } else {
    allowed_chars = ALL_CHARS;
    allowed_chars_count = ALL_CHARS_COUNT;
  }

  uint8_t idx = allowed_chars_count;
  do {
    getrandom(&idx, 1, GRND_NONBLOCK);
  } while (idx >= allowed_chars_count);

  return allowed_chars[idx];
}

void generate_password(char *buffer, size_t buffer_size,
                       const pwd_props_t *props) {
  buffer_size -= 1;
  for (unsigned int i = 0; i < buffer_size; ++i) {
    buffer[i] = get_random_char(props);
  }
  buffer[buffer_size] = '\0';
}
