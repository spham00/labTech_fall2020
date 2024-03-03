/* view.c -- view module for clock project (the V in MVC)
 *
 * Darren Provine, 17 July 2009
 * Sarah Pham, 26 November 2021
 *    Set timeformat to "no format" to give it some value so the
 *        program doesn't try to use it when it's null.
 *    Corrected differences between 2021/11/17 code from class and
 *        current script.
 *    Added proper formatting for DATE_MODE and AMPM_MODE.
 *
 *    Corrected some of the hexadecimals for number displays.
 *    AM/PM/Date/24H modes show appropriately in the display.
 *    Added DATE_MODE and TEST_MODE capabilities for LED display.
 *
 * Copyright (C) Darren Provine, 2009-2019, All Rights Reserved
 */

#include "clock.h"
#include "view.h"
#include "time.h"

/* see "view.h" for list of bits that set properties */

int view_props = 0x00; // default is 24-hour mode, plain text

// returns old properties so you can save them if needed
void set_view_properties(int viewbits)
{
    view_props = viewbits;
}

int get_view_properties()
{
    return view_props;
}


void do_test(struct tm *dateinfo)
{
    // turn display bits on and off
    digit *where = get_display_location();
    if ( dateinfo->tm_sec % 2 == 1 ) {
	where[0] = 0x7f;
	where[1] = 0x7f;
	where[2] = 0x7f;
	where[3] = 0x7f;
	where[4] = 0x7f;
	where[5] = 0x7f;
	where[6] = 0x7f;
	where[7] = 0xff;
    } else {
	where[0] = 0x00;
	where[1] = 0x00;
	where[2] = 0x00;
	where[3] = 0x00;
	where[4] = 0x00;
	where[5] = 0x00;
	where[6] = 0x00;
	where[7] = 0x00;
    }

    display();
    fflush(stdout);
}

#define MAX_TIMESTR 40 // big enough for any valid data
// make_timestring
// returns a string formatted from the "dateinfo" object.
char * make_timestring (struct tm *dateinfo, int dividers)
{
    // Declaring the pointer like this can lead to bugs!
    // This crashes if it's not set!
    // It will crash in class and we'll see how to fix it!
    // NEVER DO THIS! (Don't declare it as just "char *timeformat")
    char *timeformat = "(no format)"; // see strftime(3)

    // allocate memory for timeformat
    timeformat = (char*) calloc(80, sizeof(char));
    
    if ( view_props & DATE_MODE ) {
        // if dividers is true:
        //   make a string such as "10/31/12 dt" or " 3/17/12 dt"
        //   (note: no leading zero on month!)
        // if dividers is false:
        //   make a string such as "103112d" or " 31712d"
        //   (note: no leading zero on month!)
        // 
	
        if ( dividers ) {
            strftime(timeformat, 80, "%m/%d/%Y dt", dateinfo);
	} else {
            strftime(timeformat, 80, "%m%d%yd", dateinfo);
       	}

    } else {
        // if dividers is true:
        //   am/pm: make a string such as "11:13:52 am" or " 4:21:35 pm"
        //          (note: no leading zero on hour!)
        //   24 hr: make a string such as "14:31:25 24"
        //          (include leading zero on hour)
        // if dividers is false:
        //   am/pm: make a string such as "111352a" or " 42135p"
        //          (note: no leading zero on hour!)
        //   24 hr: make a string such as "1431252"
        //          (include leading zero on hour)
        // see strftime(3) for details
	
        if ( dividers ) {
            if ( view_props & AMPM_MODE ) {
                strftime(timeformat, 80, "%I:%M:%S %p", dateinfo);
		
		// lowercase the last two letters for am and pm
	        int len = strlen(timeformat);
	        for (int i = len - 2; timeformat[i]; i++) {
		    timeformat[i] = tolower(timeformat[i]);
	        }

            } else {
                timeformat = "%H:%M:%S 24";
            }
	    
        } else {
	    if ( view_props & AMPM_MODE ) {
                strftime(timeformat, 80, "%I%M%S%p", dateinfo);
	        
		// lowercase the last letter for a and p
		int len = strlen(timeformat) - 1;
	        timeformat[len] = '\0';
	        timeformat[len - 1] = tolower(timeformat[len - 1]);
	       
	    } else {
	        timeformat = "%H%M%S2";
            }
	}
    }

    if ( AMPM_MODE || DATE_MODE ) {
	// remove the leading zero and add a space instead
	if ( timeformat[0] == '0' ) {
	    timeformat[0] = ' ';
	}
    }

    // make the timestring and return it
    static char timestring[MAX_TIMESTR];
    strftime(timestring, MAX_TIMESTR, timeformat, dateinfo);
    return timestring;
}

/* We get a pointer to a "struct tm" object, put it in a string, and
 * then send it to the screen.
 */
void show_led(struct tm *dateinfo)
{

    digit *where = get_display_location();
    int i;
    digit  bitvalues = 0;
    int hour;
    int indicator;

    if ( view_props & TEST_MODE ) {
        do_test(dateinfo);
        return;
    }
    
    // The display of each number in LED
    for (i = 0; i < 6; i++) {
        switch ( make_timestring(dateinfo, 0)[i] ) {
            case ' ': bitvalues = 0x00; break;
            case '1': bitvalues = 0x24; break;
            case '2': bitvalues = 0x5d; break;
            case '3': bitvalues = 0x6d; break;
            case '4': bitvalues = 0x2e; break;
            case '5': bitvalues = 0x6b; break;	// 0110 1011
            case '6': bitvalues = 0x7b; break;	// 0111 1011
            case '7': bitvalues = 0x25; break;	// 0010 0101
            case '8': bitvalues = 0x7f; break;
            case '9': bitvalues = 0x2f; break;	// 0010 1111
            case '0': bitvalues = 0x77; break;	// 0111 0111
        }
        where[i] = bitvalues;
    }

    // colons blink
    if ( view_props & DATE_MODE ) {
	// if in date mode, colons don't appear
	where[7] = 0x00;
    }
    else if ( dateinfo->tm_sec % 2 == 0 ) {
	where[7] = 0xf0;
    } else {
	where[7] = 0x00;
    }

    // am/pm/24hr/date
    if ( view_props & DATE_MODE ) {
	where[7] |= 0x01;
    } else if ( view_props & AMPM_MODE ) {
	char buffer[80];
	strftime(buffer, 80, "%p", dateinfo);
	if ( strcmp("PM", buffer) == 0) {
	    where[7] |= 0x04;
	} else {
	    where[7] |= 0x08;
        }
    } else {
	where[7] |= 0x02;
    }

    display();
    fflush(stdout);
}

void show_text(struct tm *dateinfo)
{
    printf("\r%s ", make_timestring(dateinfo, 1));
    fflush(stdout);
}


void show(struct tm *dateinfo)
{
    if ( view_props & LED_MODE )
        show_led(dateinfo);
    else
        show_text(dateinfo);        
}
