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
#include <stdarg.h>
#include <assert.h>
#include <time.h>

struct message {
	const char *fmt;
	va_list args;
	const char *file;
	int line;
	const char *function;
	int loglevel;
	struct tm *time;
};

struct loglevel {
	FILE *fp;
	const char *color;
	const char *prefix;
	const char *time_fmt;
	int print_enabled;
	int print_function;
	int print_color;
	int print_time;
};

static struct loglevel loglevels[LOG_ENUM_LENGTH + 1];

static void log_to_output(struct message *m)
{
	struct loglevel *l = &loglevels[m->loglevel];

	if(!l->print_enabled)
		return;

	const char *color;
	const char *color_reset;

	if(l->print_color)
	{
		color = l->color;
		color_reset = LOG_COLOR_RESET;
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

void log_set_stream(int loglevel, FILE *stream)
{
	assert(loglevel >= 0 && loglevel <= LOG_ENUM_LENGTH);

	struct loglevel *l = &loglevels[loglevel];
	l->fp = stream;
}

void log_set_color(int loglevel, const char *color)
{
	assert(loglevel >= 0 && loglevel <= LOG_ENUM_LENGTH);

	struct loglevel *l = &loglevels[loglevel];
	l->color = color;
}

void log_set_prefix(int loglevel, const char *prefix)
{
	assert(loglevel >= 0 && loglevel <= LOG_ENUM_LENGTH);

	struct loglevel *l = &loglevels[loglevel];
	l->prefix = prefix;
}

void log_set_time_fmt(int loglevel, const char *time_fmt)
{
	assert(loglevel >= 0 && loglevel <= LOG_ENUM_LENGTH);

	struct loglevel *l = &loglevels[loglevel];
	l->time_fmt = time_fmt;
}

void log_print_enable(int loglevel, int set)
{
	assert(loglevel >= 0 && loglevel <= LOG_ENUM_LENGTH);

	struct loglevel *l = &loglevels[loglevel];
	l->print_enabled = set;
}

void log_print_function(int loglevel, int set)
{
	assert(loglevel >= 0 && loglevel <= LOG_ENUM_LENGTH);

	struct loglevel *l = &loglevels[loglevel];
	l->print_function = set;
}

void log_print_color(int loglevel, int set)
{
	assert(loglevel >= 0 && loglevel <= LOG_ENUM_LENGTH);

#ifdef __WIN32__
	set = 0; // Printing colors on Windows is not supported
#endif
	// TODO: Check if output stream supports color

	struct loglevel *l = &loglevels[loglevel];
	l->print_color = set;
}

void log_print_time(int loglevel, int set)
{
	assert(loglevel >= 0 && loglevel <= LOG_ENUM_LENGTH);

	struct loglevel *l = &loglevels[loglevel];
	l->print_time = set;
}

void log_setup_default(void)
{
	// Trace
	log_set_stream(LOG_TRACE, stdout);
	log_set_color(LOG_TRACE, LOG_COLOR_RESET);
	log_set_prefix(LOG_TRACE, "[TRACE]");
	log_set_time_fmt(LOG_TRACE, "(%H:%M:%S)");
	log_print_enable(LOG_TRACE, 1);
	log_print_function(LOG_TRACE, 0);
	log_print_color(LOG_TRACE, 0);
	log_print_time(LOG_TRACE, 1);
	// Debug
	log_set_stream(LOG_DEBUG, stdout);
	log_set_color(LOG_DEBUG, LOG_COLOR_GREEN);
	log_set_prefix(LOG_DEBUG, "[DEBUG]");
	log_set_time_fmt(LOG_DEBUG, "(%H:%M:%S)");
	log_print_enable(LOG_DEBUG, 1);
	log_print_function(LOG_DEBUG, 1);
	log_print_color(LOG_DEBUG, 1);
	log_print_time(LOG_DEBUG, 1);
	// Info
	log_set_stream(LOG_INFO, stdout);
	log_set_color(LOG_INFO, LOG_COLOR_BLUE);
	log_set_prefix(LOG_INFO, "[INFO]");
	log_set_time_fmt(LOG_INFO, "(%H:%M:%S)");
	log_print_enable(LOG_INFO, 1);
	log_print_function(LOG_INFO, 0);
	log_print_color(LOG_INFO, 1);
	log_print_time(LOG_INFO, 1);
	// Warning
	log_set_stream(LOG_WARNING, stderr);
	log_set_color(LOG_WARNING, LOG_COLOR_YELLOW);
	log_set_prefix(LOG_WARNING, "[WARNING]");
	log_set_time_fmt(LOG_WARNING, "(%H:%M:%S)");
	log_print_enable(LOG_WARNING, 1);
	log_print_function(LOG_WARNING, 1);
	log_print_color(LOG_WARNING, 1);
	log_print_time(LOG_WARNING, 1);
	// Error
	log_set_stream(LOG_ERROR, stderr);
	log_set_color(LOG_ERROR, LOG_COLOR_RED);
	log_set_prefix(LOG_ERROR, "[ERROR]");
	log_set_time_fmt(LOG_ERROR, "(%H:%M:%S)");
	log_print_enable(LOG_ERROR, 1);
	log_print_function(LOG_ERROR, 1);
	log_print_color(LOG_ERROR, 1);
	log_print_time(LOG_ERROR, 1);
	// Fatal
	log_set_stream(LOG_FATAL, stderr);
	log_set_color(LOG_FATAL, LOG_COLOR_BOLD_RED);
	log_set_prefix(LOG_FATAL, "[FATAL]");
	log_set_time_fmt(LOG_FATAL, "(%H:%M:%S)");
	log_print_enable(LOG_FATAL, 1);
	log_print_function(LOG_FATAL, 1);
	log_print_color(LOG_FATAL, 1);
	log_print_time(LOG_FATAL, 1);
}

void _log(int loglevel, const char *file, int line, const char *function, const char *fmt, ...)
{
	assert(loglevel >= 0 && loglevel <= LOG_ENUM_LENGTH);

	time_t t = time(NULL);

	struct message m = {
		.fmt = fmt,
		.file = file,
		.line = line,
		.function = function,
		.loglevel = loglevel,
		.time = localtime(&t),
	};

	va_start(m.args, fmt);
	log_to_output(&m);
	va_end(m.args);
}
