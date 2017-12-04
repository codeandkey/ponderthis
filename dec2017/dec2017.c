/*
 * Justin Stanley
 * IBM december 2017
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "node.h"
#include "log.h"
#include "gen.h"
#include "verify.h"

#define DEFAULT_NODES 12
#define DEFAULT_EDGES 3
#define DEFAULT_VERBOSE 0
#define DEFAULT_LOGTIME 1

#define MODE_VERIFY 0
#define MODE_GEN 1
#define MODE_HELP 2

int help(char* a0);

int main(int argc, char** argv) {
	int nodes = DEFAULT_NODES, edges = DEFAULT_EDGES, mode = MODE_HELP, target = 0;
	set_log_dest(stdout);

	set_log_time(DEFAULT_LOGTIME);

	char* verify_filename = NULL;

	for (int i = 0; i < argc; ++i) {
		if (!strcmp(argv[i], "verify")) {
			mode = MODE_VERIFY;

			if (++i < argc) {
				verify_filename = argv[i++];
			} else {
				mode = MODE_HELP;
			}

			continue;
		}

		if (!strcmp(argv[i], "gen")) {
			mode = MODE_GEN;
			i++;

			if (i < argc) {
				nodes = strtol(argv[i++], NULL, 10);
			}

			if (i < argc) {
				edges = strtol(argv[i++], NULL, 10);
			}

			if (i < argc) {
				target = strtol(argv[i++], NULL, 10);
			}

			continue;
		}

		if (!strcmp(argv[i], "log")) {
			++i;

			if (!strncmp(argv[i], "verbose", 7)) {
				strtok(argv[i], "=");
				char* vflag = strtok(NULL, "=");

				if (strlen(vflag)) {
					int nv = strtol(vflag, NULL, 10);
					set_log_verbose(nv);
					_log("set log verbosity to %d\n", nv);
				} else {
					mode = MODE_HELP;
				}
			}

			if (!strncmp(argv[i], "time", 4)) {
				strtok(argv[i], "=");
				char* vflag = strtok(NULL, "=");

				if (strlen(vflag)) {
					set_log_time(strtol(vflag, NULL, 10));
				} else {
					mode = MODE_HELP;
				}
			}

			continue;
		}
	}

	_log("starting subroutine for mode %d\n", mode);

	int result = 0;
	switch (mode) {
	case MODE_VERIFY:
		result = (verify(verify_filename) > 0);
		break;
	case MODE_GEN:
		result = gen(nodes, edges, target);
		break;
	default:
	case MODE_HELP:
		return help(argv[0]);
	}

	if (result) {
		_log("subroutine terminated with failure code %d\n", result);
	} else {
		_log("subroutine terminated successfully\n");
	}

	return result;
}

int help(char* a0) {
	_log("usage: %s [mode [args]]\n", a0);
	_log("modes: \n");
	_log("verify <filename>\n");
	_log("gen [nodes=%d] [edges=%d] [target]\n", DEFAULT_NODES, DEFAULT_EDGES);
	_log("log (verbose=%d | time=%d)\n", DEFAULT_VERBOSE, DEFAULT_LOGTIME);
	return -1;
}
