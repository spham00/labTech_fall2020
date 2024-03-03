/* view.h - Defines view properties and other functions to be used
 *
 * S Pham, 10 December 2021
 */

/* VIEW OPTIONS
 * 
 * unused ----------------------+
 * MODE_TIME -----------------+ |
 * MODE_TEST ---------------+ | |
 * unused ----------------+ | | |
 *			  | | | |
 *			  | | | |
 *			  | | | |
 *			  v v v v
 * 		0 0 0 0   0 0 0 0
 */
#define MODE_TIME	0X02
#define MODE_TEST	0x04

// methods in view.c used by other c files
// set packed bits for what you want
void set_view_properties( int );
int get_view_properties( void );

void show( char * );
void do_time();
void do_test();

// in model.c
void local_time();
