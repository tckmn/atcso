#include <stdlib.h>
#include <ncurses.h>


/**
 * The main function, called when atcso is started (duh).
 */
int main(int argc, char** argv) {
    initscr();
    raw();      // Disable line buffering
    noecho();   // Don't show things the user is typing

    printw("hai wurld");
    refresh();

    while (getch() != 'q');  // Wait for the user to hit `q' to quit
    endwin();

    return EXIT_SUCCESS;
}
