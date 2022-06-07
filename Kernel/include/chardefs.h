#ifndef _CHARDEFS_H_
#define _CHARDEFS_H_

#define PRINTABLE(x) ((x) != CAPS_LOCK && (x) != UP_ARROW && (x) != DOWN_ARROW && (x) != LEFT_ARROW && (x) != RIGHT_ARROW && (x) != ALT )

#define IS_ALPHA(x) ( ( (x) >='a' && (x) <= 'z') || ( (x)>='A' && (x) <= 'Z') ) 


#endif