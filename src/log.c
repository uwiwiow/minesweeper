#include "log.h"

static int logTypeLevel = LOG_INFO;

void setTraceLogLevel(int logType) {
    logTypeLevel = logType;
}

void log_info(const char *format, ...) {
    if (LOG_INFO >= logTypeLevel) {
        va_list args;
        va_start(args, format);
        fprintf(stdout, "\n%s -> %s():%i \n\t", __FILE_NAME__, __FUNCTION__, __LINE__);
        vfprintf(stdout, format, args);
        va_end(args);
    }
}

void log_error(int errorCondition, const char *format, ...) {
    if (LOG_ERROR >= logTypeLevel) {
        if (errorCondition) {
            va_list args;
            va_start(args, format);
            fprintf(stderr, "\n%s -> %s():%i -> Error(%i):\n\t%s\n\t",
            __FILE_NAME__, __FUNCTION__, __LINE__, errno, strerror(errno));
            vfprintf(stderr, format, args);
            va_end(args);
            exit(EXIT_FAILURE);
        }
    }
}
