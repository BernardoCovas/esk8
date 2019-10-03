#include <esk8_err.h>

#include <esk8_bms.h>
#include <esk8_bms_utils.h>

esk8_err_t esk8_bms_get_deep_status(

    esk8_bms_config_t      *bmsConfig,
    esk8_bms_deep_status_t *outStatus

)
{
    esk8_uart_reg_t readRegs[] = {
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
        (void*) &outStatus->serialNumber,
        (void*) &outStatus->firmwareVersion,
        (void*) &outStatus->manufactureDate,
        (void*) &outStatus->factoryCapacity_mAh,
        (void*) &outStatus->actualCapacity_mAh,
        (void*) &outStatus->remainingCapacity_prc,
        (void*) &outStatus->remainingCapacity_mAh,
        (void*) &outStatus->chargeFullCycles,
        (void*) &outStatus->chargeCount,
        (void*) &outStatus->packHeath_prc,
        (void*) &outStatus->cellVoltage_mV[0],
        (void*) &outStatus->cellVoltage_mV[1],
        (void*) &outStatus->cellVoltage_mV[2],
        (void*) &outStatus->cellVoltage_mV[3],
        (void*) &outStatus->cellVoltage_mV[4],
        (void*) &outStatus->cellVoltage_mV[5],
        (void*) &outStatus->cellVoltage_mV[6],
        (void*) &outStatus->cellVoltage_mV[7],
        (void*) &outStatus->cellVoltage_mV[8],
        (void*) &outStatus->cellVoltage_mV[9],
        };

    uint16_t nReg = sizeof(readRegs) / sizeof(readRegs[0]);

    for (int i=0; i<nReg; i++)
    {
        size_t readSize = readRegs[i]==ESK8_REG_BMS_SERIAL_NUMBER?0x0E:0x02;

        esk8_err_t errCode = get_data_with_response(
            bmsConfig,
            ESK8_ADDR_BMS,
            readRegs[i],
            readSize,
            outRegP[i]);

        if (errCode != ESK8_OK)
            return errCode;

    }

    return ESK8_OK;
}