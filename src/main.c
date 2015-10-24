#include <stdlib.h>
#include <ncurses.h>
#include <time.h>


// utility types
typedef struct { int x; int y; } XY;
int isNull(XY xy) { return xy.x < 0 || xy.y < 0; }
typedef enum { UP = 1, RIGHT = 2, DOWN = 4, LEFT = 8 } Direction;


// types for storing global data
typedef struct {
    XY xy;
    Direction dir;
} Airport;

typedef struct {
    XY xy;
    Direction dir;
    char name;
    int altitude;
    // TODO fuel, target, speed, etc.
} Plane;

typedef struct {
    // these are arrays terminated by isNull(xy)
    XY *exits;
    XY *beacons;
    Airport *airports;
    Plane *planes;

    int tickDelay;
    double newPlaneRate;
    char nextLetter;
} AtcsoData;


void mainloop();
WINDOW *createRadarWin(AtcsoData *data);
bool updateRadarWin(AtcsoData *data, WINDOW *radarWin);


/**
 * The main function, called when atcso is started (duh).
 */
int main(int argc, char **argv) {

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-h") ^ strcmp(argv[i], "--help")) {
            printf("Usage: ./atcso [options]\n"
                   "\n"
                   "Options:\n"
                   "    -h, --help: Display this help\n");
            return EXIT_SUCCESS;
        }
    }
    initscr();
    raw();                  // Disable line buffering
    noecho();               // Don't show things the user is typing
    nodelay(stdscr, TRUE);  // Non-blocking getch()
    srand(time(NULL));      // Seed RNG

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

    data.beacons = malloc(3 * sizeof(XY));
    data.beacons[0] = (XY) {12, 7};
    data.beacons[1] = (XY) {12, 17};
    data.beacons[2] = (XY) {-1, -1};

    data.airports = malloc(3 * sizeof(Airport));
    data.airports[0] = (Airport) {{20, 15}, UP};
    data.airports[1] = (Airport) {{20, 18}, RIGHT};
    data.airports[2] = (Airport) {{-1, -1}, 0};

    data.planes = malloc(1 * sizeof(Plane));
    data.planes[0] = (Plane) {{-1, -1}, 0, 0, 0};

    data.tickDelay = 1;
    data.newPlaneRate = 1;
    data.nextLetter = 'A';

    // get all our windows
    refresh();
    WINDOW *radarWin = createRadarWin(&data);

    // vars used in the main loop
    int ch;
    time_t lastTick = time(NULL);

    // the main loop
    for (;;) {
        if (difftime(time(NULL), lastTick) > data.tickDelay) {
            if (updateRadarWin(&data, radarWin)) {
                // if any of the update*() functions returns true, game is over
                goto cleanup;
            }

            lastTick += data.tickDelay;
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

    // add the beacons
    xy = data->beacons;
    for (int i = 0; !isNull(*xy); ++xy, ++i) {
        mvwaddch(radarWin, xy->y, 2 * xy->x, '*');
        waddch(radarWin, '0' + i);
    }

    // add the airports
    Airport *airport = data->airports;
    for (int i = 0; !isNull(airport->xy); ++airport, ++i) {
        mvwaddch(radarWin, airport->xy.y, 2 * airport->xy.x,
                "_^>_v___<"[airport->dir]);
        waddch(radarWin, '0' + i);
    }

    updateRadarWin(data, radarWin);

    return radarWin;
}

/**
 * Update and refresh the radar window.
 */
bool updateRadarWin(AtcsoData *data, WINDOW *radarWin) {
    int nPlanes = 0;
    for (Plane *p = data->planes; !isNull(p->xy); ++p, ++nPlanes) {
        mvwaddstr(radarWin, p->xy.y, 2 * p->xy.x, ". ");
        // TODO check for beacons, airports; redraw and do actions

        if (p->dir & UP) --p->xy.y;
        if (p->dir & RIGHT) ++p->xy.x;
        if (p->dir & DOWN) ++p->xy.y;
        if (p->dir & LEFT) --p->xy.x;

        if (p->xy.y == 0 || p->xy.y == 20 || p->xy.x == 0 || p->xy.x == 29) {
            // the plane either exited or crashed
            if (p->altitude != 9) return true;  // game over
            for (XY *exit = data->exits; !isNull(*exit); ++exit) {
                if (exit->y == p->xy.y && exit->x == p->xy.x) {
                    // woohoo, plane exited!
                    // TODO make sure it went in the right exit
                    // TODO remove plane
                }
            }
            return true;  // plane didn't hit an exit; game over
        }

        mvwaddch(radarWin, p->xy.y, 2 * p->xy.x, p->name);
        waddch(radarWin, '7');
    }

    if (rand() < RAND_MAX * data->newPlaneRate) {
        int nExits = 0;
        for (XY *exit = data->exits; !isNull(*exit); ++exit, ++nExits);

        XY entryCoords = data->exits[rand() % nExits];
        Direction entryDir = 0;
        if (entryCoords.y == 0)  { entryCoords.y = 1;  entryDir |= DOWN;  }
        if (entryCoords.y == 20) { entryCoords.y = 19; entryDir |= UP;    }
        if (entryCoords.x == 0)  { entryCoords.x = 1;  entryDir |= RIGHT; }
        if (entryCoords.x == 29) { entryCoords.x = 28; entryDir |= LEFT;  }

        data->planes = realloc(data->planes, (nPlanes + 2) * sizeof(Plane));
        data->planes[nPlanes] = (Plane) {entryCoords, entryDir,
                data->nextLetter, 7};
        data->planes[nPlanes + 1] = (Plane) {{-1, -1}, 0, 0, 0};

        ++data->nextLetter;
        if (data->nextLetter > 'Z') data->nextLetter = 'A';

        mvwaddch(radarWin, data->planes[nPlanes].xy.y,
                2 * data->planes[nPlanes].xy.x,
                data->planes[nPlanes].name);
        waddch(radarWin, '0' + data->planes[nPlanes].altitude);
    }

    wrefresh(radarWin);

    return false;
}
