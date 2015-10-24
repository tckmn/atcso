#include <stdlib.h>
#include <ncurses.h>
#include <time.h>


typedef struct {
} AtcsoData;


void mainloop();
WINDOW *createRadarWin();
void updateRadarWin(AtcsoData *data, WINDOW *radarWin);


/**
 * The main function, called when atcso is started (duh).
 */
int main(int argc, char **argv) {
    initscr();
    raw();                  // Disable line buffering
    noecho();               // Don't show things the user is typing
    nodelay(stdscr, TRUE);  // Non-blocking getch()

    mainloop();             // Start the game!

    endwin();
    return EXIT_SUCCESS;
}

/**
 * The main loop: runs infinitely until the game is ended.
 */
void mainloop() {
    // get all our windows
    refresh();
    WINDOW *radarWin = createRadarWin();

    // TODO put this somewhere... better
    const int TICK_DELAY = 2;

    // the main loop
    int ch;
    time_t lastTick = time(NULL);
    AtcsoData data;
    for (;;) {
        if (difftime(time(NULL), lastTick) > TICK_DELAY) {
            updateRadarWin(&data, radarWin);

            lastTick += TICK_DELAY;
        }

        if ((ch = getch()) != ERR) {
            switch (ch) {
            case 'q':
            case 'Q':
                goto cleanup;
            }
        }
    }

    cleanup:
    delwin(radarWin);
}

/**
 * Creates the radar window, the biggest one that has all the planes and stuff.
 */
WINDOW *createRadarWin() {
    WINDOW *radarWin = newwin(21, 60, 0, 0);

    for (int i = 0; i < 59; ++i) waddch(radarWin, '-');
    waddch(radarWin, ' ');
    for (int i = 0; i < 19; ++i) {
        waddstr(radarWin, "| ");
        for (int j = 0; j < 28; ++j) waddstr(radarWin, ". ");
        waddstr(radarWin, "| ");
    }
    for (int i = 0; i < 59; ++i) waddch(radarWin, '-');
    waddch(radarWin, ' ');

    wrefresh(radarWin);
    return radarWin;
}

/**
 * Update and refresh the radar window.
 */
void updateRadarWin(AtcsoData *data, WINDOW *radarWin) {
    // TODO do stuff
}
