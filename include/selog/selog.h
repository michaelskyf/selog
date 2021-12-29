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
#ifndef SELOG_H
#define SELOG_H

#include <stdio.h>
#include <stdarg.h>

enum loglevels {
	SELOG_TRACE,
	SELOG_DEBUG,
	SELOG_INFO,
	SELOG_WARNING,
	SELOG_ERROR,
	SELOG_FATAL,
	SELOG_ENUM_LENGTH = SELOG_FATAL
};

enum time_relations {
	SELOG_TIME_EPOCH,
	SELOG_TIME_INIT
};

#ifdef __cplusplus
extern "C" {
#endif

void selog_set_stream(int loglevel, FILE *stream);
void selog_set_color(int loglevel, const char *color);
void selog_set_prefix(int loglevel, const char *prefix);
void selog_set_time_fmt(int loglevel, const char *time_fmt);
void selog_set_time_relation(int loglevel, int);
void selog_print_enable(int loglevel, int);
void selog_print_function(int loglevel, int);
void selog_print_color(int loglevel, int);
void selog_print_time(int loglevel, int);

void selog_setup_default(void);

__attribute__((format(printf, 5, 6)))
void selog_printf(int loglevel, const char *file, int line, const char *function, const char *fmt, ...);
void selog_vprintf(int loglevel, const char *file, int line, const char *function, const char *fmt, va_list args);

#ifdef __cplusplus
}
#endif

#define SELOG_COLOR_RED	 		"\033[0;31m"
#define SELOG_COLOR_GREEN 		"\033[0;32m"
#define SELOG_COLOR_YELLOW 		"\033[0;33m"
#define SELOG_COLOR_BLUE	 	"\033[0;34m"
#define SELOG_COLOR_PURPLE	 	"\033[0;35m"
#define SELOG_COLOR_CYAN 		"\033[0;36m"
#define SELOG_COLOR_WHITE 		"\033[0;37m"

#define SELOG_COLOR_BOLD_RED 		"\033[1;31m"
#define SELOG_COLOR_BOLD_GREEN 		"\033[1;32m"
#define SELOG_COLOR_BOLD_YELLOW 	"\033[1;33m"
#define SELOG_COLOR_BOLD_BLUE 		"\033[1;34m"
#define SELOG_COLOR_BOLD_PURPLE 	"\033[1;35m"
#define SELOG_COLOR_BOLD_CYAN 		"\033[1;36m"
#define SELOG_COLOR_BOLD_WHITE 		"\033[1;37m"

#define SELOG_COLOR_RESET		"\033[0m"


#define log_trace(...)		selog_printf(SELOG_TRACE, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_debug(...)		selog_printf(SELOG_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_info(...)		selog_printf(SELOG_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_warning(...)	selog_printf(SELOG_WARNING, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_error(...)		selog_printf(SELOG_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_fatal(...)		selog_printf(SELOG_FATAL, __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif
