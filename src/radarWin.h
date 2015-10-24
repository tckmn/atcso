#ifndef __RADAR_WIN_H__
#define __RADAR_WIN_H__

#include "data.h"

WINDOW *createRadarWin(AtcsoData *data);
bool updateRadarWin(AtcsoData *data, WINDOW *radarWin);

#endif
