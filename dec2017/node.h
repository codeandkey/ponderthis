#pragma once

#define MAXCHILDREN 1024

typedef struct _node {
	int value, children[MAXCHILDREN];
} node;
