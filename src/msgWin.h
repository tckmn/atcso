#ifndef __MSGWIN_H__
#define __MSGWIN_H__

#include "data.h"

WINDOW *createMsgWin(AtcsoData *data);
bool updateMsgWin(AtcsoData *data, WINDOW *msgWin);

#endif
