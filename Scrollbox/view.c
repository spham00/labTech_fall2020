/* view.c -- display for scrolling box
 *
 * Darren Provine, 20 March 2012
 *
 * Sarah Pham, 8 December 2021
 *     Added scroll functionality.
 *     Added "view.h" capabilities with respective modes.
 *     Added "do_time()" and "do_test()" functions for those modes.
 *     Added "make_timestring()" function for time mode.
 */

#include "scroll.h"
#include "LEDots.h"
#include "Chars/All_Chars.h"
#include "view.h"

/* see "view.h" for list of bits that set properties */
int view_props = 0x00;

// returns old properties to save them if needed and sets
void set_view_properties(int viewbits)
{
    view_props = viewbits;
}

int get_view_properties()
{
    return view_props;
}

// This function makes a timestring for the display of the time
char * make_timestring(struct tm *dateinfo)
{
    char *timeformat = "(no format)";
    timeformat = (char*) calloc(80, sizeof(char));

    // format: "HH:MM:SS " 
    timeformat = "%H:%M:%S ";
    static char timestring[40];
    strftime(timestring, 40, timeformat, dateinfo);
    return timestring;
}

// This function displays the time in the display and calls
// end_time() to ensure that the time display ends after 5 seconds.
void do_time(struct tm *dateinfo)
{
    byte *where = get_display_location();
    
    // for each position in the display, display that
    for(int i = 0; i < 9; i++) {
	int c = (int) make_timestring(dateinfo)[i];
	int index = c - 0x20;
	
	// cycle through the lines of each character
	for (int line = 0; line < 10; line ++) {
	    where[i + line * 9] = (*glyph[index])[line];
	}
    }

    end_time();

    display();
    fflush(stdout);
}

// This function causes characters to flash on and off the screen
// to test all the LEDs.
void do_test(struct tm *dateinfo)
{
    int 	position;
    byte *where = get_display_location();

    // blink characters every other second on all positions
    if ( dateinfo->tm_sec % 2 == 1 ) {
	for ( position = 0; position < 9; position++) {
	    for (int line = 0; line < 10; line ++) {
		// turns every bit in the display on
		where[position + line * 9] = 0xff;
	    }
	}
    }
    else {
	for ( position = 0; position < 9; position++) {
	    for (int line = 0; line < 10; line ++) {
		// turns every bit in the display off
		where[position + line * 9] = 0x00;
	    }
	}
    }

    end_time();

    display();
    fflush(stdout);
}

void show(char *text)
{
    int position; // where in the string are we?
    int line;     // which line are we on?
    byte *where;  // where in memory are we putting the bits?
    int c;        // what character are we doing?
    int index;    // where is the character in the glyph[] array?

    // do something else if test or time buttons clicked
    if ( view_props & ( MODE_TEST | MODE_TIME )) {
	local_time();
	return;
    }

    where = get_display_location();

    if (debug) {
        fprintf(stderr, " text:|%s|", text);
        fflush(stderr);
    }

    // start at the rightmost position
    position = 8;

    // Characters start at " ", which is ASCII 0x20
    // but is in the array at location 0.
    c = (int) text[position];
    if (debug) {
        fprintf(stderr, "-%c", c);
        fflush(stderr);
    }

    // shift each character in the text to the left
    for ( ; position > -1; position--) {
        c = (int) text[position];
        index = c - 0x20;
        for (line=0; line < 10; line++) {
            where[position + line * 9] = (*glyph[index])[line];
        }
    }
    
    if (debug) {
        fprintf(stderr, "-\r");
        fflush(stderr);
    }

    display();
    fflush(stdout);
}

