#ifndef _CHARDEFS_H_
#define _CHARDEFS_H_

#define MAYUS_DIFF ('a'-'A')
#define CAPS_LOCK 0x3A
#define BACKSPACE '\b'
#define ENTER '\n'
#define UP_ARROW 501
#define DOWN_ARROW 502
#define LEFT_ARROW 503
#define RIGHT_ARROW 504
#define CLEAR_LINE 350
#define ESC 310
#define ALT 340
#define PRINTABLE(x) ((x) != CAPS_LOCK && (x) != UP_ARROW && (x) != DOWN_ARROW && (x) != LEFT_ARROW && (x) != RIGHT_ARROW && (x) != ALT && (x) != 0)
#define ARROW(x) ((x) == UP_ARROW || (x) == DOWN_ARROW || (x) == LEFT_ARROW || (x) == RIGHT_ARROW)

#define IS_ALPHA(x) ( ( (x) >='a' && (x) <= 'z') || ( (x)>='A' && (x) <= 'Z') ) 


#endif