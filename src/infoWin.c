#include "infoWin.h"

/**
 * Creates the info window, the one on the right with all the planes, their
 * destinations, and "queued" commands.
 */
WINDOW *createInfoWin(AtcsoData *data) {
    WINDOW *infoWin = newwin(24, 20, 0, 60);

    updateInfoWin(data, infoWin);

    return infoWin;
}

/**
 * Update and refresh the message window.
 */
bool updateInfoWin(AtcsoData *data, WINDOW *infoWin) {
    waddstr(infoWin, "... ");

    wrefresh(infoWin);

    return false;
}
