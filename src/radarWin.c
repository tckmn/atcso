#include <stdlib.h>

#include "radarWin.h"

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

        if (p->targetAltitude > p->altitude) ++p->altitude;
        if (p->targetAltitude < p->altitude) --p->altitude;
        // TODO check for landing at airport, crashing

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
        waddch(radarWin, '0' + p->altitude);
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
                data->nextLetter, 7, 7};
        data->planes[nPlanes + 1] = (Plane) {{-1, -1}, 0, 0, 0, 0};

        ++data->nextLetter;
        if (data->nextLetter > 'z') data->nextLetter = 'a';

        mvwaddch(radarWin, data->planes[nPlanes].xy.y,
                2 * data->planes[nPlanes].xy.x,
                data->planes[nPlanes].name);
        waddch(radarWin, '0' + data->planes[nPlanes].altitude);
    }

    wrefresh(radarWin);

    return false;
}
