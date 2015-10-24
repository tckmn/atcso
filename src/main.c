#include <stdlib.h>
#include <ncurses.h>
#include <time.h>


// utility types
typedef struct { int x; int y; } XY;
int isNull(XY xy) { return xy.x < 0 || xy.y < 0; }
typedef enum { UP, RIGHT, DOWN, LEFT } Direction;


// types for storing global data
typedef struct {
    XY xy;
    Direction dir;
} Airport;

typedef struct {
    // these are arrays terminated by isNull(xy)
    XY *exits;
    Airport *airports;
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

    data.exits = malloc(9 * sizeof(XY));
    data.exits[0] = (XY) {12, 0};
    data.exits[1] = (XY) {29, 0};
    data.exits[2] = (XY) {29, 7};
    data.exits[3] = (XY) {29, 17};
    data.exits[4] = (XY) {9, 20};
    data.exits[5] = (XY) {0, 13};
    data.exits[6] = (XY) {0, 7};
    data.exits[7] = (XY) {0, 0};
    data.exits[8] = (XY) {-1, -1};

    data.airports = malloc(3 * sizeof(Airport));
    data.airports[0] = (Airport) {{20, 15}, UP};
    data.airports[1] = (Airport) {{20, 18}, RIGHT};
    data.airports[2] = (Airport) {{-1, -1}, 0};

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
    XY *xy = data->exits;
    for (int i = 0; !isNull(*xy); ++xy, ++i) {
        mvwaddch(radarWin, xy->y, 2 * xy->x, '0' + i);
    }

    // add the airports
    Airport *airport = data->airports;
    for (int i = 0; !isNull(airport->xy); ++airport, ++i) {
        mvwaddch(radarWin, airport->xy.y, 2 * airport->xy.x, "^>v<"[airport->dir]);
        waddch(radarWin, '0' + i);
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
