#ifndef __INFOWIN_H__
#define __INFOWIN_H__

#include "data.h"

WINDOW *createInfoWin(AtcsoData *data);
bool updateInfoWin(AtcsoData *data, WINDOW *infoWin);

#endif
