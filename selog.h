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

enum loglevels {
	LOG_TRACE,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	LOG_FATAL,
	LOG_ENUM_LENGTH = LOG_FATAL
};

#ifdef __cplusplus
extern "C" {
#endif

void log_set_stream(int loglevel, FILE *stream);
void log_set_color(int loglevel, const char *color);
void log_set_prefix(int loglevel, const char *prefix);
void log_set_time_fmt(int loglevel, const char *time_fmt);
void log_print_function(int loglevel, int);
void log_print_color(int loglevel, int);
void log_print_time(int loglevel, int);

void log_setup_default(void);

__attribute__((format(printf, 5, 6)))
void _log(int loglevel, const char *file, int line, const char *function, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#define LOG_COLOR_RED 		"\033[0;31m"
#define LOG_COLOR_GREEN 	"\033[0;32m"
#define LOG_COLOR_YELLOW 	"\033[0;33m"
#define LOG_COLOR_BLUE 		"\033[0;34m"
#define LOG_COLOR_PURPLE 	"\033[0;35m"
#define LOG_COLOR_CYAN 		"\033[0;36m"
#define LOG_COLOR_WHITE 	"\033[0;37m"

#define LOG_COLOR_BOLD_RED 		"\033[1;31m"
#define LOG_COLOR_BOLD_GREEN 		"\033[1;32m"
#define LOG_COLOR_BOLD_YELLOW 		"\033[1;33m"
#define LOG_COLOR_BOLD_BLUE 		"\033[1;34m"
#define LOG_COLOR_BOLD_PURPLE 		"\033[1;35m"
#define LOG_COLOR_BOLD_CYAN 		"\033[1;36m"
#define LOG_COLOR_BOLD_WHITE 		"\033[1;37m"

#define LOG_COLOR_NONE 		""
#define LOG_COLOR_RESET		"\033[0m"


#define log_trace(...)		_log(LOG_TRACE, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_debug(...)		_log(LOG_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_info(...)		_log(LOG_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_warning(...)	_log(LOG_WARNING, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_error(...)		_log(LOG_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_fatal(...)		_log(LOG_FATAL, __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif
