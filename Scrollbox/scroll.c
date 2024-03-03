/* scroll.c -- scrolling display
 *
 * Darren Provine, 20 March 2012
 *
 * Sarah Pham, 8 December 2021
 * 	Added code to handle keystrokes and mouse clicks.
 * 	Added function "end_time()" to end the time after 5 secs pass.
 */

#include <time.h>
#include <stdio.h>

// sleep needs this
#include <unistd.h>

#include "scroll.h"
#include "view.h"

// debug flag - changed via -D option
int debug = 0;

static char bugaddress[]="kilroy@elvis.rowan.edu";

// version -- say which version this is and exit
// (note simple output; you could also print the rcsid too)
void version()
{
    fprintf(stderr, "scroll version 1\n");
    exit(0);
}

// usage -- print brief summary and instructions
void usage(char *progname)
{
    fprintf(stderr, "This program shows a scrolling box.\n");
    fprintf(stderr, "Usage: %s [-dDvh] text\n", progname);
    fprintf(stderr,
            "  -d # : set scroll delay to # milliseconds (default=1000)\n");
    fprintf(stderr,
            "  -D   : turn on debug messages (more Ds = more debugging)\n");
    fprintf(stderr, "  -v   : show version information\n");
    fprintf(stderr, "  -h   : this help message\n");
    fprintf(stderr, "Use quotes for multi-word messages, 'like this'.\n");
    fprintf(stderr, "report bugs to %s \n", bugaddress);
    exit (0);
}

void exit_scroll()
{
    end_display();
    exit(0);
}

int time_mode_end;
int test_mode_end;

// keyboard and mouse handling
void process_key(keybits KeyCode)
{
    int row, col;
    int view_props;
    time_t now;

    if ( KeyCode & 0x80 ) { // mouse click
        row = (KeyCode & 0x70) >> 4;
        col = (KeyCode & 0x0f);

	if (row == 0 && (col == 0 || col == 1)) { // "fast" and "slow"
	    int check;	  // stores the difference/sum of delay for now

	    // calculate delay and store in local variable 
	    if (col == 0) { // "fast"
		check = get_delay() - 500;
	    } else { // "slow"
	    	check = get_delay() + 500;
	    }
	    
	    // checking limits of delay before setting
	    if (check >= 100 && check <= 5000) {
		start_timer(check);
	    }
	}

	if (row == 0 && col == 2) { // "time"
	    time_mode_end = time(&now) + 5;
	    view_props = get_view_properties();
	    view_props |= ( MODE_TIME );
	    set_view_properties (view_props); 
	}
	
	if (row == 0 && col == 3) { // "test"
	    test_mode_end = time(&now) + 5;
	    view_props = get_view_properties();
	    view_props |= ( MODE_TEST );
	    set_view_properties (view_props); 
	}

        if (row == 0 && col == 4) { // "off"
            exit_scroll();
        }

    } else { // keyboard press
	int check;
        switch(KeyCode) {
	    case 'f': // fast
		check = get_delay() - 500;
		if (check >= 100) {
		    start_timer(check);
		}
		break;
	    case 's': //slow
		check = get_delay() + 500;
		if (check <= 5000) {
		    start_timer(check);
		}
		break;
	    case 'c': // time ('c' stands for clock)
	        time_mode_end = time(&now) + 5;
	        view_props = get_view_properties();
	        view_props |= ( MODE_TIME );
	        set_view_properties (view_props); 
		break;
	    case 't': // test
	        test_mode_end = time(&now) + 5;
	        view_props = get_view_properties();
	        view_props |= ( MODE_TEST );
	        set_view_properties (view_props);
	        break;	
            case 'q': // off
	        exit_scroll();
		break;
        }
    }
}

int main(int argc, char *argv[])
{
    int   initial_delay = 1000;
    int   letter; 		// option char, if any
    // note C does automatic concatenation of long strings
    char  title[81] =
               "----------------------------  "
               "Sarah Pham was here!"
               "  ----------------------------";

    // loop through all the options; getopt() can handle together or apart
    while ( ( letter = getopt(argc, argv, "d:Dvh")) != -1 ) {
        switch (letter) {
            case 'd':  initial_delay = atoi(optarg); break;
            case 'D':  debug++;                      break;
            case 'v':  version();                    break;
            case 'h':  usage(argv[0]);               break;

            case '?':  // unknown flag; fall through to next case
            default:   // shouldn't happen, but Just In Case
                       // note that getopt() warns about the unknown flag
                  fprintf(stderr, "run \"%s -h\" for help\n", argv[0]);
                  exit(1);
        }
    }

    // optind is the first argument after options are processed
    // if there aren't any, the user didn't give a message to scroll
    if (optind == argc) {
        usage(argv[0]);
    }

    // put the information from the command line into the module
    setup(argv[optind]);

    // set up the view
    register_keyhandler(process_key);
    start_display();
    set_title_bar(title);

    // turn on some keys in row 2, uncomment if needed
    //set_key_text(0, "foobar");
    //set_key_text(1, "2nd");

    // start the model running
    start_timer(initial_delay);

    // wait for the model to signal controller
    while ( 1 ) {
        get_key();
    }

    end_display();

    return 0;
}


/* This is the function the model uses to signal the controller
 * that there's work to do.
 */
void timer(int sig)
{
    char   *segment;

    segment = display_string();
    
    if (debug >= 3) {
        fprintf(stderr, "display_string() returned |%s|\r\n", segment);
        sleep(2);
    }
    /* Send those chars to the view. */
    show(segment);
    get_key();
}

// This function ends the time or test modes if 5 seconds have passed.
void end_time()
{
    int view_props;
    time_t now;

    (void) time(&now);

    if (MODE_TIME && now > time_mode_end) {
	view_props = get_view_properties();
	view_props &= ( ~MODE_TIME );
	set_view_properties(view_props);
    }

    if (MODE_TEST && now > test_mode_end) {
	view_props = get_view_properties();
	view_props &= ( ~MODE_TEST );
	set_view_properties(view_props);
    }
}
