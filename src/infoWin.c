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
    werase(infoWin);

    wprintw(infoWin, "Time: %i  Safe: %i\n\npl dt comm\n", data->time,
            data->score);

    for (Plane *plane = data->planes; !isNull(plane->xy); ++plane) {
        wprintw(infoWin, "%c%c %c%c stuff\n", plane->name, '0' + plane->altitude,
                plane->destType, '0' + plane->dest);
    }

    wrefresh(infoWin);

    return false;
}
