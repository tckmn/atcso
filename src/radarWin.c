#include <stdlib.h>
#include <string.h>  // WHY OH WHY is memmove defined in here

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
                "^_>_v_<_"[airport->dir]);
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
    }

    int pIdx = 0;
    for (Plane *p = data->planes; !isNull(p->xy); ++p, ++pIdx) {
        // TODO check for beacons, airports; redraw and do actions

        if (p->targetAltitude > p->altitude) ++p->altitude;
        if (p->targetAltitude < p->altitude) --p->altitude;
        // TODO check for landing at airport, crashing

        if (p->dir != p->targetDir) {
            if ((p->dir < p->targetDir ? p->dir+8 : p->dir) > p->targetDir+4) {
                // we're turning clockwise
                for (int i = 0; (i < 2) && (p->dir != p->targetDir); ++i) {
                    p->dir = (p->dir + 1) % 8;
                }
            } else {
                // going counterclockwise
                for (int i = 0; (i < 2) && (p->dir != p->targetDir); ++i) {
                    --p->dir;
                    if (p->dir < 0) p->dir += 8;
                }
            }
        }

        p->xy.y += dy(p->dir);
        p->xy.x += dx(p->dir);

        if (p->xy.y == 0 || p->xy.y == 20 || p->xy.x == 0 || p->xy.x == 29) {
            // the plane either exited or crashed
            if (p->altitude != 9) return true;  // game over
            int exitNum = 0;
            for (XY *exit = data->exits; !isNull(*exit); ++exit, ++exitNum) {
                if (exit->y == p->xy.y && exit->x == p->xy.x) {
                    // woohoo, plane exited!
                    if (p->destType == 'E' && p->dest == exitNum) {
                        // remove plane
                        memmove(data->planes + pIdx, data->planes + pIdx + 1,
                                sizeof(Plane) * (nPlanes - pIdx));
                        data->planes = realloc(data->planes,
                                nPlanes * sizeof(Plane));
                        p = data->planes + pIdx;
                        goto exited;
                    } else {
                        return true;  // wrong exit, game over
                    }
                }
            }
            return true;  // plane didn't hit an exit; game over
        }

        mvwaddch(radarWin, p->xy.y, 2 * p->xy.x, p->name);
        waddch(radarWin, '0' + p->altitude);

        exited: {}
    }

    if (rand() < RAND_MAX * data->newPlaneRate) {
        int nExits = 0;
        for (XY *exit = data->exits; !isNull(*exit); ++exit, ++nExits);
        int nAirports = 0;
        for (Airport *ap = data->airports; !isNull(ap->xy); ++ap, ++nAirports);

        XY entryCoords = data->exits[rand() % nExits];
        int dy = 0, dx = 0;
        if (entryCoords.y == 0)  { entryCoords.y = 1;  dy = 1;  }
        if (entryCoords.y == 20) { entryCoords.y = 19; dy = -1; }
        if (entryCoords.x == 0)  { entryCoords.x = 1;  dx = 1;  }
        if (entryCoords.x == 29) { entryCoords.x = 28; dx = -1; }
        Direction entryDir = fromdyx(dy, dx);
        int destIdx = rand() % (nExits + nAirports);

        data->planes = realloc(data->planes, (nPlanes + 2) * sizeof(Plane));
        data->planes[nPlanes] = (Plane) {entryCoords, data->nextLetter, 7, 7,
                entryDir, entryDir, destIdx > nExits ? 'A' : 'E',
                destIdx > nExits ? destIdx - nExits : destIdx};
        data->planes[nPlanes + 1] = (Plane) {{-1, -1}, 0, 0, 0, 0, 0, 0, 0};

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
