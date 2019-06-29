#ifndef _ESK8_BMS_UTILS_H
#define _ESK8_BMS_UTILS_H

#include <esk8_err.h>
#include <esk8_bms.h>

esk8_err_t get_data_with_response(
    
    esk8_bms_config_t *bmsConfig,
    esk8_uart_addr_t   dstAddr,
    esk8_uart_reg_t    readReg,
    size_t               readRegSize,
    void                *outValue

);

#endif /* _ESK8_BMS_UTILS_H */
