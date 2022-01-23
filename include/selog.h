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
#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

struct loglevel {
	FILE *fp;
	const char *color;
	const char *prefix;
	const char *time_fmt;
	int flags;
};

/** @defgroup loglevels
 * selog's pre-defined loglevels
 * @{
 */
extern struct loglevel * const selog_loglevel_trace;
extern struct loglevel * const selog_loglevel_debug;
extern struct loglevel * const selog_loglevel_info;
extern struct loglevel * const selog_loglevel_warning;
extern struct loglevel * const selog_loglevel_error;
extern struct loglevel * const selog_loglevel_fatal;
/** @}*/


/*!
 * @brief Flags that control logging format
 */
enum flags {
	SELOG_FLAG_ENABLED = 1 << 0,
	SELOG_FLAG_TIME = 1 << 1,
	SELOG_FLAG_TIME_MODE = 1 << 2,
	SELOG_FLAG_COLOR = 1 << 3,
	SELOG_FLAG_FUNCTION = 1 << 4,
};
enum time_modes{
	SELOG_TIME_MODE_EPOCH = 0,
	SELOG_TIME_MODE_INIT = SELOG_FLAG_TIME_MODE,
};

enum {
	SELOG_FLAG_ALL = SELOG_FLAG_ENABLED
		| SELOG_FLAG_TIME
		| SELOG_TIME_MODE_EPOCH
#ifndef _WIN32
		| SELOG_FLAG_COLOR
#endif
		| SELOG_FLAG_FUNCTION
};

#ifdef __cplusplus
extern "C" {
#endif

void selog_set_stream(struct loglevel *l, FILE *stream);
void selog_set_color(struct loglevel *l, const char *color);
void selog_set_prefix(struct loglevel *l, const char *prefix);
void selog_set_time_fmt(struct loglevel *l, const char *time_fmt);

/*!
 * @brief Set flag in loglevel
 *
 * @param[in]	loglevel	Pointer to loglevel struct
 * @param[in]	flag		enum
 */
int selog_set_flag(struct loglevel *l, int flag);

/*!
 * @brief Get flag value from loglevel
 *
 * @param[in]	loglevel	Pointer to loglevel struct
 * @param[in]	flag		enum
 */
int selog_get_flag(struct loglevel *l, int flag);

/*!
 * @brief Unset flag in loglevel
 *
 * @param[in]	loglevel	Pointer to loglevel struct
 * @param[in]	flag		enum
 */
int selog_unset_flag(struct loglevel *l, int flag);

/*!
 * @brief Setup default logging parameters
 *
 */
void selog_setup_default(void);

/*!
 * @brief Log formatted message according to flags set in loglevel
 *
 * @param[in]	loglevel	Pointer to loglevel struct
 * @param[in]	file		__FILENAME__ macro
 * @param[in]	line		__LINE__ macro
 * @param[in]	function	__func__ macro
 * @param[in]	fmt		Format specification
 * @param[in]	...		Arguments for format specification
 * @returns			Number of printed characters or -1
 */
__attribute__((format(printf, 5, 6)))
int selog_logf(struct loglevel *l, const char *file, int line, const char *function, const char *fmt, ...);
/*!
 * @brief Log formatted message according to flags set in loglevel
 *
 * @param[in]	loglevel	Pointer to loglevel struct
 * @param[in]	file		__FILENAME__ macro
 * @param[in]	line		__LINE__ macro
 * @param[in]	function	__func__ macro
 * @param[in]	fmt		Format specification
 * @param[in]	args		Arguments for format specification
 * @returns			Number of printed characters or -1
 */
int selog_vlogf(struct loglevel *l, const char *file, int line, const char *function, const char *fmt, va_list args);

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


/** @defgroup logging_macros
 * Convenience macros used to log a message to pre-defined loglevel
 * @{
 */
#define log_trace(...)		selog_logf(selog_loglevel_trace, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define log_debug(...)		selog_logf(selog_loglevel_debug, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define log_info(...)		selog_logf(selog_loglevel_info, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define log_warning(...)	selog_logf(selog_loglevel_warning, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define log_error(...)		selog_logf(selog_loglevel_error, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define log_fatal(...)		selog_logf(selog_loglevel_fatal, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
/** @}*/

#endif
