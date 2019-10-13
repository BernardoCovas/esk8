#ifndef _ESK8_BMS_UTILS_H
#define _ESK8_BMS_UTILS_H

#include <esk8_err.h>
#include <esk8_bms.h>

esk8_err_t
get_data_with_response(
    esk8_bms_hndl_t  hndl,
    esk8_uart_addr_t dst_addr,
    esk8_uart_reg_t  reg,
    size_t           reg_size,
    void             *out_val
);

#endif /* _ESK8_BMS_UTILS_H */
