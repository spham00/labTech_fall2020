/* wordscroll.c - scroll a word (more than one if you use quotes)
 *
 * assumes display size of 9, max size of 100
 *
 * D Provine and Posse, 9 April 2015
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSIZE       100

int main(int argc, char *argv[])
{
    char message[BUFSIZE] = "";
    char visible[9 + 1]; // +1 for null
    int  step;
    int  themessage;
    int  newline = 0;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s [-n] string\n", argv[0]);
        exit(1);
    }

    if (argc == 3) {
        newline = 1;
    }

    themessage = argc - 1;

    // we need 9 blanks before and after the message, plus a null
    if (strlen(argv[themessage]) > (BUFSIZE - 19) ) {
        fprintf(stderr, "%s: message too long\n", argv[0]);
        exit(1);
    }

    // pad message with blanks
    sprintf(message, "         %s         ", argv[themessage]);

    step = 0;
    while ( 1 ) {
        sprintf(visible, "%.*s", 9, &(message[step]) );
        printf(" |%s|%c", visible, newline ? '\n' : '\r');
        fflush(stdout);
        usleep(100000);
        if ( step < strlen(message) - 9 ) {
            step++;
        } else {
            step = 0;
        }
    }

    return 0;
}

