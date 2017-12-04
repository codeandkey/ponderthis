#include "verify.h"
#include "log.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))

int minpcache[VERIFY_MAXNODES * VERIFY_MAXNODES] = {0};
int _verify(node* graph, int n);

int verify(char* fn) {
	FILE* f = fopen(fn, "r");

	if (!f) {
		_log("failed to open [%s] for reading\n", fn);
		return -1;
	}

	_log("verifying node graph from [%s]\n", fn);

	char buf[VERIFY_BUFLEN] = {0};
	node nodes[VERIFY_MAXNODES] = {0};
	int cnode = 0;

	while(fgets(buf, sizeof buf, f)) {
		++cnode;
		nodes[cnode].value = cnode;
		int cchild = 0;
		for (char* c = strtok(buf, " "); c; c = strtok(NULL, " ")) {
			int nc = strtol(c, NULL, 10);
			nodes[cnode].children[cchild] = nc;
			cchild++;
		}
	}

	fclose(f);
	return _verify(nodes, cnode);
}

int _verify(node* nodes, int cnode) {
	int numchildren = -1, maxref = 0, mrefnode = 0;

	for (int i = 1; i <= cnode; ++i) {
		int cchild = 0;
		for (int* c = nodes[i].children; *c; ++c) {
			int nc = *c;
			if (nc > maxref) {
				maxref = nc;
				mrefnode = cnode;
			}
			if (nc <= 0) {
				_log("invalid connection: node %d refers to invalid node %d\n", i, nc);
				return -1;
			}
			if (nc == i) {
				_log("invalid connection: node %d refers to itself\n", i);
				return -1;
			}
			for (int dup = 0; dup < cchild; ++dup) {
				if (nc == nodes[i].children[dup]) {
					_log("invalid connection: duplicate connection from %d to %d\n", i, nc);
					return -1;
				}
			}
			cchild++;
		}
		if (numchildren < 0) numchildren = cchild;
		if (numchildren && cchild != numchildren) {
			_log("invalid number of children: expected %d, parsed %d on node %d\n", numchildren, cchild, i);
			return -1;
		}
	}

	if (maxref > cnode) {
		_log("invalid connection: %d refers to out-of-range node %d [%d nodes total]\n", mrefnode, maxref, cnode);
		return -1;
	}

	_log("parsed %d nodes from input\n", cnode);

	/* first, verify that all connections are two-way */
	_log("testing for two-way connection failures\n");

	for (int i = 0; i < cnode + 1; ++i) {
		for (int* c = nodes[i].children; *c; ++c) {
			int loc = 0;
			for (int* j = nodes[*c].children; *j; ++j) {
				if (*j == i) loc = 1;
			}

			if (!loc) {
				_log("one-way connection detected from %d to %d\n", i, *c);
				return -1;
			}
		}
	}

	_log("all connections verified, testing for maximum path lengths\n");

	memset(minpcache, 0, sizeof minpcache);

	int maxlen = 0, sola[VERIFY_MAXSOLS], solb[VERIFY_MAXSOLS], solcnt = 0;

	for (int i = 1; i < cnode; ++i) {
		for (int j = i + 1; j < cnode + 1; ++j) {
			/* test for length between i and j */
			int len = cminpath(nodes, cnode, i, j);

			if (!len) {
				_log("failed to find any valid paths from %d to %d\n", i, j);
				return -1;
			}

			if (len > maxlen) {
				maxlen = len;
				solcnt = 0;
			}

			if (len >= maxlen) {
				sola[solcnt] = i;
				solb[solcnt++] = j;
			}
		}
	}

	_log("maximum path length: %d hops\n", maxlen);
	_log("solution set [%d total]", solcnt);

	if (solcnt <= VERIFY_HIDESOLS) {
		for (int i = 0; i < solcnt; ++i) {
			if (!(i%((int) sqrt(solcnt)))) {
				_lognf("\n");
				_lognfstart();
			}

			_lognf(" (%d => %d)", sola[i], solb[i]);
		}

		_lognf("\n");
	} else {
		_log("(omitting large solution set)\n");
	}

	_log("successfully verified %dN*%dC graph data\n", cnode, numchildren);
	return maxlen;
}

int cminpath(node* nodes, int n, int i, int j) {
	int ind = min(i,j)*VERIFY_MAXNODES+max(i,j); /* symmetric cache so we don't have to worry really */
	if (minpcache[ind] <= 0) minpcache[ind] = minpath(nodes, n, i, j);
	return minpcache[ind];
}

int minpath(node* nodes, int n, int i, int j) {
	/* iterative approach: probably fastest */
	int *d = malloc(sizeof(*d) * (n + 1)), *d2 = malloc(sizeof(*d) * (n + 1)), *dead = malloc(sizeof(*d) * (n + 1));
	memset(d, 0, sizeof(*d) * (n+1));
	memset(d2, 0, sizeof(*d2) * (n+1));
	memset(dead, 0, sizeof(*dead) * (n+1));
	int dc = 1, dc2 = 0, chops = 1;
	int *dcur = d, *dnext = d2, *dtmp;
	*dcur = i;

	while (1) {
		for (int t = 0; t < dc; ++t) {
			dead[dcur[t]] = 1;

			for (int* c = nodes[dcur[t]].children; *c; ++c) {
				if (*c == j) goto cleanup; /* faster than tracking states okay */
				if (dead[*c]) continue;
	
				dnext[dc2++] = *c;
				dead[*c] = 1;
			}
		}

		if (!dc2) {
			chops = 0; /* no possible paths, get to cleanup */
			break;
		}

		dtmp = dcur;
		dcur = dnext;
		dnext = dtmp;
		dc = dc2;
		dc2 = 0;
		chops++;
	}

	cleanup:
	free(d);
	free(d2);
	free(dead);
	return chops;
}

