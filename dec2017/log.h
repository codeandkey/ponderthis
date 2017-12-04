#pragma once

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#define LOGBUF 256

#define _log(x, ...) do_log("[%s:%d] " x, __FILE__, __LINE__, ##__VA_ARGS__)
#define _lognf(x, ...) do_lognf(x, ##__VA_ARGS__)
#define _lognfstart do_lognfstart

void do_log(const char* fmt, ...);
void do_lognf(const char* fmt, ...);
void do_lognfstart(void);
void set_log_dest(FILE* dest);
void set_log_verbose(int verbose);
void set_log_time(int use_time);
