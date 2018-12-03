#include "types.h"

#define VIDMEM  0x000B8000
#define TERMHEIGHT 25
#define TERMWIDTH 80
#define MAXCHAR TERMHEIGHT * TERMWIDTH


/* struct of video memory data */
typedef struct vid_data {
      uint8_t character;
      uint8_t highbits;
} vid_data_t;


/* struct containing offset of terminal display */
typedef struct terminal_info {
      uint32_t offset;
      uint8_t cursor_start;
      uint8_t cursor_end;
} terminal_info_t;

void vid_init();                               /* initialization of video display     */

void clear_term();                             /* clears the terminal                 */

void print_term(uint8_t * string, int length); /* prints string to terminal           */

void printchar_term(char a);                   /* prints character to terminal        */

void backspace();                              /* deletes last character              */

void tab();                                    /* indents offset by tab               */

void set_term_x(uint32_t x);                   /* adjusts terminal offset by x        */

void print_num(int x);                         /* test function for printing numbers  */

void fill_color();                             /* fills display with red              */

void move_cursor();                             /* moves the cursor to the offset specified by tinfo*/
