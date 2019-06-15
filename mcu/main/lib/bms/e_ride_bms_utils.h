#ifndef _E_RIDE_BMS_UTILS_H
#define _E_RIDE_BMS_UTILS_H

#include <e_ride_err.h>
#include <e_ride_bms.h>

e_ride_err_t get_data_with_response(
    
    e_ride_bms_config_t *bmsConfig,
    e_ride_uart_addr_t   dstAddr,
    e_ride_uart_reg_t    readReg,
    size_t                readRegSize,
    void                 *outValue

);

#endif /* _E_RIDE_BMS_UTILS_H */
