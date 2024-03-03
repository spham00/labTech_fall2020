SCROLL BOX

As with the clock, we have a file 'LED-layout.txt', which explains which
bits go where to turn on LEDs.  The manufacturer of the display has
also sent us a file 'LEDots.h', which has prototypes and declarations
needed for our compiler, and 'LEDots.o', which has all the object code.
(They did not send us the C source files, because they consider that to
be a trade secret.)

What we have to do is modify the View portion of our scroll box to use
this display.  The Controller handles the flags about which display to
use, but it doesn't handle keystrokes.

As with the Clock, the model has basic functionality, but it will
not show time.

WHAT YOU NEED TO DO:

1) Look at the logic of 'track start position and find next N characters'
   that we used on the text-only scrollbox, and see how display_string()
   model.c similarly pulls out the correct 9 characters from the message
   to be shown.

   Note that stringscroll.c uses a loop; the display_string() function
   does NOT have a loop.  Each time it's called, it advances one position
   in the string and return the characters it gets, and that's all.

   The loop is provided by 'while ( 1 )' in main() and the timer, which
   combine to call display_string() repeatedly.  display_string() has
   the body of the loop, but doesn't have a loop of its own.


2) Modify show() in view.c so that it shows all 9 characters of the
   text it is passed.


3) Add code to handle keystrokes and mouse clicks.  Note that you
   can make the box scroll faster/slower by just lowering/raising
   the value of the 'delay' variable and then re-calling the
   'start_timer()' function.  But put limits: don't let delay drop
   to less than 100, or rise to more than 5000.

   You can make up whatever you want for 'time' and 'test', as long
   as one shows the time for 5 seconds and the other tests all the
   LEDs in one way or another.


4) Get extra credit if you want.

A) Adding some things for the second row of buttons to do.

   You can use the second-row keys for whatever.  In the past, 
   students have created 'DATE' buttons, which show the date.
   Others have done inverse video, vertical scrolling for
   short messages, mirror-mode where the letters are flipped
   and scroll the other direction so it looks right in a mirror.

   If you have an idea but aren't sure how to do it, or whether
   it'll be too hard, talk to me and I'll give suggestions.


B) Add your own option flags:

   Maybe '-c' for 'just be a clock', and then set up the bottom row
   of buttons with AM/PM and 24hr buttons, and even re-use the '-o'
   offset code you wrote for the clock project.

   One student made a clock that counted for 10 seconds, and then
   showed the date for five seconds, and then went back to the time.
   (The date was shown with an abbreviation for the month, as 'Dec 12'.)

   One pair made the time infinitely scroll across the the display
   ('... 10:22:14 ... 10:22:15 ... 10:22:16 ...'), but they couldn't
   get it synced so that the seconds never changed while scrolling.


C) Other stuff

   Note that the title bar is 80 characters; so you can put in not only
   your name, but you could put the date in the upper-right.  You could
   use the sysinfo(2) system call to put the total number of running
   processes, or the system load, or some other such information in the
   title bar.

   You could even put the date/time in the title bar continually, updating
   that every second and/or minute while the message scrolls by.  (You
   could add an option flag for whether you want seconds or not.)

   A combination would be an option flag so that it shows the time
   every minute or so after it gets to the end of the scrolling
   message.


NOTE ON IMPLEMENTATION:

As with the clock, one way to make these features work is to create a
variable in 'model.c' and/or 'view.c' and make up some constants, and
use get/set functions the way the view does in the clock.

#define MODE_NORMAL  0x01
#define MODE_CLOCK   0x02
#define MODE_DATE    0x04
#define MODE_TEST    0x08

Then your Controller module, would have a switch statement where
you check which mode you're in and act accordingly.

Unlike the clock, these modes are not already in place: you will
have to create them.

Notes:

 1) For modes which are supposed to be temporary, you can add
    a counter for those and decrement it each time.


 2) Anything about the text of the message is in the model.
    Show the time, or switching to a different message, or anything
    about the text to be displayed, is something the model should
    do.  'What it says' is in the model.


 3) Anything about the way it looks is in the view.  Going to inverse
    video, or putting the characters in backwards and reversing
    direction, is something the view handles.  'How it looks' is in
    the view.

    The 'test' mode, in which we test all the LEDs, is also about
    the view.


 4) Some things require coordination: if you want to do smooth
    scrolling, for example, you'll to update the screen more often.
    That will affect the limits of your fast/slow button and how big
    the steps should be.


If you need to change show() to take extra parameters, feel free to do
so.  Just be sure to change the prototype in scroll.h, or the compiler
will fuss at you.

