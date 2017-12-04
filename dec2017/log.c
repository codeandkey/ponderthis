#include "log.h"

#include <time.h>
#include <unistd.h>

static FILE* fout;
int use_color = 0, v_level = 0xFF, use_time = 0;

#define BASESTR "(%d-%m-%Y %H:%M:%S) "
#define COLORSTR "\e[0;32m" BASESTR "\e[0;39m"

void set_log_dest(FILE* out) {
	fout = out;
	use_color = isatty(fileno(out));
}

void set_log_verbose(int lv) {
	v_level = lv;
}

void set_log_time(int use) {
	use_time = use;
}

void do_log(const char* fmt, ...) {
	char dbuf[LOGBUF];
	va_list v;
	va_start(v, fmt);
	if (use_time) {
		time_t t = time(NULL);
		struct tm* lt = localtime(&t);
		strftime(dbuf, sizeof(dbuf) - 1, use_color ? COLORSTR : BASESTR, lt);
		fprintf(fout, dbuf);
	}
	vfprintf(fout, fmt, v);
	va_end(v);
}

void do_lognf(const char* fmt, ...) {
	va_list v;
	va_start(v, fmt);
	vfprintf(fout, fmt, v);
	va_end(v);
}

void do_lognfstart(void) {
	if (!use_time) return;
	char dbuf[LOGBUF];
	time_t t = time(NULL);
	struct tm* lt = localtime(&t);
	strftime(dbuf, sizeof(dbuf) - 1, use_color ? COLORSTR : BASESTR, lt);
	fprintf(fout, dbuf);
}
