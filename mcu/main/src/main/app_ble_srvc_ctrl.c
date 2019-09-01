#include <esk8_pwm.h>
#include <esk8_ble.h>
#include <esk8_log.h>

#include <app_ble_srvc.h>

#include <stdint.h>
#include <stdbool.h>


#define SRVC_CTRL_NAME  "Ctrl Service"
#define LOG_TAG         ESK8_TAG_BLE "(ctrl) "


       uint16_t SRVC_CTRL_UUID                 = 0x5AC0;

       uint16_t SRVC_CTRL_SPEED_UUID           = 0x5AC1;
static uint8_t  SRVC_CTRL_SPEED_VAL[1]         = {0};
static uint16_t SRVC_CTRL_SPEED_DESC           = 0x0000;

       uint16_t SRVC_CTRL_PWR_UUID             = 0x5AC2;
static uint8_t  SRVC_CTRL_PWR_VAL[1]           = {0};
static uint16_t SRVC_CTRL_PWR_DESC             = 0x0000;

static uint16_t SRVC_UUID_PRIMARY              = ESP_GATT_UUID_PRI_SERVICE;
static uint16_t CHAR_UUID_DECLARE              = ESP_GATT_UUID_CHAR_DECLARE;
static uint16_t CHAR_UUID_CONFIG               = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static uint8_t  CHAR_PROP_READ_WRITE           = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;

/**
 * Indexes for the
 * control service attributes.
 */
typedef enum
{
    SRVC_IDX_CTRL_SRVC,
    SRVC_IDX_CTRL_SPEED_CHAR,
    SRVC_IDX_CTRL_SPEED_CHAR_VAL,
    SRVC_IDX_CTRL_SPEED_DESC,   /* CCCD */
    SRVC_IDX_CTRL_PWR_CHAR,
    SRVC_IDX_CTRL_PWR_CHAR_VAL,
    SRVC_IDX_CTRL_PWR_DESC,     /* CCCD */

    SRVC_CTRL_NUM_ATTR
}
esk8_app_attr_idx_t;


static esk8_pwm_config_t pwm_Config = {0};
static void srvc_ctrl_evt_cb(esk8_ble_notif_t* bleNotif);

static void srvc_ctrl_update_speed(uint8_t speed);
static void srvc_ctrl_update_pwr(bool pwr);


static uint16_t srvc_ctrl_attr_hndl_list[SRVC_CTRL_NUM_ATTR];
static esp_gatts_attr_db_t srvc_ctrl_attr_list[] =
{
    /**
     * Service
     */
    [SRVC_IDX_CTRL_SRVC] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&SRVC_UUID_PRIMARY, ESP_GATT_PERM_READ,
            sizeof(uint16_t), sizeof(uint16_t), (uint8_t*)&SRVC_CTRL_UUID
        },
    },

    /**
     * Speed Control
     */
    [SRVC_IDX_CTRL_SPEED_CHAR] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&CHAR_UUID_DECLARE, ESP_GATT_PERM_READ,
            sizeof(uint8_t), sizeof(uint8_t), &CHAR_PROP_READ_WRITE
        },
    },

    [SRVC_IDX_CTRL_SPEED_CHAR_VAL] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&SRVC_CTRL_SPEED_UUID, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
            sizeof(SRVC_CTRL_SPEED_VAL), sizeof(SRVC_CTRL_SPEED_VAL), SRVC_CTRL_SPEED_VAL
        },
    },

    [SRVC_IDX_CTRL_SPEED_DESC] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&CHAR_UUID_CONFIG, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
            sizeof(SRVC_CTRL_SPEED_DESC), sizeof(SRVC_CTRL_SPEED_DESC), (uint8_t*)&SRVC_CTRL_SPEED_DESC
        },
    },

    /**
     * Power Control
     */
    [SRVC_IDX_CTRL_PWR_CHAR] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&CHAR_UUID_DECLARE, ESP_GATT_PERM_READ,
            sizeof(uint8_t), sizeof(uint8_t), &CHAR_PROP_READ_WRITE
        },
    },

    [SRVC_IDX_CTRL_PWR_CHAR_VAL] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&SRVC_CTRL_PWR_UUID, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
            sizeof(SRVC_CTRL_PWR_VAL), sizeof(SRVC_CTRL_PWR_VAL), SRVC_CTRL_PWR_VAL
        },
    },

    [SRVC_IDX_CTRL_PWR_DESC] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&CHAR_UUID_CONFIG, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
            sizeof(SRVC_CTRL_PWR_DESC), sizeof(SRVC_CTRL_PWR_DESC), (uint8_t*)&SRVC_CTRL_PWR_DESC
        },
    }
};


/* Declared in service header, defined here */
esk8_ble_app_t app_srvc_ctrl =
{
    .app_serviceName = SRVC_CTRL_NAME,
    .attr_list       = srvc_ctrl_attr_list,
    .attr_hndlList   = srvc_ctrl_attr_hndl_list,
    .attr_numAttr    = SRVC_CTRL_NUM_ATTR,
    .app_evtFunc     = &srvc_ctrl_evt_cb,
};




void srvc_ctrl_evt_cb(esk8_ble_notif_t* bleNotif)
{
    switch (bleNotif->event)
    {
        /**
         * The tab event came. We can init.
         * If this event does not come, the lib is never started.
         * This is fine, because it means it is also never read.
         */
        case ESP_GATTS_CREAT_ATTR_TAB_EVT:
            esk8_pwm_sgnl_init(&pwm_Config);
            break;

        case ESP_GATTS_WRITE_EVT:
        {
            int srvcIdx;
            esk8_err_t errCode = esk8_ble_get_idx_from_handle(
                &app_srvc_ctrl,
                bleNotif->param->write.handle,
                &srvcIdx);

            if (errCode)
                break;

            switch (srvcIdx)
            {
            case SRVC_IDX_CTRL_PWR_CHAR_VAL:
                if (bleNotif->param->write.len != 1)
                    break;
                srvc_ctrl_update_pwr((bool)bleNotif->param->write.value[0]);
                break;
            case SRVC_IDX_CTRL_SPEED_CHAR_VAL:
                if (bleNotif->param->write.len != 1)
                    break;
                srvc_ctrl_update_speed(bleNotif->param->write.value[0]);
                break;
            default:
                break;
            }
        }
            break;
        default:
            printf(LOG_TAG "Got event: %d\n", bleNotif->event);
            break;
    }
}


void srvc_ctrl_update_speed(uint8_t speed)
{
    esk8_err_t errCode = esk8_pwm_sgnl_set(&pwm_Config, speed);
    if (errCode)
        printf(LOG_TAG "Got error: %s setting speed to: 0x%02x\n", esk8_err_to_str(errCode), speed);
    else
        printf(LOG_TAG "Set speed to 0x%02x.\n", speed);

}


void srvc_ctrl_update_pwr(bool pwr)
{
    printf(LOG_TAG "Would set power to %s.\n", pwr?"ON":"OFF");
}
