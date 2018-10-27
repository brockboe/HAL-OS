#include "types.h"

#define VIDMEM  0x000B8000
#define TERMHEIGHT 25
#define TERMWIDTH 80
#define MAXCHAR TERMHEIGHT * TERMWIDTH

typedef struct vid_data {
      uint8_t character;
      uint8_t highbits;
} vid_data_t;

typedef struct terminal_info {
      uint32_t offset;
} terminal_info_t;

void vid_init();

void clear_term();

void print_term(uint8_t * string, int length);

void printchar_term(char a);

void backspace();
