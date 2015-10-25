#ifndef __DATA_H__
#define __DATA_H__


#include <ncurses.h>


// utility types
typedef struct { int x; int y; } XY;
#define isNull(xy) ((xy).x < 0 || (xy).y < 0)
typedef enum { UP = 1, RIGHT = 2, DOWN = 4, LEFT = 8 } Direction;

// types for storing global data
typedef struct {
    XY xy;
    Direction dir;
} Airport;

typedef struct {
    XY xy;
    Direction dir;
    char name;
    int altitude;
    int targetAltitude;
    // TODO fuel, target, speed, etc.
} Plane;

typedef struct {
    // these are arrays terminated by isNull(xy)
    XY *exits;
    XY *beacons;
    Airport *airports;
    Plane *planes;

    int tickDelay;
    double newPlaneRate;
    char nextLetter;
} AtcsoData;

#endif
