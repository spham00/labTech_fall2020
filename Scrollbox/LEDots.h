/* LEDots.h -- header file for dot matris clock library file
 *
 * Darren Provine, 14 March 2010
 */

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <ncurses.h>
#include <term.h>
#include <stdlib.h>

#ifndef BYTE
typedef unsigned char byte;
#define BYTE
#endif

byte *get_display_location(void);

void start_display(void);
void end_display(void);

void set_title_bar(char *);

void display(void);


void get_key(void);

#ifndef KEYBITS
typedef unsigned char keybits;
#define KEYBITS
#endif

int register_keyhandler( void(*f)(keybits) );

void set_key_text(int, char *);

