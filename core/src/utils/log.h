#include <stdio.h>
#include "colors.h"

#ifdef DEBUG_LOG_ENABLED
#define LOG(fmt, ...)      fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define LOG(...)
#endif

#define LOGINFO(fmt, ...) fprintf(stderr, ANSI_COLOR_GREEN "[info] " ANSI_COLOR_RESET fmt, ##__VA_ARGS__)
#define LOGERROR(fmt, ...) fprintf(stderr, ANSI_COLOR_RED fmt ANSI_COLOR_RESET, ##__VA_ARGS__)
