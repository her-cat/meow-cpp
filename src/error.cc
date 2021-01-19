#include "error.h"
#include "log.h"

/* 将错误码转为错误信息 */
const char *meow_strerror(int code)
{
    switch (code) {
        case MEOW_ERROR_SESSION_CLOSED_BY_SERVER:
            return "Session closed by server";
        case MEOW_ERROR_SESSION_CLOSED_BY_CLIENT:
            return "Session closed by client";
        default:
            snprintf(meow_error, sizeof(meow_error), "Unknown error: %d", code);
            return meow_error;
    }
}
