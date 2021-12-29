/*
    This file is part of selog.

    selog is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    selog is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with selog. If not, see <https://www.gnu.org/licenses/>.
*/
#include <selog/selog.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>

static time_t init_time;

struct message {
	const char *fmt;
	va_list args;
	const char *file;
	int line;
	const char *function;
	struct loglevel *loglevel;
	struct tm *time;
};

struct loglevel {
	FILE *fp;
	const char *color;
	const char *prefix;
	const char *time_fmt;
	int time_relation;
	int print_enabled;
	int print_function;
	int print_color;
	int print_time;
};

static struct loglevel loglevels[SELOG_ENUM_LENGTH + 1];

static void log_to_output(struct message *m)
{
	struct loglevel *l = m->loglevel;

	const char *color;
	const char *color_reset;

	if(l->print_color)
	{
		color = l->color;
		color_reset = SELOG_COLOR_RESET;
	} else {
		color = "";
		color_reset = "";
	}

	char time_buff[16];
	if(l->print_time)
		time_buff[strftime(time_buff, sizeof(time_buff) - 1, l->time_fmt, m->time)] = '\0';
	else
		time_buff[0] = '\0';

	// Print message prefix
	if(l->print_function)
		fprintf(l->fp, "%s%s%s %s()->%s:%d:%s ", time_buff, color, l->prefix, m->function, m->file, m->line, color_reset);
	else
		fprintf(l->fp, "%s%s%s:%s ", time_buff, color, l->prefix, color_reset);

	// Print message
	vfprintf(l->fp, m->fmt, m->args);
	putc('\n', l->fp);

	fflush(l->fp);
}

void selog_set_stream(int loglevel, FILE *stream)
{
	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	struct loglevel *l = &loglevels[loglevel];
	l->fp = stream;
}

void selog_set_color(int loglevel, const char *color)
{
	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	struct loglevel *l = &loglevels[loglevel];
	l->color = color;
}

void selog_set_prefix(int loglevel, const char *prefix)
{
	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	struct loglevel *l = &loglevels[loglevel];
	l->prefix = prefix;
}

void selog_set_time_fmt(int loglevel, const char *time_fmt)
{
	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	struct loglevel *l = &loglevels[loglevel];
	l->time_fmt = time_fmt;
}

void selog_set_time_relation(int loglevel, int set)
{
	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	struct loglevel *l = &loglevels[loglevel];
	l->time_relation = set;
}

void selog_print_enable(int loglevel, int set)
{
	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	struct loglevel *l = &loglevels[loglevel];
	l->print_enabled = set;
}

void selog_print_function(int loglevel, int set)
{
	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	struct loglevel *l = &loglevels[loglevel];
	l->print_function = set;
}

void selog_print_color(int loglevel, int set)
{
	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	struct loglevel *l = &loglevels[loglevel];

	if(!isatty(fileno(l->fp)))
		set = 0;

#ifdef __WIN32__
	set = 0; // Printing colors on Windows is not supported
#endif

	l->print_color = set;
}

void selog_print_time(int loglevel, int set)
{
	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	struct loglevel *l = &loglevels[loglevel];
	l->print_time = set;
}

void selog_setup_default(void)
{
	init_time = time(NULL);
	// Trace
	selog_set_stream(SELOG_TRACE, stdout);
	selog_set_color(SELOG_TRACE, SELOG_COLOR_RESET);
	selog_set_prefix(SELOG_TRACE, "[TRACE]");
	selog_set_time_fmt(SELOG_TRACE, "(%H:%M:%S)");
	selog_set_time_relation(SELOG_TRACE, SELOG_TIME_EPOCH);
	selog_print_enable(SELOG_TRACE, 1);
	selog_print_function(SELOG_TRACE, 0);
	selog_print_color(SELOG_TRACE, 0);
	selog_print_time(SELOG_TRACE, 1);
	// Debug
	selog_set_stream(SELOG_DEBUG, stdout);
	selog_set_color(SELOG_DEBUG, SELOG_COLOR_GREEN);
	selog_set_prefix(SELOG_DEBUG, "[DEBUG]");
	selog_set_time_fmt(SELOG_DEBUG, "(%H:%M:%S)");
	selog_set_time_relation(SELOG_DEBUG, SELOG_TIME_EPOCH);
	selog_print_enable(SELOG_DEBUG, 1);
	selog_print_function(SELOG_DEBUG, 1);
	selog_print_color(SELOG_DEBUG, 1);
	selog_print_time(SELOG_DEBUG, 1);
	// Info
	selog_set_stream(SELOG_INFO, stdout);
	selog_set_color(SELOG_INFO, SELOG_COLOR_BLUE);
	selog_set_prefix(SELOG_INFO, "[INFO]");
	selog_set_time_fmt(SELOG_INFO, "(%H:%M:%S)");
	selog_set_time_relation(SELOG_INFO, SELOG_TIME_EPOCH);
	selog_print_enable(SELOG_INFO, 1);
	selog_print_function(SELOG_INFO, 0);
	selog_print_color(SELOG_INFO, 1);
	selog_print_time(SELOG_INFO, 1);
	// Warning
	selog_set_stream(SELOG_WARNING, stderr);
	selog_set_color(SELOG_WARNING, SELOG_COLOR_YELLOW);
	selog_set_prefix(SELOG_WARNING, "[WARNING]");
	selog_set_time_fmt(SELOG_WARNING, "(%H:%M:%S)");
	selog_set_time_relation(SELOG_WARNING, SELOG_TIME_EPOCH);
	selog_print_enable(SELOG_WARNING, 1);
	selog_print_function(SELOG_WARNING, 1);
	selog_print_color(SELOG_WARNING, 1);
	selog_print_time(SELOG_WARNING, 1);
	// Error
	selog_set_stream(SELOG_ERROR, stderr);
	selog_set_color(SELOG_ERROR, SELOG_COLOR_RED);
	selog_set_prefix(SELOG_ERROR, "[ERROR]");
	selog_set_time_fmt(SELOG_ERROR, "(%H:%M:%S)");
	selog_set_time_relation(SELOG_ERROR, SELOG_TIME_EPOCH);
	selog_print_enable(SELOG_ERROR, 1);
	selog_print_function(SELOG_ERROR, 1);
	selog_print_color(SELOG_ERROR, 1);
	selog_print_time(SELOG_ERROR, 1);
	// Fatal
	selog_set_stream(SELOG_FATAL, stderr);
	selog_set_color(SELOG_FATAL, SELOG_COLOR_BOLD_RED);
	selog_set_prefix(SELOG_FATAL, "[FATAL]");
	selog_set_time_fmt(SELOG_FATAL, "(%H:%M:%S)");
	selog_set_time_relation(SELOG_FATAL, SELOG_TIME_EPOCH);
	selog_print_enable(SELOG_FATAL, 1);
	selog_print_function(SELOG_FATAL, 1);
	selog_print_color(SELOG_FATAL, 1);
	selog_print_time(SELOG_FATAL, 1);
}

void selog_printf(int loglevel, const char *file, int line, const char *function, const char *fmt, ...)
{
	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	struct loglevel *l = &loglevels[loglevel];

	if(!l->print_enabled)
		return;

	struct message m = {
		.fmt = fmt,
		.file = file,
		.line = line,
		.function = function,
		.loglevel = l,
	};
	time_t t = time(NULL);
	if(l->time_relation == SELOG_TIME_EPOCH)
	{
		m.time = localtime(&t);
	} else {
		t -= init_time;
		m.time = gmtime(&t);
	}

	va_start(m.args, fmt);
	log_to_output(&m);
	va_end(m.args);
}

void selog_vprintf(int loglevel, const char *file, int line, const char *function, const char *fmt, va_list args)
{
	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	struct loglevel *l = &loglevels[loglevel];

	if(!l->print_enabled)
		return;

	struct message m = {
		.fmt = fmt,
		.file = file,
		.line = line,
		.function = function,
		.loglevel = l,
	};
	time_t t = time(NULL);
	if(l->time_relation == SELOG_TIME_EPOCH)
	{
		m.time = localtime(&t);
	} else {
		t -= init_time;
		m.time = gmtime(&t);
	}

	va_copy(m.args, args);
	log_to_output(&m);
}
