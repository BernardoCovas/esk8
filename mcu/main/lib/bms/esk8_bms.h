#ifndef _ESK8_BMS_H
#define _ESK8_BMS_H

#include <esk8_config.h>
#include <esk8_uart.h>
#include <esk8_err.h>

#include <driver/uart.h>


typedef struct __attribute__((__packed__))
{
    uint16_t capacity;
    uint16_t voltage;
    int16_t  current;
    uint8_t  temperature1;
    uint8_t  temperature2;
} esk8_bms_status_t;


typedef struct __attribute__((__packed__))
{
    uint8_t  serialNumber[14];
    uint16_t firmwareVersion;
    uint16_t manufactureDate;
    uint16_t factoryCapacity_mAh;
    uint16_t actualCapacity_mAh;
    uint16_t remainingCapacity_prc; // Units in percentage.
    uint16_t remainingCapacity_mAh; // Units in mAh.
    uint16_t chargeFullCycles;
    uint16_t chargeCount;
    uint16_t packHeath_prc;
    uint16_t cellVoltage_mV[10];
    uint8_t  isCharging;
    uint8_t  isOverVoltage;
    uint8_t  isOverHeat;
} esk8_bms_deep_status_t;


typedef void (*esk8_bms_cb_t)(esk8_bms_status_t* status, uint8_t nStatus);


typedef struct
{
    uart_port_t         bmsUartPort;
    uint8_t             numBat;
    uint8_t*            batRxPins;
    uint8_t*            batTxPins;
    uint8_t             batTxPin;
    uint32_t            bmsUpdateMs;
    uart_config_t       uartConfig;
} esk8_bms_config_t;


esk8_err_t esk8_bms_init(

    esk8_bms_config_t* bmsConfig

);

esk8_err_t esk8_bms_set_rx(

    esk8_bms_config_t* eSkateUart,
    uint8_t uartRxI

);

/**
 * Same as `esk8_bms_init()`, but uses
 * values from `esk8_config.h`.
 **/
esk8_err_t esk8_bms_init_from_config_h(

    esk8_bms_config_t* outConfig

);


/**
 * Asks the active BMS for all the useful
 * registers, and updates `outStatus`.
 * This function waits for all the
 * responses.
 * Returns `ESK8_BMS_SUCCESS` on
 * success, anything else on error.
 **/
esk8_err_t esk8_bms_get_status(

    esk8_bms_config_t *bmsConfig,
    esk8_bms_status_t *outStatus

);


/**
 * Reads the pack's deep status.
 * Takes a while, and the result is stored
 * in `outDeepStatus`.
 **/
esk8_err_t esk8_bms_get_deep_status(

    esk8_bms_config_t      *bmsConfig,
    esk8_bms_deep_status_t *outStatus

);


/**
 * Sends to the current active uart
 * the messages to read all the registers
 * with battery info.
 **/
esk8_err_t esk8_bms_get_all_status(

    esk8_bms_config_t* bmsConfig

);


#endif /* _ESK8_BMS_H */
