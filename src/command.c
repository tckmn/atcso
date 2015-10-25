#include <stdlib.h>
#include <stdarg.h>
#include "command.h"

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

TreeNode commands;

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
            p->targetAltitude = max(0, p->altitude + (extra - '0'));
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
    // TODO
}

TreeNode *mkc(int count, ...);

void initializeCommands() {
    commands = (TreeNode) {0, "", NULL, mkc(2,
        (TreeNode) {'a', "altitude", NULL, mkc(3,
            (TreeNode) {'c', "climb", NULL, mkc(1,
                (TreeNode) {'#', "%c000 feet", altitudeClimb, NULL, 0}
            ), 1},
            (TreeNode) {'d', "descend", NULL, mkc(1,
                (TreeNode) {'#', "%c000 feet", altitudeDescend, NULL, 0}
            ), 1},
            (TreeNode) {'#', "%c000 feet", altitudeSet, NULL, 0}
        ), 3},
        (TreeNode) {'t', "turn", NULL, mkc(8,
            (TreeNode) {'w', "0 degrees", turnTo, NULL, 0},
            (TreeNode) {'e', "45 degrees", turnTo, NULL, 0},
            (TreeNode) {'d', "90 degrees", turnTo, NULL, 0},
            (TreeNode) {'c', "135 degrees", turnTo, NULL, 0},
            (TreeNode) {'x', "180 degrees", turnTo, NULL, 0},
            (TreeNode) {'z', "225 degrees", turnTo, NULL, 0},
            (TreeNode) {'a', "270 degrees", turnTo, NULL, 0},
            (TreeNode) {'q', "315 degrees", turnTo, NULL, 0}
        ), 8}
    ), 2};
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
