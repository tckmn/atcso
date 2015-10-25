#include <stdlib.h>
#include <time.h>


#include "radarWin.h"
#include "msgWin.h"
#include "infoWin.h"
#include "input.h"
#include "command.h"


void mainloop();


/**
 * The main function, called when atcso is started (duh).
 */
int main(int argc, char **argv) {
    initscr();
    raw();                  // Disable line buffering
    noecho();               // Don't show things the user is typing
    nodelay(stdscr, TRUE);  // Non-blocking getch()
    srand(time(NULL));      // Seed RNG
    initializeCommands();   // duh

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
    data.planes[0] = (Plane) {{-1, -1}, 0, 0, 0, 0, 0, 0, 0};

    data.tickDelay = 1;
    data.newPlaneRate = 5;
    data.newPlaneCounter = data.newPlaneRate;
    data.nextLetter = 'a';

    data.bqes = NULL;
    data.nBqes = 0;

    data.time = 0;
    data.score = 0;

    // get all our windows
    refresh();
    WINDOW *radarWin = createRadarWin(&data);
    WINDOW *msgWin = createMsgWin(&data);
    WINDOW *infoWin = createInfoWin(&data);

    // vars used in the main loop
    int ch;
    time_t lastTick = time(NULL);

    // the main loop
    for (;;) {
        if (difftime(time(NULL), lastTick) > data.tickDelay) {
            if (updateRadarWin(&data, radarWin) ||
                    updateInfoWin(&data, infoWin) ||
                    updateMsgWin(&data, msgWin) ||
                    updateCommands(&data)) {
                // if any of the update*() functions returns true, game is over
                goto cleanup;
            }

            lastTick += data.tickDelay;
            ++data.time;
        }

        if ((ch = getch()) != ERR) {
            if (ch == 3) goto cleanup;  // Ctrl+C
            else {
                // let input.c handle it
                handleInput(ch, &data, msgWin);
            }
        }
    }

    cleanup:
    free(data.exits);
    free(data.beacons);
    free(data.airports);
    free(data.planes);

    werase(msgWin);
    waddstr(msgWin, "Game over... (press space to exit)");
    wrefresh(msgWin);
    while ((ch = getch()) != ' ');

    delwin(radarWin);
    delwin(msgWin);
    delwin(infoWin);
}
