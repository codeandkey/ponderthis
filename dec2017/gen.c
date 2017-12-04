#include "gen.h"
#include "log.h"
#include "node.h"
#include "verify.h"

#include <string.h>

int node_connect(node* a, node* b, int max_edges);
int _node_connect(node* a, node* b, int e);

int _gen3(int nodes, int edges, int target, void (*emit)(node* nodes, int n, void* d), void* d);
int _gen_spec(int nodes, int edges, int target, void (*emit)(node* nodes, int n, void* d), void* d);

int _gengraph(int nodes, int edges, int target, void (*emit)(node* nodes, int n, void* d), void* d) {
	return _gen_spec(nodes, edges, target, emit, d);
	/* return _gen3(nodes, edges, target, emit, d); 3-edge generating */
}

int _output_sol(node* nodes, int num) {
	FILE* out = fopen("sol", "w");

	for (int i = 1; i <= num; ++i) {
		for (int* c = nodes[i].children; *c; ++c) {
			fprintf(out, "%d ", *c);
		}
		fprintf(out, "\n");
	}

	fclose(out);
	return 0;
}

void _output_graph(node* nodes, int num) {
	for (int i = 1; i <= num; ++i) {
		_lognfstart();
		_lognf("[%d] => ", i);
		for (int* c = nodes[i].children; *c; ++c) {
			_lognf("%d ", *c);
		}
		_lognf("\n");
	}
}

void _debug_graph(node* nodes, int num) {
	for (int i = 0; i < num; ++i) {
		_lognfstart();
		_lognf("[%d] => ", nodes[i].value);
		for (int* c = nodes[i].children; *c; ++c) {
			_lognf("%d ", *c);
		}
		_lognf("\n");
	}
}

void _gen_emit(node* nodes, int n, void* d) {
	_log("solution emitted: \n");
	_output_graph(nodes, n);
	_log("verifying..\n");
	_verify(nodes, n);
	_output_sol(nodes, n);
	++*((int*) d);
}

int gen(int nodes, int edges, int target) {
	_log("starting generating procedure for n=%d, edges=%d\n", nodes, edges);

	if (nodes <= 0 || edges <= 0 || edges >= nodes) {
		_log("invalid node, edge selection\n");
		return -1;
	}

	_log("starting search for complete graph with target=%d\n", target);

	node* graph_out = malloc(sizeof(*graph_out)*(nodes+1));
	memset(graph_out, 0, sizeof(*graph_out)*(nodes+1));

	int num_sols = 0;

	_gengraph(nodes, edges, target, _gen_emit, &num_sols);

	_log("found %d solutions for N%d E%d with diameter >= %d\n", num_sols, nodes, edges, target);

	return 0;
}

int _gen3(int nodes, int edges, int target, void (*emit)(node* nodes, int n, void* d), void* d) {
	_log("starting specialized 3-edge generation for target %d\n", target);

	if (!target) {
		_log("bipartite search cannot be performed without a base target\n");
		return -1;
	}

	if (edges != 3) {
		_log("specialized generator only works for 3-edge graphs\n");
		return -1;
	}

	if (target < 3) {
		_log("target under algorithm range\n");
		return -1;
	}

	/* we don't actually need to do much, the minimum graph is easily predictable. */
	int inner_nodes = target - 3;
	int doubles = inner_nodes / 2;
	int singles = inner_nodes & 1;
	int width = 2 * doubles - 1 + singles + 4;
	int total_nodes = 2 * width;

	node* og = malloc(sizeof(*og) * (total_nodes + 1));
	memset(og, 0, sizeof(*og) * (total_nodes + 1));

	for (int i = 1; i <= total_nodes; ++i) {
		og[i].value = i;
	}

	_log("constructing 3-graph, inner %d, predicted minimum nodes = %d\n", inner_nodes, total_nodes);

	/* construct 4-corners */
	int cres = 1;
	cres &= node_connect(og + 1, og + width + 2, edges);
	cres &= node_connect(og + 2, og + width + 1, edges);
	cres &= node_connect(og + width - 1, og + width * 2, edges);
	cres &= node_connect(og + width, og + width * 2 - 1, edges);

	if (!cres) {
		_log("corner connections failed\n");
		free(og);
		return 0;
	}

	cres &= node_connect(og + 1, og + width + 1, edges);
	cres &= node_connect(og + 1, og + 2, edges);
	cres &= node_connect(og + width + 1, og + width + 2, edges);

	cres &= node_connect(og + width, og + width * 2, edges);
	cres &= node_connect(og + width, og + width - 1, edges);
	cres &= node_connect(og + width * 2, og + width * 2 - 1, edges);

	if (!cres) {
		_log("start/finish block connections failed\n");
		free(og);
		return 0;
	}

	int cur = 3;

	if (singles) {
		_log("performing singles connection at cur=%d\n", cur);

		cres &= node_connect(og + cur - 1, og + cur, edges);
		cres &= node_connect(og + width + cur - 1, og + width + cur, edges);
		cres &= node_connect(og + cur, og + cur + width, edges);

		if (!cres) {
			_log("singles block connections failed\n");
			free(og);
			return 0;
		}

		cur++;
	}

	_log("appending any doubles, cur=%d / %d\n", cur, width);
	while (cur <= width - 2) {
		_log("inserting double, cur = %d\n", cur);

		cres &= node_connect(og + cur - 1, og + cur, edges);
		cres &= node_connect(og + width + cur - 1, og + cur, edges);
		cres &= node_connect(og + cur, og + cur + width, edges);
		cres &= node_connect(og + cur + 1, og + cur + width, edges);
		cres &= node_connect(og + width + cur, og + cur + width + 1, edges);

		if (cur != width - 2) {
			node_connect(og + cur + 1, og + cur + 1 + width, edges);
		}

		if (cur == 3) {
			node_connect(og + cur - 1, og + cur - 1 + width, edges);
		}

		if (!cres) {
			_log("doubles block connections failed\n");
			free(og);
			return 0;
		}

		cur += 2;
	}

	_log("finalized graph:\n");
	emit(og, total_nodes, d);

	free(og);
	return 0;
}

int _iter_pb(int a, int b, int* k) {
	_log("(%d, %d)\n", a, b);

	if (a - 1 == b && b > 1) _iter_pb(a - 1, b - 1, k);

	if (k) {
		for (int* ck = k + (a*8); *ck; ++ck) {
			if (b - *ck >= 0) _iter_pb(a, b - *ck, NULL);
		}
	}

	return 0;
}

int _gen_spec(int nodes, int edges, int target, void (*emit)(node* nodes, int n, void* d), void* d) {
	_log("starting special generation process following known kchain\n");

	if (nodes != 100 || edges != 9 || target != 27) {
		_log("this process uses a known k-chain, cannot be generalized to any other parameters\n");
		return -1;
	}

	/* first, we construct the initblock, a 2->8->5 partition chain with only 2 k-transforms */

	node init2[2] = {0}, init8[8] = {0}, init5[5] = {0};

	for (int i = 1; i <= 2; ++i) {
		init2[i - 1].value = i;
	}

	for (int i = 1; i <= 8; ++i) {
		init8[i - 1].value = i + 2;
	}

	for (int i = 1; i <= 5; ++i) {
		init5[i - 1].value = i + 10;
	}

	node_connect(init2, init2 + 1, edges); /* perform k=1 on init2, 2x9 -> 2x8 output */

	/* connect first two init partitions */
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 8; ++j) {
			node_connect(init2 + i, init8 + j, edges);
		}
	}

	/* perform k=2 on init8, 8x7 -> 8x5 output */
	node_connect(init8, init8 + 7, edges);
	for (int i = 0; i < 7; ++i) {
		node_connect(init8 + i, init8 + i + 1, edges);
	}

	/* connect second, third init partitions */
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 5; ++j) {
			node_connect(init8 + i, init5 + j, edges);
		}
	}

	/* third partition should be at 5x1, prepping us for the chain... */
	_log("emitting init2");
	_debug_graph(init2, 2);
	_log("emitting init8");
	_debug_graph(init8, 8);
	_log("emitting init5");
	_debug_graph(init5, 5);

	int c5base = init5->value;

	node g[101];
	memset(g, 0, sizeof g);

	memcpy(g + 1, init2, sizeof init2);
	memcpy(g + 3, init8, sizeof init8);
	memcpy(g + 11, init5, sizeof init5);

	for (int i = 16; i <= 100; ++i) {
		g[i].value = i;
	}

	for (int ch = 0; ch < 8; ++ch) {
		int cchainbase = c5base + 5;

		/* no k-transforms on the first 1-partition which is nice. */
		for (int i = 0; i < 5; ++i) {
			node_connect(&g[c5base + i], &g[cchainbase], edges);
		}

		/* output for 1st chain partition: 1x4 */

		for (int i = 0; i < 4; ++i) {
			node_connect(&g[cchainbase], &g[cchainbase + 1 + i], edges);
		}

		/* output for second chain partition: 4x8, k-transform to 4x5 */
		node_connect(&g[cchainbase + 1], &g[cchainbase + 4], edges);
		for (int i = 0; i < 3; ++i) {
			node_connect(&g[cchainbase + 1 + i], &g[cchainbase + 2 + i], edges);
		}
		node_connect(&g[cchainbase + 1], &g[cchainbase + 3], edges);
		node_connect(&g[cchainbase + 2], &g[cchainbase + 4], edges);

		/* connect second, third */

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 5; ++j) {
				node_connect(&g[cchainbase + 1 + i], &g[cchainbase + 5 + j], edges);
			}
		}

		/* k-transform the third partition from 5x5 to 5x1 iff it isn't the last one. */
		if (ch != 7) {
			for (int i = 0; i < 4; ++i) {
				for (int j = i + 1; j < 5; ++j) {
					node_connect(&g[cchainbase + 5 + i], &g[cchainbase + 5 + j], edges);
				}
			}
		}

		c5base = cchainbase + 5;
	}

	_log("finished chaining, constructing terminating block\n");

	for (int i = 0; i < 5; ++i) {
		for (int j = 96; j <= 100; ++j) {
			node_connect(&g[c5base + i], &g[j], edges);
		}
	}

	/* finally, k-transform the last block from 5x4 to 5x0, and finish the graph :) */

	for (int i = 0; i < 4; ++i) {
		for (int j = i + 1; j < 5; ++j) {
			node_connect(&g[96 + i], &g[96 + j], edges);
		}
	}

	_log("emitting sol\n");
	emit(g, 100, d);
	_log("all done :) \n");
	return 0;
}

/* 0: connection failed, 1: connection succeeded, 2: connection already exists */
int node_connect(node* a, node* b, int e) {
	int r = _node_connect(a, b, e);
	if (r != 1) {
		_log("node_connect failure from (%d => %d)\n", a->value, b->value);
	}
	return r;
}

int _node_connect(node* a, node* b, int max_edges) {
	int ia = 1, ib = 1;
	int *da = a->children, *db = b->children;

	if (a == b) return 0;
	if (a->value == b->value) return 0;

	while (*da) {
		if (*da == b->value) return 2;
		da++;
		ia++;
	}

	if (ia > max_edges) {
		return 0;
	}

	while (*db) {
		if (*db == a->value) return 2;
		db++;
		ib++;
	}

	if (ib > max_edges) {
		return 0;
	}

	*da = b->value;
	*db = a->value;

	return 1;
}
