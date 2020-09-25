#include <stdio.h>
#include <stdint.h>
#include <sys/random.h>

char get_random_char(void);
void generate_password(char *buffer, size_t buffer_size);

int main() {
  const size_t pwd_str_size = 64;
  char pwd_str[pwd_str_size];
  generate_password(pwd_str, pwd_str_size);
  printf("%s\n", pwd_str);
  return 0;
}

char get_random_char(void) {
  const char ALLOWED_CHARS[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '!', '?', '*', '#', '_', '.', '-', ',', '$', '%', '&', '/', '^', '@', '~', '=', '+', ':'
  };
  const size_t NUM_ALLOWED_CHARS = sizeof(ALLOWED_CHARS) / sizeof(ALLOWED_CHARS[0]);

  uint8_t idx = NUM_ALLOWED_CHARS;
  do {
    getrandom(&idx, 1, GRND_NONBLOCK);
  } while(idx >= NUM_ALLOWED_CHARS);

  return ALLOWED_CHARS[idx];
}

void generate_password(char *buffer, size_t buffer_size) {
  buffer_size -= 1;
  for(unsigned int i = 0; i < buffer_size; ++i) {
    buffer[i] = get_random_char();
  }
  buffer[buffer_size] = '\0';
}
