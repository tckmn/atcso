#include <stdlib.h>
#include "command.h"

TreeNode commands;

void altitudeClimb(AtcsoData *data, char plane, char extra) {
    // TODO
}

void altitudeDescend(AtcsoData *data, char plane, char extra) {
    // TODO
}

void altitudeSet(AtcsoData *data, char plane, char extra) {
    // TODO
}

void initializeCommands() {
    commands = (TreeNode) {0, "", NULL, mk2tn(
        (TreeNode) {'a', "altitude", NULL, mk3tn(
            (TreeNode) {'c', "climb", NULL, mk1tn(
                (TreeNode) {'#', "#000 feet", altitudeClimb, NULL, 0}
            ), 0},
            (TreeNode) {'d', "descend", NULL, mk1tn(
                (TreeNode) {'#', "#000 feet", altitudeDescend, NULL, 0}
            ), 0},
            (TreeNode) {'#', "#000 feet", altitudeSet, NULL, 0}
        ), 0},
        (TreeNode) {'t', "turn", NULL, NULL, 0}
    ), 2};
}
