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
} TreeNode;

extern TreeNode commands;
void initializeCommands();

TreeNode *mk1tn(TreeNode n1) {
    TreeNode *a = malloc(1 * sizeof(TreeNode));
    a[0] = n1;
    return a;
}

TreeNode *mk2tn(TreeNode n1, TreeNode n2) {
    TreeNode *a = malloc(2 * sizeof(TreeNode));
    a[0] = n1;
    a[1] = n2;
    return a;
}

TreeNode *mk3tn(TreeNode n1, TreeNode n2, TreeNode n3) {
    TreeNode *a = malloc(3 * sizeof(TreeNode));
    a[0] = n1;
    a[1] = n2;
    a[2] = n3;
    return a;
}

#endif
