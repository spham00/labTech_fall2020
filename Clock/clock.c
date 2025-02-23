/* clock.c -- controller part of the clock project
 *
 * Darren Provine, 17 July 2009
 *
 * Sarah Pham, 26 November 2021
 *     Added name to display on clock.
 *     Implemented date and test mode functionality.
 *
 * Copyright (C) Darren Provine, 2009-2019, All Rights Reserved
 */

#include "clock.h"
#include <limits.h>

/* CONTROLLER */

static char bugaddress[]="kilroy@elvis.rowan.edu";

// version -- say which version this is and exit
// (note simple output)
void version()
{
    fprintf(stderr, "simple clock Fall 2021\n");
    exit(0);
}

// usage -- print brief summary and instructions
void usage(char *progname)
{
    fprintf(stderr, "This program displays a realtime clock.\n");
    fprintf(stderr, "Usage: %s [-advh] [-o number]\n", progname);
    fprintf(stderr, "  -a    : am/pm instead of 24 hour\n");
    fprintf(stderr, "  -d    : show date instead of time\n");
    fprintf(stderr, "  -l    : use simulated LED display\n");
    fprintf(stderr, "  -o #  : offset the time by # seconds \n");
    fprintf(stderr, "  -v    : show version information\n");
    fprintf(stderr, "  -h    : this help message\n");
    fprintf(stderr, "report bugs to %s \n", bugaddress);
    exit (0);
}

// These store timestamps for when the different modes end
int test_mode_end;
int date_mode_end = INT_MAX;

void process_key(keybits KeyCode)
{
    void stop_clock(void);
    int KeyRow, KeyCol;
    int view_props;
    time_t now;
    
    if ( ( KeyCode & 0x00ff ) == 0 ) {  // no ASCII code, so mouse hit

	KeyCol = KeyCode >> 12;
	KeyRow = (KeyCode >> 8) & 0xf;

        if (KeyRow == 0) {
            switch (KeyCol) {
                case 0: // 24-hour mode
                    view_props = get_view_properties();
                    view_props &= ( ~ AMPM_MODE );
                    set_view_properties (view_props);
                    break;
                case 1: // AM/PM mode
                    view_props = get_view_properties();
                    view_props |= ( AMPM_MODE );
                    set_view_properties (view_props);
                    break;
                case 2: // date mode
		    date_mode_end = time(&now) + 5;
		    view_props = get_view_properties();
		    view_props |= ( DATE_MODE );
		    set_view_properties (view_props);
                    break;
                case 3: // test mode
		    test_mode_end = time (&now) + 5;
		    view_props = get_view_properties();
		    view_props |= ( TEST_MODE );
		    set_view_properties (view_props);
                    break;
                case 4: // off
                    stop_clock();
                    break;
            }
        } else if (KeyRow == 1) { // Other buttons (sample, foobar, etc.)
            switch (KeyCol) {
                case 0:
                    break;
            }
        }        
    } else { // keystroke
        // figure out ASCII value from first 8 bits
        // right now any key goes to 24-hour mode
        //KeyCode = '2';
        switch( KeyCode ) {
            case '2': // 24-hr mode
                view_props = get_view_properties();
                view_props &= ( ~ AMPM_MODE );
                set_view_properties (view_props);
                break;
            case 'a': // AM/PM mode
                view_props = get_view_properties();
                view_props |= ( AMPM_MODE );
                set_view_properties (view_props);
                break;
            case 'd': // date mode
		// set the bit
		date_mode_end = time(&now) + 5;
		view_props = get_view_properties();
		view_props |= ( DATE_MODE );
		set_view_properties (view_props);
                break;
            case 't': // test mode
		test_mode_end = time (&now) + 5;
		view_props = get_view_properties();
		view_props |= ( TEST_MODE );
		set_view_properties (view_props);
                break;
            case 'q': // off
                stop_clock();
                break;
        }
    }

    // force update when keys are hit
    tick(0);
}

void stop_clock()
{
    end_display();
    exit(0);
}

int main(int argc, char *argv[])
{
    int letter;  // option character

    // next three are for setting view properties
    int view_props;
    int ampm = 0;     // default to 24hr
    int date = 0;     // default to time
    int LED  = 0;     // default to text

    // loop through all the options; getopt() can handle together or apart
    while ( ( letter = getopt(argc, argv, "adlo:vh")) != -1 ) {
        // *INDENT-OFF*
        switch (letter) {
            case 'a':  ampm = 1;               break;
            case 'd':  date = 1;               break;                
            case 'l':  LED  = 1;               break;
            case 'o':  set_offset (atoi(optarg));  break;
            case 'v':  version();              break;
            case 'h':  usage(argv[0]);         break;

            case '?':  // unknown flag; fall through to next case
            default:   // shouldn't happen, but Just In Case
                       // note that getopt() warns about the unknown flag
                  fprintf(stderr, "run \"%s -h\" for help\n", argv[0]);
                  exit(1);
        }
        // *INDENT-ON*
    }

    // All the flags have been scanned.
    // "optind" is now the index of the first non-option item

    // set view properties; default is "24 hour text mode"
    view_props = 0;  // all bits off
    if ( ampm )
        view_props = AMPM_MODE;
    if ( date )
        view_props |= DATE_MODE;  // note |= to switch on a bit
    if ( LED )
        view_props |= LED_MODE;

    set_view_properties (view_props);

    if (LED) { // set up the fancy display
        start_display();
        // has to be exactly 78 chars
        set_title_bar("----------------------------"
                      " Sarah Pham was here at: "
                      "----------------------------");
        register_keyhandler(process_key);

        // turn on some keys in row 2,
        // or comment these out if unused
        set_key_text(0, "Sample");
        set_key_text(1, "Button");
        set_key_text(2, "foobar");
        set_key_text(3, "blurfl");
    }

    /* get the model running */
    start_timer();

    while (1) {
        if (LED) {
            get_key();
        } else {
            pause(); // wait for signal
        }
    }

    /* no return because never reached */
}


/* This function is called is called by the model when a new
 * time is ready for display.
 */
void new_time(struct tm *dateinfo)
{
    int view_props;
    time_t now;

    (void) time(&now);

    // handle date mode
    if ( DATE_MODE && now > date_mode_end ) {
        // put in code to turn off DATE bit
	view_props = get_view_properties();
	view_props &= ( ~DATE_MODE );
	set_view_properties (view_props);
    }
    if ( TEST_MODE && now > test_mode_end ) {
        // put in code to turn off TEST bit
	view_props = get_view_properties();
	view_props &= ( ~TEST_MODE );
	set_view_properties (view_props);
    }    

    show(dateinfo);
}
