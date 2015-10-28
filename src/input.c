#include "input.h"
#include "command.h"


TreeNode *curNode;
char curPlane = 0, extra;


/**
 * Called for every keystroke, except ctrl+c.
 */
void handleInput(char ch, AtcsoData *data, WINDOW *msgWin) {
    if (!curPlane) {
        // we're not in the middle of a command yet
        if (ch >= 'a' && ch <= 'z') {
            curPlane = ch;
            curNode = &commands;
            waddch(msgWin, ch);
            waddch(msgWin, ':');
            wrefresh(msgWin);
        }
    } else if (ch == '\n') {
        // user tried to submit (possibly unfinished) command
        if (curNode->func) {
            (*curNode->func)(data, curPlane, extra);
            curPlane = 0;
            werase(msgWin);
            wrefresh(msgWin);
        }
    } else if (ch == '\x7f') {
        // user pressed the backspace key with something currently in the input
        // area (we've already checked for the "input is empty" state)
        if (curNode->parent) {
            int y, x, len = 0; getyx(msgWin, y, x);
            for (char *ch = curNode->str; *ch != '\0'; ++ch, ++len);
            wmove(msgWin, y, x - len - (curNode->hotkey == '#' ? 0 : 1));
            wclrtoeol(msgWin);
            wrefresh(msgWin);
            curNode = curNode->parent;
        } else {
            // we only have a plane name. remove everything
            wmove(msgWin, 0, 0);
            wclrtoeol(msgWin);
            wrefresh(msgWin);
            curPlane = 0;
        }
    } else {
        if (ch >= '0' && ch <= '9') {
            extra = ch;
            ch = '#';
        }

        for (int i = 0; i < curNode->nChildren; ++i) {
            if (curNode->children[i].hotkey == ch) {
                if (ch != '#') extra = ch;
                curNode = curNode->children + i;
                outputNode:
                waddch(msgWin, ' ');
                if (ch == '#') {
                    wprintw(msgWin, curNode->str, extra);
                } else {
                    waddstr(msgWin, curNode->str);
                }
                wrefresh(msgWin);
                return;
            }
        }

        // a bit of special-casing, because "delays" may be attached to any
        // command
        if (ch == 'a' && curNode->func) {
            delayedCmd = curNode->func;
            delayedExtra = extra;
            curNode = getDelayTree(curNode);
            goto outputNode; // spaghetti code! \o/
        }
    }
}
