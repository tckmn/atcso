#include <stdlib.h>
#include <stdarg.h>
#include <string.h>  // WHY OH WHY is memmove defined in here
#include "command.h"

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

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
            }
            return;
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

void initializeCommands() {
    commands = (TreeNode) {0, "", NULL, mkc(2,
        (TreeNode) {'a', "altitude", NULL, mkc(3,
            (TreeNode) {'c', "climb", NULL, mkc(1,
                (TreeNode) {'#', "%c000 feet", altitudeClimb, NULL, 0, NULL}
            ), 1, NULL},
            (TreeNode) {'d', "descend", NULL, mkc(1,
                (TreeNode) {'#', "%c000 feet", altitudeDescend, NULL, 0, NULL}
            ), 1, NULL},
            (TreeNode) {'#', "%c000 feet", altitudeSet, NULL, 0, NULL}
        ), 3, NULL},
        (TreeNode) {'t', "turn", NULL, mkc(8,
            (TreeNode) {'w', "0 degrees", turnTo, NULL, 0, NULL},
            (TreeNode) {'e', "45 degrees", turnTo, NULL, 0, NULL},
            (TreeNode) {'d', "90 degrees", turnTo, NULL, 0, NULL},
            (TreeNode) {'c', "135 degrees", turnTo, NULL, 0, NULL},
            (TreeNode) {'x', "180 degrees", turnTo, NULL, 0, NULL},
            (TreeNode) {'z', "225 degrees", turnTo, NULL, 0, NULL},
            (TreeNode) {'a', "270 degrees", turnTo, NULL, 0, NULL},
            (TreeNode) {'q', "315 degrees", turnTo, NULL, 0, NULL}
        ), 8, NULL}
    ), 2, NULL};

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
