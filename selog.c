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
#include "selog.h"
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

static struct loglevel loglevels[SELOG_ENUM_LENGTH + 1];
static time_t init_time;
#ifdef _WIN32
static CRITICAL_SECTION mutex;
#else
static pthread_mutex_t mutex;
#endif

/* Declarations */
static inline int _internal_selog_get_flag_value(struct loglevel *l, int flag);
static void lock(void);
static void unlock(void);
static int log_to_output(struct message *m);

/* Definitions */
static inline int _internal_selog_get_flag_value(struct loglevel *l, int flag)
{
	return (l->flags & flag);
}

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

	if(!_internal_selog_get_flag_value(l, SELOG_FLAG_ENABLED))
		return 0;

	if(_internal_selog_get_flag_value(l, SELOG_FLAG_COLOR))
	{
		color = l->color;
		color_reset = SELOG_COLOR_RESET;
	} else {
		color = "";
		color_reset = "";
	}

	if(_internal_selog_get_flag_value(l, SELOG_FLAG_TIME))
		time_buff[strftime(time_buff, sizeof(time_buff) - 1, l->time_fmt, m->time)] = '\0';
	else
		time_buff[0] = '\0';

	/* Print message prefix */
	if(_internal_selog_get_flag_value(l, SELOG_FLAG_FUNCTION))
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

void selog_set_stream(int loglevel, FILE *stream)
{
	struct loglevel *l;

	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	l = &loglevels[loglevel];
	lock();
	l->fp = stream;
	unlock();
}

void selog_set_color(int loglevel, const char *color)
{
	struct loglevel *l;

	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	l = &loglevels[loglevel];
	lock();
	l->color = color;
	unlock();
}

void selog_set_prefix(int loglevel, const char *prefix)
{
	struct loglevel *l;

	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	l = &loglevels[loglevel];
	lock();
	l->prefix = prefix;
	unlock();
}

void selog_set_time_fmt(int loglevel, const char *time_fmt)
{
	struct loglevel *l;

	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	l = &loglevels[loglevel];
	lock();
	l->time_fmt = time_fmt;
	unlock();
}

int selog_set_flag(int loglevel, int flag)
{
	struct loglevel *l;
	int ret;

	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	l = &loglevels[loglevel];
	lock();
	ret = (l->flags |= flag);
	unlock();
	return ret;
}

int selog_unset_flag(int loglevel, int flag)
{
	struct loglevel *l;
	int ret;

	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	l = &loglevels[loglevel];
	lock();
	ret = (l->flags &= ~flag);
	unlock();
	return ret;
}

int selog_get_flag(int loglevel, int flag)
{
	struct loglevel *l;

	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	l = &loglevels[loglevel];
	return _internal_selog_get_flag_value(l, flag);
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
	selog_set_stream(SELOG_TRACE, stdout);
	selog_set_color(SELOG_TRACE, SELOG_COLOR_RESET);
	selog_set_prefix(SELOG_TRACE, "[TRACE]");
	selog_set_time_fmt(SELOG_TRACE, "(%H:%M:%S)");
	selog_set_flag(SELOG_TRACE, SELOG_FLAG_ALL);
	selog_unset_flag(SELOG_INFO, SELOG_FLAG_FUNCTION);
	selog_unset_flag(SELOG_INFO, SELOG_FLAG_COLOR);
	/* Debug */
	selog_set_stream(SELOG_DEBUG, stdout);
	selog_set_color(SELOG_DEBUG, SELOG_COLOR_GREEN);
	selog_set_prefix(SELOG_DEBUG, "[DEBUG]");
	selog_set_time_fmt(SELOG_DEBUG, "(%H:%M:%S)");
	selog_set_flag(SELOG_DEBUG, SELOG_FLAG_ALL);
	/* Info */
	selog_set_stream(SELOG_INFO, stdout);
	selog_set_color(SELOG_INFO, SELOG_COLOR_BLUE);
	selog_set_prefix(SELOG_INFO, "[INFO]");
	selog_set_time_fmt(SELOG_INFO, "(%H:%M:%S)");
	selog_set_flag(SELOG_INFO, SELOG_FLAG_ALL);
	selog_unset_flag(SELOG_INFO, SELOG_FLAG_FUNCTION);
	/* Warning */
	selog_set_stream(SELOG_WARNING, stderr);
	selog_set_color(SELOG_WARNING, SELOG_COLOR_YELLOW);
	selog_set_prefix(SELOG_WARNING, "[WARNING]");
	selog_set_time_fmt(SELOG_WARNING, "(%H:%M:%S)");
	selog_set_flag(SELOG_WARNING, SELOG_FLAG_ALL);
	/* Error */
	selog_set_stream(SELOG_ERROR, stderr);
	selog_set_color(SELOG_ERROR, SELOG_COLOR_RED);
	selog_set_prefix(SELOG_ERROR, "[ERROR]");
	selog_set_time_fmt(SELOG_ERROR, "(%H:%M:%S)");
	selog_set_flag(SELOG_ERROR, SELOG_FLAG_ALL);
	/* Fatal */
	selog_set_stream(SELOG_FATAL, stderr);
	selog_set_color(SELOG_FATAL, SELOG_COLOR_BOLD_RED);
	selog_set_prefix(SELOG_FATAL, "[FATAL]");
	selog_set_time_fmt(SELOG_FATAL, "(%H:%M:%S)");
	selog_set_flag(SELOG_FATAL, SELOG_FLAG_ALL);
}

int selog_logf(int loglevel, const char *file, int line, const char *function, const char *fmt, ...)
{
	int ret;
	time_t t = time(NULL);
	struct message m;
	struct loglevel *l;

	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	l = &loglevels[loglevel];

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

int selog_vlogf(int loglevel, const char *file, int line, const char *function, const char *fmt, va_list args)
{
	int ret;
	time_t t = time(NULL);
	struct message m;
	struct loglevel *l;

	assert(loglevel >= 0 && loglevel <= SELOG_ENUM_LENGTH);

	l = &loglevels[loglevel];

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
