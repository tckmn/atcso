#include <stdlib.h>
#include <ncurses.h>
#include <time.h>


typedef struct { int x; int y; } XY;
int isNull(XY xy) { return xy.x < 0 || xy.y < 0; }


typedef struct {
    XY *exits;
} AtcsoData;


void mainloop();
WINDOW *createRadarWin(AtcsoData *data);
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
    // initalize all the global data
    AtcsoData data;
    data.exits = malloc(3 * sizeof(XY));
    data.exits[0] = (XY) {10, 0};
    data.exits[1] = (XY) {0, 10};
    data.exits[2] = (XY) {-1, -1};

    // get all our windows
    refresh();
    WINDOW *radarWin = createRadarWin(&data);

    // TODO put this somewhere... better
    const int TICK_DELAY = 2;

    // vars used in the main loop
    int ch;
    time_t lastTick = time(NULL);

    // the main loop
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
    free(data.exits);
    delwin(radarWin);
}

/**
 * Creates the radar window, the biggest one that has all the planes and stuff.
 */
WINDOW *createRadarWin(AtcsoData *data) {
    WINDOW *radarWin = newwin(21, 60, 0, 0);

    // draw the outline
    for (int i = 0; i < 59; ++i) waddch(radarWin, '-');
    waddch(radarWin, ' ');
    for (int i = 0; i < 19; ++i) {
        waddstr(radarWin, "| ");
        for (int j = 0; j < 28; ++j) waddstr(radarWin, ". ");
        waddstr(radarWin, "| ");
    }
    for (int i = 0; i < 59; ++i) waddch(radarWin, '-');
    waddch(radarWin, ' ');

    // add the exits
    XY *p = data->exits;
    for (int i = 0; !isNull(*p); ++p, ++i) {
        mvwaddch(radarWin, p->y, 2 * p->x, '0' + i);
    }

    wrefresh(radarWin);
    return radarWin;
}

/**
 * Update and refresh the radar window.
 */
void updateRadarWin(AtcsoData *data, WINDOW *radarWin) {
    // TODO do stuff
}
