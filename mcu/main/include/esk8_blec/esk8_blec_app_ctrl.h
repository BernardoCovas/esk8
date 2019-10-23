#ifndef _ESK8_BLEC_APP_CTRL_H
#define _ESK8_BLEC_APP_CTRL_H

#include <esk8_err.h>
#include <esk8_blec_apps.h>

#include <stdint.h>


/**
 * Increment speed by `incr`.
 */
esk8_err_t
esk8_blec_app_ctrl_incr(
    uint8_t incr
);

esk8_err_t
esk8_blec_app_ctrl_pwr(
    int prw
);

#endif /* _ESK8_BLEC_APP_CTRL_H */
