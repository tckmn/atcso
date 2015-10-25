#include "input.h"
#include "command.h"

TreeNode curNode;
char curPlane = 0, extra;

void handleInput(char ch, AtcsoData *data, WINDOW *msgWin) {
    if (!curPlane) {
        if (ch >= 'a' && ch <= 'z') {
            curPlane = ch;
            curNode = commands;
            waddch(msgWin, ch);
            waddch(msgWin, ':');
            wrefresh(msgWin);
        }
    } else if (ch == '\n') {
        if (curNode.func) {
            (*curNode.func)(data, curPlane, extra);
            curPlane = 0;
            werase(msgWin);
            wrefresh(msgWin);
        }
    } else if (ch == '\x7f') {
        if (curNode.parent) {
            int y, x, len = 0; getyx(msgWin, y, x);
            for (char *ch = curNode.str; *ch != '\0'; ++ch, ++len);
            wmove(msgWin, y, x - len - (curNode.hotkey == '#' ? 0 : 1));
            wclrtoeol(msgWin);
            wrefresh(msgWin);
            curNode = *curNode.parent;
        } else {
            // we only have a plane name. remove everything
            wmove(msgWin, 0, 0);
            wclrtoeol(msgWin);
            wrefresh(msgWin);
            curPlane = 0;
        }
    } else {
        extra = ch;
        if (ch >= '0' && ch <= '9') ch = '#';

        for (int i = 0; i < curNode.nChildren; ++i) {
            if (curNode.children[i].hotkey == ch) {
                curNode = curNode.children[i];
                outputNode:
                waddch(msgWin, ' ');
                if (ch == '#') {
                    wprintw(msgWin, curNode.str, extra);
                } else {
                    waddstr(msgWin, curNode.str);
                }
                wrefresh(msgWin);
                break;
            }
        }

        if (ch == 'a' && curNode.func) {
            delayedCmd = curNode.func;
            curNode = getDelayTree(&curNode);
            goto outputNode; // spaghetti code! \o/
        }
    }
}
