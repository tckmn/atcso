#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "data.h"

struct treeNode_t;
typedef struct treeNode_t {
    char hotkey;
    char *str;
    void (*func)(AtcsoData*, char plane, char extra);
    struct treeNode_t *children;
    int nChildren;
    struct treeNode_t *parent;
} TreeNode;

extern TreeNode commands;
void initializeCommands();

#endif
