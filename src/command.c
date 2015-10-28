#include <stdlib.h>
#include <stdarg.h>
#include <string.h>  // WHY OH WHY is memmove defined in here
#include "command.h"


#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))


/**
 * In order to avoid bringing in trig fuctions, we hardcode the tangents of
 * certain values here. Here's how it works: if we find the slope between the
 * two XYs, we then have all the information we need to get the *angle* between
 * them: it's simply atan(slope) with a few sign complications.
 *
 * But we don't need to bring in an entire math lib when we can just hardcode a
 * few *key values*---precaluclated slopes that have a known angle. Since we
 * only need to be accurate to the nearest 45 degrees, this is sufficient.
 */
Direction getDir(XY a, XY b) {
    if (a.x == b.x) return (a.y < b.y) ? DOWN : UP;
    double slope = (double)(b.y - a.y) / (b.x - a.x);
    if (a.x < b.x) {
        if (slope > 2.414214) return DOWN; // tan(67.5deg), between 90 and 45
        else if (slope > 0.414214) return DOWN_RIGHT; // tan(22.2deg)
        else if (slope > -0.414214) return RIGHT; // duh
        else if (slope > -2.414214) return UP_RIGHT;
        else return UP;
    } else {
        if (slope > 2.414214) return UP;
        else if (slope > 0.414214) return UP_LEFT;
        else if (slope > -0.414214) return LEFT;
        else if (slope > -2.414214) return DOWN_LEFT;
        else return DOWN;
    }
}


TreeNode commands, delayTree;
callback delayedCmd = NULL;
char delayedExtra = 0;


void altitudeClimb(AtcsoData *data, char plane, char extra) {
    for (Plane *p = data->planes; !isNull(p->xy); ++p) {
        if (p->name == plane) {
            p->targetAltitude = min(9, p->altitude + (extra - '0'));
            return;
        }
    }
    // TODO: error, unknown plane
}


void altitudeDescend(AtcsoData *data, char plane, char extra) {
    for (Plane *p = data->planes; !isNull(p->xy); ++p) {
        if (p->name == plane) {
            p->targetAltitude = max(0, p->altitude - (extra - '0'));
            return;
        }
    }
    // TODO: error, unknown plane
}


void altitudeSet(AtcsoData *data, char plane, char extra) {
    for (Plane *p = data->planes; !isNull(p->xy); ++p) {
        if (p->name == plane) {
            p->targetAltitude = extra - '0';
            return;
        }
    }
    // TODO: error, unknown plane
}


void turnTo(AtcsoData *data, char plane, char extra) {
    for (Plane *p = data->planes; !isNull(p->xy); ++p) {
        if (p->name == plane) {
            switch (extra) {
                case 'w': p->targetDir = 0; break;
                case 'e': p->targetDir = 1; break;
                case 'd': p->targetDir = 2; break;
                case 'c': p->targetDir = 3; break;
                case 'x': p->targetDir = 4; break;
                case 'z': p->targetDir = 5; break;
                case 'a': p->targetDir = 6; break;
                case 'q': p->targetDir = 7; break;
                case 'l': p->targetDir = negmod((int)p->dir - 1, 8); break;
                case 'r': p->targetDir = (p->dir + 1) % 8; break;
                case 'L': p->targetDir = negmod((int)p->dir - 2, 8); break;
                case 'R': p->targetDir = (p->dir + 2) % 8; break;
            }
            return;
        }
    }
    // TODO: error, unknown plane
}


void turnTowardsBeacon(AtcsoData *data, char plane, char extra) {
    for (Plane *p = data->planes; !isNull(p->xy); ++p) {
        if (p->name == plane) {
            int i = 0;
            for (XY *b = data->beacons; !isNull(*b); ++b, ++i) {
                if (i == (extra - '0')) {
                    p->targetDir = getDir(p->xy, *b);
                    break;
                }
            }
            return;
        }
    }
    // TODO: error, unknown plane
}


void turnTowardsExit(AtcsoData *data, char plane, char extra) {
    for (Plane *p = data->planes; !isNull(p->xy); ++p) {
        if (p->name == plane) {
            int i = 0;
            for (XY *e = data->exits; !isNull(*e); ++e, ++i) {
                if (i == (extra - '0')) {
                    p->targetDir = getDir(p->xy, *e);
                    break;
                }
            }
            return;
        }
    }
    // TODO: error, unknown plane
}


void turnTowardsAirport(AtcsoData *data, char plane, char extra) {
    for (Plane *p = data->planes; !isNull(p->xy); ++p) {
        if (p->name == plane) {
            int i = 0;
            for (Airport *a = data->airports; !isNull(a->xy); ++a, ++i) {
                if (i == (extra - '0')) {
                    p->targetDir = getDir(p->xy, a->xy);
                    break;
                }
            }
            return;
        }
    }
    // TODO: error, unknown plane
}


void circle(AtcsoData *data, char plane, char extra) {
    for (Plane *p = data->planes; !isNull(p->xy); ++p) {
        if (p->name == plane) {
            if (extra == 'l') {
                p->targetDir = CIRCLE_LEFT;
                if (p->dir % 2 != 0) ++p->dir;  // we can't rotate 3 times
            } else {
                // right is default
                p->targetDir = CIRCLE_RIGHT;
                if (p->dir % 2 != 0) --p->dir;
            }
        }
    }
    // TODO: error, unknown plane
}


void delayBeacon(AtcsoData *data, char plane, char extra) {
    BeaconQueueEvent bqe = (BeaconQueueEvent) {extra - '0', delayedCmd, plane,
            delayedExtra};
    data->bqes = realloc(data->bqes, (++data->nBqes) * sizeof(BeaconQueueEvent));
    data->bqes[data->nBqes - 1] = bqe;
}


/**
 * This is called exactly once every "tick" to handle queued events.
 */
bool updateCommands(AtcsoData *data) {
    // # beacons < # bqe's < # planes
    // therefore, the outer loop (the one run the fewest times) should loop
    //   over the largest array
    for (Plane *plane = data->planes; !isNull(plane->xy); ++plane) {
        // now we loop over beacons, because there's no point in looping over
        //   all the events if the plane's not even on a beacon
        int bIdx = 0;
        for (XY *beacon = data->beacons; !isNull(*beacon); ++beacon, ++bIdx) {
            if (eq(plane->xy, *beacon)) {
                // finally, check to see if there are any events matching this
                for (int i = 0; i < data->nBqes; ++i) {
                    if (data->bqes[i].plane == plane->name &&
                            data->bqes[i].which == bIdx) {
                        // yay! we have an event!
                        (*data->bqes[i].func)(data, data->bqes[i].plane,
                                data->bqes[i].extra);
                        // remove bqe
                        memmove(data->bqes + i, data->bqes + i + 1,
                                sizeof(BeaconQueueEvent) * (--data->nBqes - i));
                        data->bqes = realloc(data->bqes,
                                data->nBqes * sizeof(BeaconQueueEvent));
                        --i;
                    }
                }
                break;  // plane can't be on multiple beacons
            }
        }
    }
    return false;
}


TreeNode *mkc(int count, ...);
void setParents(TreeNode *tn);


/**
 * This functions is expected to be called exactly once at the very beginning
 * of the program. It initializes the (from here on, constant) data in the
 * `commands' and `delayTree' variables.
 */
void initializeCommands() {
    commands = (TreeNode) {0, "", NULL, mkc(3,
        (TreeNode) {'a', "altitude", NULL, mkc(3,
            (TreeNode) {'c', "climb", NULL, mkc(1,
                (TreeNode) {'#', "%c000 feet", altitudeClimb, NULL, 0, NULL}
            ), 1, NULL},
            (TreeNode) {'d', "descend", NULL, mkc(1,
                (TreeNode) {'#', "%c000 feet", altitudeDescend, NULL, 0, NULL}
            ), 1, NULL},
            (TreeNode) {'#', "%c000 feet", altitudeSet, NULL, 0, NULL}
        ), 3, NULL},
        (TreeNode) {'t', "turn", NULL, mkc(13,
            (TreeNode) {'w', "0 degrees", turnTo, NULL, 0, NULL},
            (TreeNode) {'e', "45 degrees", turnTo, NULL, 0, NULL},
            (TreeNode) {'d', "90 degrees", turnTo, NULL, 0, NULL},
            (TreeNode) {'c', "135 degrees", turnTo, NULL, 0, NULL},
            (TreeNode) {'x', "180 degrees", turnTo, NULL, 0, NULL},
            (TreeNode) {'z', "225 degrees", turnTo, NULL, 0, NULL},
            (TreeNode) {'a', "270 degrees", turnTo, NULL, 0, NULL},
            (TreeNode) {'q', "315 degrees", turnTo, NULL, 0, NULL},
            (TreeNode) {'l', "left", turnTo, NULL, 0, NULL},
            (TreeNode) {'r', "right", turnTo, NULL, 0, NULL},
            (TreeNode) {'L', "hard left", turnTo, NULL, 0, NULL},
            (TreeNode) {'R', "hard right", turnTo, NULL, 0, NULL},
            (TreeNode) {'t', "towards", NULL, mkc(3,
                (TreeNode) {'b', "beacon", NULL, mkc(1,
                    (TreeNode) {'#', "%c", turnTowardsBeacon, NULL, 0, NULL}
                ), 1, NULL},
                (TreeNode) {'e', "exit", NULL, mkc(1,
                    (TreeNode) {'#', "%c", turnTowardsExit, NULL, 0, NULL}
                ), 1, NULL},
                (TreeNode) {'a', "airport", NULL, mkc(1,
                    (TreeNode) {'#', "%c", turnTowardsAirport, NULL, 0, NULL}
                ), 1, NULL}
            ), 3, NULL}
        ), 13, NULL},
        (TreeNode) {'c', "circle", circle, mkc(2,
            (TreeNode) {'r', "right", circle, NULL, 0, NULL},
            (TreeNode) {'l', "left", circle, NULL, 0, NULL}
        ), 2, NULL}
    ), 3, NULL};

    delayTree = (TreeNode) {0, "at", NULL, mkc(1,
        (TreeNode) {'b', "beacon", NULL, mkc(1,
            (TreeNode) {'#', "%c", delayBeacon, NULL, 0, NULL}
        ), 1, NULL}
    ), 1, NULL};

    // traverse tree, set parents recursively
    setParents(&commands);
    setParents(&delayTree);
}


TreeNode *getDelayTree(TreeNode *parent) {
    delayTree.parent = parent;
    return &delayTree;
}


TreeNode *mkc(int count, ...) {
    TreeNode *a = malloc(count * sizeof(TreeNode));
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; ++i) {
        a[i] = va_arg(args, TreeNode);
    }

    va_end(args);
    return a;
}


void setParents(TreeNode *tn) {
    for (int i = 0; i < tn->nChildren; ++i) {
        tn->children[i].parent = tn;
        setParents(tn->children + i);
    }
}
