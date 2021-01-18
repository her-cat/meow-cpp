#include "log.h"

char meow_debug[MEOW_DEBUG_MSG_SIZE];
char meow_trace[MEOW_TRACE_MSG_SIZE];
char meow_warn[MEOW_WARN_MSG_SIZE];
char meow_error[MEOW_ERROR_MSG_SIZE];

void meow_log_put(int level, char *cnt)
{
    const char *level_str;
    char date_str[MEOW_LOG_DATE_STRLEN];
    char log_str[MEOW_LOG_BUFFER_SIZE];
    int n;
    time_t t;
    struct tm *p;

    switch (level) {
        case MEOW_LOG_DEBUG:
            level_str = "DEBUG";
            break;
        case MEOW_LOG_NOTICE:
            level_str = "NOTICE";
            break;
        case MEOW_LOG_ERROR:
            level_str = "ERROR";
            break;
        case MEOW_LOG_WARNING:
            level_str = "WARNING";
            break;
        case MEOW_LOG_TRACE:
            level_str = "TRACE";
            break;
        default:
            level_str = "INFO";
            break;
    }

    t = time(NULL);
    p = localtime(&t);
    snprintf(date_str, MEOW_LOG_DATE_STRLEN, "%d-%02d-%02d %02d:%02d:%02d", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);

    n = snprintf(log_str, MEOW_LOG_BUFFER_SIZE, "[%s]\t%s\t%s\n", date_str, level_str, cnt);
    if (write(STDOUT_FILENO, log_str, n) < 0) {
        printf("write(log_fd, size=%d) failed. Error: %s[%d].\n", n, strerror(errno), errno);
    }
}
