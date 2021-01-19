#ifndef MEOW_ERROR_H
#define MEOW_ERROR_H

#include "meow.h"

enum meow_error_code
{
    /* connection error */
    MEOW_ERROR_SESSION_CLOSED_BY_SERVER = 1001,
    MEOW_ERROR_SESSION_CLOSED_BY_CLIENT,
};

const char *meow_strerror(int code);

#endif /* MEOW_ERROR_H */
