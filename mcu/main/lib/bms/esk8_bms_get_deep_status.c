#include <esk8_err.h>
#include <esk8_bms.h>

#include "esk8_bms_utils.h"


esk8_err_t
esk8_bms_get_deep_status(
    esk8_bms_hndl_t hndl,
    esk8_bms_deep_status_t *out_status
)
{
    esk8_uart_reg_t read_regs[] = {
        ESK8_REG_BMS_SERIAL_NUMBER,
        ESK8_REG_BMS_FW_VERSION,
        ESK8_REG_BMS_MANUFACTURE_DATE,
        ESK8_REG_BMS_FACTORY_CAPACITY,
        ESK8_REG_BMS_ACTUAL_CAPACITY,
        ESK8_REG_BMS_CAPACITY,
        ESK8_REG_BMS_CAPACITY_mAh,
        ESK8_REG_BMS_CHARGE_FULL_CYCLES,
        ESK8_REG_BMS_CHARGE_COUNT,
        ESK8_REG_BMS_HEALTH,
        ESK8_REG_BMS_CELL0_V,
        ESK8_REG_BMS_CELL1_V,
        ESK8_REG_BMS_CELL2_V,
        ESK8_REG_BMS_CELL3_V,
        ESK8_REG_BMS_CELL4_V,
        ESK8_REG_BMS_CELL5_V,
        ESK8_REG_BMS_CELL6_V,
        ESK8_REG_BMS_CELL7_V,
        ESK8_REG_BMS_CELL8_V,
        ESK8_REG_BMS_CELL9_V,
        };

    void* outRegP[] = {
        (void*) &out_status->serialNumber,
        (void*) &out_status->firmwareVersion,
        (void*) &out_status->manufactureDate,
        (void*) &out_status->factoryCapacity_mAh,
        (void*) &out_status->actualCapacity_mAh,
        (void*) &out_status->remainingCapacity_prc,
        (void*) &out_status->remainingCapacity_mAh,
        (void*) &out_status->chargeFullCycles,
        (void*) &out_status->chargeCount,
        (void*) &out_status->packHeath_prc,
        (void*) &out_status->cellVoltage_mV[0],
        (void*) &out_status->cellVoltage_mV[1],
        (void*) &out_status->cellVoltage_mV[2],
        (void*) &out_status->cellVoltage_mV[3],
        (void*) &out_status->cellVoltage_mV[4],
        (void*) &out_status->cellVoltage_mV[5],
        (void*) &out_status->cellVoltage_mV[6],
        (void*) &out_status->cellVoltage_mV[7],
        (void*) &out_status->cellVoltage_mV[8],
        (void*) &out_status->cellVoltage_mV[9],
        };

    uint16_t reg_num = sizeof(read_regs) / sizeof(read_regs[0]);

    for (int i=0; i<reg_num; i++)
    {
        size_t readSize = read_regs[i]==ESK8_REG_BMS_SERIAL_NUMBER?0x0E:0x02;

        esk8_err_t err = get_data_with_response(
            hndl, ESK8_ADDR_BMS, read_regs[i],
            readSize,
            outRegP[i]
        );

        if (err != ESK8_OK)
            return err;
    }

    return ESK8_OK;
}