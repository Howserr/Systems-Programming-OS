#ifndef _EDGE_LIST_H
#define _EDGE_LIST_H

#include <stdint.h>

typedef struct edge edge;
struct edge {
    int yMax;
    int yMin;
    int x;
    int sign;
    int dX;
    int dY;
    int sum;
};

typedef struct node {
    edge *edge;
    struct node *next;
} node;

void PrintListY(node *head);
void PrintListX(node *head);
int GetListSize(node *head);
node * GetNodeFromList(node *head, int index);
void AddToList(node **head, node *nodeToAdd);
bool RemoveFromList(node **head, edge *edgeToRemove);
void SwapInEdgeList(node *head, int first, int second);
void SortEdgeListByY(node *edgeListHead);
void SortEdgeListByX(node *edgeListHead);

#endif