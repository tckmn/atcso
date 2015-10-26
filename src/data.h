#ifndef __DATA_H__
#define __DATA_H__


#include <ncurses.h>


// utility types
typedef struct { int x; int y; } XY;
#define isNull(xy) ((xy).x < 0 || (xy).y < 0)
#define eq(xy1, xy2) ((xy1).x == (xy2).x && (xy1).y == (xy2).y)
typedef enum { UP, UP_RIGHT, RIGHT, DOWN_RIGHT,
    DOWN, DOWN_LEFT, LEFT, UP_LEFT } Direction;
#define dx(dir) ((dir == UP || dir == DOWN) ? 0 : \
        (dir == UP_LEFT || dir == LEFT || dir == DOWN_LEFT) ? -1 : 1)
#define dy(dir) ((dir == RIGHT || dir == LEFT) ? 0 : \
        (dir == UP_LEFT || dir == UP || dir == UP_RIGHT) ? -1 : 1)
#define fromdyx(dy, dx) ( \
            (dy == -1) ? \
                ((dx == -1) ? UP_LEFT : (dx == 1) ? UP_RIGHT : UP) \
            : (dy == 1) ? \
                ((dx == -1) ? DOWN_LEFT : (dx == 1) ? DOWN_RIGHT : DOWN) \
            : ((dx == -1) ? LEFT : RIGHT) \
        )

struct atcsoData_t;

// types for storing global data
typedef struct {
    XY xy;
    Direction dir;
} Airport;

typedef struct {
    XY xy;
    char name;
    int altitude;
    int targetAltitude;
    Direction dir;
    Direction targetDir;
    char destType;
    int dest;
    // TODO fuel, speed, etc.
} Plane;

typedef void (*callback)(struct atcsoData_t*, char, char);

typedef struct {
    int which;
    callback func;
    char plane;
    char extra;
} BeaconQueueEvent;

typedef struct atcsoData_t {
    // these are arrays terminated by isNull(xy)
    XY *exits;
    XY *beacons;
    Airport *airports;
    Plane *planes;
    int *planesSorted;

    int tickDelay;
    double newPlaneRate;
    double newPlaneCounter;
    char nextLetter;

    BeaconQueueEvent *bqes;
    int nBqes;

    int time;
    int score;
} AtcsoData;

#endif
