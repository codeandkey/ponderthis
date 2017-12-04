#pragma once

#include "node.h"

#define VERIFY_BUFLEN 1024
#define VERIFY_MAXNODES 1024
#define VERIFY_MAXSOLS 32
#define VERIFY_HIDESOLS 32

int minpath(node* nodes, int n, int i, int j);
int cminpath(node* nodes, int n, int i, int j);
int verify(char* filename);
int _verify(node* graph, int n);
