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
#include "include/selog.h"
#include <stdio.h>
#include <assert.h>
#include <time.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <pthread.h>
#endif

/* Structs */
struct message {
	const char *fmt;
	va_list args;
	const char *file;
	int line;
	const char *function;
	struct loglevel *loglevel;
	struct tm *time;
};

/* Variables */
static time_t init_time;
#ifdef _WIN32
static CRITICAL_SECTION mutex;
#else
static pthread_mutex_t mutex;
#endif

struct loglevel _selog_loglevel_trace;
struct loglevel _selog_loglevel_debug;
struct loglevel _selog_loglevel_info;
struct loglevel _selog_loglevel_warning;
struct loglevel _selog_loglevel_error;
struct loglevel _selog_loglevel_fatal;

struct loglevel * const selog_loglevel_trace = &_selog_loglevel_trace;
struct loglevel * const selog_loglevel_debug = &_selog_loglevel_debug;
struct loglevel * const selog_loglevel_info = &_selog_loglevel_info;
struct loglevel * const selog_loglevel_warning = &_selog_loglevel_warning;
struct loglevel * const selog_loglevel_error = &_selog_loglevel_error;
struct loglevel * const selog_loglevel_fatal = &_selog_loglevel_fatal;

/* Declarations */
static void lock(void);
static void unlock(void);
static int log_to_output(struct message *m);

/* Definitions */
static void lock()
{
#ifdef _WIN32
	EnterCriticalSection(&mutex);
#else
	pthread_mutex_lock(&mutex);
#endif
}

static void unlock()
{
#ifdef _WIN32
	LeaveCriticalSection(&mutex);
#else
	pthread_mutex_unlock(&mutex);
#endif
}

static int log_to_output(struct message *m)
{
	int ret = 0;
	int error = 0;
	int size;

	char time_buff[16];

	const char *color;
	const char *color_reset;

	struct loglevel *l = m->loglevel;

	/* Lock mutex */
	lock();

	if(!selog_get_flag(l, SELOG_FLAG_ENABLED))
		return 0;

	if(selog_get_flag(l, SELOG_FLAG_COLOR))
	{
		color = l->color;
		color_reset = SELOG_COLOR_RESET;
	} else {
		color = "";
		color_reset = "";
	}

	if(selog_get_flag(l, SELOG_FLAG_TIME))
		time_buff[strftime(time_buff, sizeof(time_buff) - 1, l->time_fmt, m->time)] = '\0';
	else
		time_buff[0] = '\0';

	/* Print message prefix */
	if(selog_get_flag(l, SELOG_FLAG_FUNCTION))
	{
		if((size = fprintf(l->fp, "%s%s%s %s()->%s:%d:%s ", time_buff, color, l->prefix, m->function, m->file, m->line, color_reset)) < 0)
			error = size;
	} else {
		if((size = fprintf(l->fp, "%s%s%s:%s ", time_buff, color, l->prefix, color_reset)) < 0)
			error = size;
	}
	ret += size;

	/* Print message */
	if((size = vfprintf(l->fp, m->fmt, m->args)) < 0)
		error = size;
	ret += size;
	if((size = fprintf(l->fp, "\n")) < 0)
		error = size;
	ret += size;

	fflush(l->fp);
	/* Unlock mutex */
	unlock();

	return (error) ? error : ret;
}

void selog_set_stream(struct loglevel *l, FILE *stream)
{
	lock();
	l->fp = stream;
	unlock();
}

void selog_set_color(struct loglevel *l, const char *color)
{
	lock();
	l->color = color;
	unlock();
}

void selog_set_prefix(struct loglevel *l, const char *prefix)
{
	lock();
	l->prefix = prefix;
	unlock();
}

void selog_set_time_fmt(struct loglevel *l, const char *time_fmt)
{
	lock();
	l->time_fmt = time_fmt;
	unlock();
}

int selog_set_flag(struct loglevel *l, int flag)
{
	int ret;

	lock();
	ret = (l->flags |= flag);
	unlock();

	return ret;
}

int selog_unset_flag(struct loglevel *l, int flag)
{
	int ret;

	lock();
	ret = (l->flags &= ~flag);
	unlock();

	return ret;
}

int selog_get_flag(struct loglevel *l, int flag)
{
	int ret;

	ret = (l->flags & flag);

	return ret;
}

void selog_setup_default(void)
{
	init_time = time(NULL);
#ifdef _WIN32
	InitializeCriticalSection(&mutex);
#else
	pthread_mutex_init(&mutex, NULL);
#endif
	/* Trace */
	selog_set_stream(selog_loglevel_trace, stdout);
	selog_set_color(selog_loglevel_trace, SELOG_COLOR_RESET);
	selog_set_prefix(selog_loglevel_trace, "[TRACE]");
	selog_set_time_fmt(selog_loglevel_trace, "(%H:%M:%S)");
	selog_set_flag(selog_loglevel_trace, SELOG_FLAG_ALL);
	selog_unset_flag(selog_loglevel_trace, SELOG_FLAG_FUNCTION);
	selog_unset_flag(selog_loglevel_trace, SELOG_FLAG_COLOR);
	/* Debug */
	selog_set_stream(selog_loglevel_debug, stdout);
	selog_set_color(selog_loglevel_debug, SELOG_COLOR_GREEN);
	selog_set_prefix(selog_loglevel_debug, "[DEBUG]");
	selog_set_time_fmt(selog_loglevel_debug, "(%H:%M:%S)");
	selog_set_flag(selog_loglevel_debug, SELOG_FLAG_ALL);
	/* Info */
	selog_set_stream(selog_loglevel_info, stdout);
	selog_set_color(selog_loglevel_info, SELOG_COLOR_BLUE);
	selog_set_prefix(selog_loglevel_info, "[INFO]");
	selog_set_time_fmt(selog_loglevel_info, "(%H:%M:%S)");
	selog_set_flag(selog_loglevel_info, SELOG_FLAG_ALL);
	selog_unset_flag(selog_loglevel_info, SELOG_FLAG_FUNCTION);
	/* Warning */
	selog_set_stream(selog_loglevel_warning, stderr);
	selog_set_color(selog_loglevel_warning, SELOG_COLOR_YELLOW);
	selog_set_prefix(selog_loglevel_warning, "[WARNING]");
	selog_set_time_fmt(selog_loglevel_warning, "(%H:%M:%S)");
	selog_set_flag(selog_loglevel_warning, SELOG_FLAG_ALL);
	/* Error */
	selog_set_stream(selog_loglevel_error, stderr);
	selog_set_color(selog_loglevel_error, SELOG_COLOR_RED);
	selog_set_prefix(selog_loglevel_error, "[ERROR]");
	selog_set_time_fmt(selog_loglevel_error, "(%H:%M:%S)");
	selog_set_flag(selog_loglevel_error, SELOG_FLAG_ALL);
	/* Fatal */
	selog_set_stream(selog_loglevel_fatal, stderr);
	selog_set_color(selog_loglevel_fatal, SELOG_COLOR_BOLD_RED);
	selog_set_prefix(selog_loglevel_fatal, "[FATAL]");
	selog_set_time_fmt(selog_loglevel_fatal, "(%H:%M:%S)");
	selog_set_flag(selog_loglevel_fatal, SELOG_FLAG_ALL);
}

int selog_logf(struct loglevel *l, const char *file, int line, const char *function, const char *fmt, ...)
{
	int ret;
	time_t t = time(NULL);
	struct message m;

	m.fmt = fmt;
	m.file = file;
	m.line = line;
	m.function = function;
	m.loglevel = l;
	m.time = localtime(&t);

	va_start(m.args, fmt);
	ret = log_to_output(&m);
	va_end(m.args);

	return ret;
}

int selog_vlogf(struct loglevel *l, const char *file, int line, const char *function, const char *fmt, va_list args)
{
	int ret;
	time_t t = time(NULL);
	struct message m;

	m.fmt = fmt;
	m.file = file;
	m.line = line;
	m.function = function;
	m.loglevel = l;
	m.time = localtime(&t);

	va_copy(m.args, args);
	ret = log_to_output(&m);

	return ret;
}
