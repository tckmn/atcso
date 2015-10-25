#include "input.h"
#include "command.h"

TreeNode curNode;
char curPlane, extra;

void initializeInput() {
    initializeCommands();
    curPlane = 0;
}

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
    } else {
        extra = '\0';
        if (ch >= '0' && ch <= '9') {
            extra = ch;
            ch = '#';
        }

        for (int i = 0; i < curNode.nChildren; ++i) {
            if (curNode.children[i].hotkey == ch) {
                curNode = curNode.children[i];
                waddch(msgWin, ' ');
                if (extra) {
                    wprintw(msgWin, curNode.str, extra);
                } else {
                    waddstr(msgWin, curNode.str);
                }
                wrefresh(msgWin);
                break;
            }
        }
    }
}
