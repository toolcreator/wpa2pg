#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/random.h>

size_t get_pwd_str_size(int argc, char *argv[]);

char get_random_char(void);
void generate_password(char *buffer, size_t buffer_size);

int main(int argc, char *argv[]) {
  const size_t pwd_str_size = get_pwd_str_size(argc, argv);
  if (pwd_str_size != 0) {
    char *pwd_str = calloc(pwd_str_size, sizeof(char));
    if (pwd_str != NULL) {

      generate_password(pwd_str, pwd_str_size);
      printf("%s\n", pwd_str);
      free(pwd_str);

    } else {
      printf("Failed to allocate memory for a password of length %lu\n", pwd_str_size - 1);
    }
  } else {
    printf("Invalid password length: %s\n", argv[1]);
  }
  return 0;
}

size_t get_pwd_str_size(int argc, char *argv[]) {
  size_t pwd_str_size = 64;
  if (argc >= 2) {
    errno = 0;
    char *stroul_endptr = NULL;
    pwd_str_size = strtoul(argv[1], &stroul_endptr, 10) + 1;
    if (errno != 0 || stroul_endptr == argv[1]) {
      pwd_str_size = 0;
    }
  }
  return pwd_str_size;
}

char get_random_char(void) {
  const char ALLOWED_CHARS[] = {
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
      '!', '?', '*', '#', '_', '.', '-', ',', '$', '%', '&', '/', '^', '@', '~', '=', '+', ':'};
  const size_t NUM_ALLOWED_CHARS = sizeof(ALLOWED_CHARS) / sizeof(ALLOWED_CHARS[0]);

  uint8_t idx = NUM_ALLOWED_CHARS;
  do {
    getrandom(&idx, 1, GRND_NONBLOCK);
  } while (idx >= NUM_ALLOWED_CHARS);

  return ALLOWED_CHARS[idx];
}

void generate_password(char *buffer, size_t buffer_size) {
  buffer_size -= 1;
  for (unsigned int i = 0; i < buffer_size; ++i) {
    buffer[i] = get_random_char();
  }
  buffer[buffer_size] = '\0';
}
