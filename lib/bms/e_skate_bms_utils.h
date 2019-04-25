#ifndef _E_SKATE_BMS_UTILS_H
#define _E_SKATE_BMS_UTILS_H

#include <e_skate_err.h>
#include <e_skate_bms.h>

e_skate_err_t get_data_with_response(
    
    e_skate_bms_config_t *bmsConfig,
    e_skate_uart_addr_t   dstAddr,
    e_skate_uart_reg_t    readReg,
    size_t                readRegSize,
    void                 *outValue

);

#endif /* _E_SKATE_BMS_UTILS_H */
