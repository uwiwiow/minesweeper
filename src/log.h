#ifndef MINESWEEPER_LOG_H
#define MINESWEEPER_LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <raylib.h>

void setTraceLogLevel(int logType);

void log_info(const char *format, ...);
#define info(FORMAT, ...) log_info(FORMAT, ##__VA_ARGS__);

void log_error(int errorCondition, const char *format, ...);
#define error(CONDITION, FORMAT, ...) log_error(CONDITION, FORMAT, ##__VA_ARGS__);

#endif //MINESWEEPER_LOG_H
