#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "data.h"

typedef void (*callback)(AtcsoData);

struct treeNode_t;
typedef struct treeNode_t {
    char hotkey;
    char *str;
    callback func;
    struct treeNode_t *children;
    int nChildren;
} TreeNode;

#endif
