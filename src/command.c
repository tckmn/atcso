#include <stdlib.h>
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

TreeNode *mk1tn(TreeNode n1);
TreeNode *mk2tn(TreeNode n1, TreeNode n2);
TreeNode *mk3tn(TreeNode n1, TreeNode n2, TreeNode n3);

void initializeCommands() {
    commands = (TreeNode) {0, "", NULL, mk2tn(
        (TreeNode) {'a', "altitude", NULL, mk3tn(
            (TreeNode) {'c', "climb", NULL, mk1tn(
                (TreeNode) {'#', "%c000 feet", altitudeClimb, NULL, 0}
            ), 1},
            (TreeNode) {'d', "descend", NULL, mk1tn(
                (TreeNode) {'#', "%c000 feet", altitudeDescend, NULL, 0}
            ), 1},
            (TreeNode) {'#', "%c000 feet", altitudeSet, NULL, 0}
        ), 3},
        (TreeNode) {'t', "turn", NULL, NULL, 0}
    ), 2};
}

TreeNode *mk1tn(TreeNode n1) {
    TreeNode *a = malloc(1 * sizeof(TreeNode));
    a[0] = n1;
    return a;
}

TreeNode *mk2tn(TreeNode n1, TreeNode n2) {
    TreeNode *a = malloc(2 * sizeof(TreeNode));
    a[0] = n1;
    a[1] = n2;
    return a;
}

TreeNode *mk3tn(TreeNode n1, TreeNode n2, TreeNode n3) {
    TreeNode *a = malloc(3 * sizeof(TreeNode));
    a[0] = n1;
    a[1] = n2;
    a[2] = n3;
    return a;
}
