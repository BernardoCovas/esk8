#include <esk8_log.h>
#include <esk8_ble.h>
#include <esk8_bms.h>

#include <app_ble_srvc.h>

#include <esp_gatts_api.h>

#define SRVC_STATUS_NAME    "Status Service"
#define LOG_TAG         ESK8_TAG_BLE "(stat) "


       uint16_t SRVC_STATUS_UUID                = 0x5AA5;

       uint16_t SRVC_STATUS_SPEED_UUID          = 0x5A01;
static uint8_t  SRVC_STATUS_SPEED_VAL[1]        = {0};
static uint16_t SRVC_STATUS_SPEED_DESC          = 0x0000;

       uint16_t SRVC_STATUS_BMS_SHALLOW_UUID    = 0x5ABA;
static uint8_t  SRVC_STATUS_BMS_SHALLOW_VAL[sizeof(esk8_bms_status_t) * 4]    = {0};
static uint16_t SRVC_STATUS_BMS_SHALLOW_DESC    = 0x0000;

       uint16_t SRVC_STATUS_BMS_DEEP_UUID       = 0x5ABD;
static uint8_t  SRVC_STATUS_BMS_DEEP_VAL[sizeof(esk8_bms_deep_status_t) * 4]  = {0};
static uint16_t SRVC_STATUS_BMS_DEEP_DESC       = 0x0000;

static uint16_t SRVC_UUID_PRIMARY               = ESP_GATT_UUID_PRI_SERVICE;
static uint16_t CHAR_UUID_DECLARE               = ESP_GATT_UUID_CHAR_DECLARE;
static uint16_t CHAR_UUID_CONFIG                = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static uint8_t  CHAR_PROP_READ_NOTIFY           = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;


static void srvc_status_evt_cb(esk8_ble_notif_t* bleNotif);


void app_srvc_status_update_speed(uint8_t speedVal);
esk8_err_t app_srvc_status_update_bms_deep(esk8_err_t,uint16_t, esk8_bms_deep_status_t*);
esk8_err_t app_srvc_status_update_bms_shallow(esk8_err_t,uint16_t,esk8_bms_status_t*);


/**
 * Indexes for the service attributes.
 */
typedef enum
{
    SRVC_IDX_STATUS_SRVC,
    SRVC_IDX_STATUS_SPEED_CHAR,
    SRVC_IDX_STATUS_SPEED_CHAR_VAL,
    SRVC_IDX_STATUS_SPEED_DESC, /* CCCD */
    SRVC_IDX_STATUS_BMS_SHALLOW_CHAR,
    SRVC_IDX_STATUS_BMS_SHALLOW_CHAR_VAL,
    SRVC_IDX_STATUS_BMS_SHALLOW_DESC, /* CCCD */
    SRVC_IDX_STATUS_BMS_DEEP_CHAR,
    SRVC_IDX_STATUS_BMS_DEEP_CHAR_VAL,
    SRVC_IDX_STATUS_BMS_DEEP_DESC, /* CCCD */
    SRVC_STATUS_NUM_ATTR
}
esk8_app_attr_idx_t;


static uint16_t srvc_status_attr_hndl_list[SRVC_STATUS_NUM_ATTR];
static esp_gatts_attr_db_t srvc_status_attr_list[] =
{
    /**
     * Service
     */
    [SRVC_IDX_STATUS_SRVC]   = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&SRVC_UUID_PRIMARY, ESP_GATT_PERM_READ,
            sizeof(uint16_t), sizeof(uint16_t), (uint8_t*)&SRVC_STATUS_UUID
        },
    },

    /**
     * Speed
     */
    [SRVC_IDX_STATUS_SPEED_CHAR]  = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&CHAR_UUID_DECLARE, ESP_GATT_PERM_READ,
            sizeof(uint8_t), sizeof(uint8_t), &CHAR_PROP_READ_NOTIFY
        },
    },

    [SRVC_IDX_STATUS_SPEED_CHAR_VAL]  = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&SRVC_STATUS_SPEED_UUID, ESP_GATT_PERM_READ,
            sizeof(SRVC_STATUS_SPEED_VAL), sizeof(SRVC_STATUS_SPEED_VAL), SRVC_STATUS_SPEED_VAL
        },
    },

    [SRVC_IDX_STATUS_SPEED_DESC]  = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&CHAR_UUID_CONFIG, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
            sizeof(SRVC_STATUS_SPEED_DESC), sizeof(SRVC_STATUS_SPEED_DESC), (uint8_t*)&SRVC_STATUS_SPEED_DESC
        },
    },

    /**
     * BMS Shallow Status
     */
    [SRVC_IDX_STATUS_BMS_SHALLOW_CHAR]  = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&CHAR_UUID_DECLARE, ESP_GATT_PERM_READ,
            sizeof(uint8_t), sizeof(uint8_t), &CHAR_PROP_READ_NOTIFY
        },
    },

    [SRVC_IDX_STATUS_BMS_SHALLOW_CHAR_VAL]  = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&SRVC_STATUS_BMS_SHALLOW_UUID, ESP_GATT_PERM_READ,
            sizeof(SRVC_STATUS_BMS_SHALLOW_VAL), sizeof(SRVC_STATUS_BMS_SHALLOW_VAL), SRVC_STATUS_BMS_SHALLOW_VAL
        },
    },

    [SRVC_IDX_STATUS_BMS_SHALLOW_DESC]  = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&CHAR_UUID_CONFIG, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
            sizeof(SRVC_STATUS_BMS_SHALLOW_DESC), sizeof(SRVC_STATUS_BMS_SHALLOW_DESC), (uint8_t*)&SRVC_STATUS_BMS_SHALLOW_DESC
        },
    },

    /**
     * BMS Deep Status
     */
    [SRVC_IDX_STATUS_BMS_DEEP_CHAR]  = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&CHAR_UUID_DECLARE, ESP_GATT_PERM_READ,
            sizeof(uint8_t), sizeof(uint8_t), &CHAR_PROP_READ_NOTIFY
        },
    },

    [SRVC_IDX_STATUS_BMS_DEEP_CHAR_VAL]  = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&SRVC_STATUS_BMS_DEEP_UUID, ESP_GATT_PERM_READ,
            sizeof(SRVC_STATUS_BMS_DEEP_VAL), sizeof(SRVC_STATUS_BMS_DEEP_VAL), SRVC_STATUS_BMS_DEEP_VAL
        },
    },

    [SRVC_IDX_STATUS_BMS_DEEP_DESC]  = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&CHAR_UUID_CONFIG, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
            sizeof(SRVC_STATUS_BMS_DEEP_DESC), sizeof(SRVC_STATUS_BMS_DEEP_DESC), (uint8_t*)&SRVC_STATUS_BMS_DEEP_DESC
        },
    }

};


/* Declared in service header, defined here */
esk8_ble_app_t app_srvc_status =
{
    .app_serviceName = SRVC_STATUS_NAME,
    .attr_list       = srvc_status_attr_list,
    .attr_hndlList   = srvc_status_attr_hndl_list,
    .attr_numAttr    = SRVC_STATUS_NUM_ATTR,
    .app_evtFunc     = &srvc_status_evt_cb,
};


/**
 * Updates the internal speed value
 * as well as the BLE attr value
 * that is used as response to
 * the speed characteristic.
 * Also, if a connection is active
 * with notify enabled, this will
 * send a notification with the error code.
 */
void app_srvc_status_update_speed(

    uint8_t speedVal

)
{
    SRVC_STATUS_SPEED_VAL[0] = speedVal;

    esp_ble_gatts_send_indicate(
        app_srvc_status._app_appHndl,
        app_srvc_status._app_connId,
        srvc_status_attr_hndl_list[SRVC_IDX_STATUS_SPEED_CHAR_VAL],
        sizeof(SRVC_STATUS_SPEED_VAL),
        (uint8_t*)&SRVC_STATUS_SPEED_VAL,
        false
    );

    esp_ble_gatts_set_attr_value(
        srvc_status_attr_hndl_list[SRVC_IDX_STATUS_SPEED_CHAR_VAL],
        sizeof(SRVC_STATUS_SPEED_VAL),
        SRVC_STATUS_SPEED_VAL
    );

}


/**
 * Updates the internal status
 * that is used as response to
 * the shallow bms status characteristic.
 * Also, if a connection is active
 * with notify enabled, this will
 * send a notification with the error code.
 */
esk8_err_t app_srvc_status_update_bms_shallow(

    esk8_err_t            bmsErrCode,
    uint16_t                bmsIndex,
    esk8_bms_status_t*    bmsStatus

)
{
    if (bmsIndex >= ESK8_UART_BMS_CONF_NUM)
        return ESK8_ERR_INVALID_PARAM;

    esk8_bms_status_t* allStatus = (esk8_bms_status_t*)SRVC_STATUS_BMS_SHALLOW_VAL;
    esk8_bms_status_t* newStatus = bmsStatus;
    allStatus[bmsIndex] = (*newStatus);

    /**
     * After copying, we update the BLE attr
     * and notify.
     */
    const size_t bms_idx_sz = sizeof(bmsIndex);
    const size_t bms_err_sz = sizeof(bmsErrCode);
    uint8_t notif_val[bms_idx_sz + bms_err_sz];

    memcpy(notif_val,               &bmsErrCode , bms_err_sz);
    memcpy(notif_val + bms_err_sz,  &bmsIndex   , bms_idx_sz);

    esp_ble_gatts_set_attr_value(
        srvc_status_attr_hndl_list[SRVC_IDX_STATUS_BMS_SHALLOW_CHAR_VAL],
        sizeof(SRVC_STATUS_BMS_SHALLOW_VAL),
        SRVC_STATUS_BMS_SHALLOW_VAL
    );

    esp_ble_gatts_send_indicate(
        app_srvc_status._app_appHndl,
        app_srvc_status._app_connId,
        srvc_status_attr_hndl_list[SRVC_IDX_STATUS_BMS_SHALLOW_CHAR_VAL],
        bms_idx_sz + bms_err_sz,
        (uint8_t*)&notif_val,
        false
    );

    return ESK8_SUCCESS;
}


/**
 * Updates the internal status
 * that is used as response to
 * the deep bms status characteristic.
 * Also, if a connection is active
 * with notify enabled, this will
 * send a notification with the error code.
 */
esk8_err_t app_srvc_status_update_bms_deep(

    esk8_err_t                bmsErrCode,
    uint16_t                    bmsIndex,
    esk8_bms_deep_status_t*   bmsStatus

)
{
    if (bmsIndex >= ESK8_UART_BMS_CONF_NUM)
        return ESK8_ERR_INVALID_PARAM;

    /* Copies the packed struct to the value at the specified index. Alternative to memcpy. */
    esk8_bms_deep_status_t* allStatus = (esk8_bms_deep_status_t*)SRVC_STATUS_BMS_DEEP_VAL;
    allStatus[bmsIndex] = (*bmsStatus);

    /**
     * After copying, we update the BLE attr
     * and notify.
     */
    size_t bms_idx_sz = sizeof(bmsIndex);
    size_t bms_err_sz = sizeof(bmsErrCode);
    uint8_t notif_val[bms_idx_sz + bms_err_sz];

    memcpy(notif_val,               &bmsErrCode , bms_err_sz);
    memcpy(notif_val + bms_err_sz,  &bmsIndex   , bms_idx_sz);

    esp_ble_gatts_set_attr_value(
        srvc_status_attr_hndl_list[SRVC_IDX_STATUS_BMS_DEEP_CHAR_VAL],
        sizeof(SRVC_STATUS_BMS_DEEP_VAL),
        SRVC_STATUS_BMS_DEEP_VAL
    );

    esp_ble_gatts_send_indicate(
        app_srvc_status._app_appHndl,
        app_srvc_status._app_connId,
        srvc_status_attr_hndl_list[SRVC_IDX_STATUS_BMS_DEEP_CHAR_VAL],
        bms_idx_sz + bms_err_sz,
        (uint8_t*)notif_val,
        false
    );

    return ESK8_SUCCESS;
}


void srvc_status_evt_cb(esk8_ble_notif_t* bleNotif)
{
    switch (bleNotif->event)
    {
        /**
         * TODO:
         * The service event came. This means we
         * are meant to use the BMS service.
         * We can init the BMS lib.
         *
         * If this event does not come, the lib is never started.
         * This is fine, because it means it is also never read.
         */
        case ESP_GATTS_CREATE_EVT:
            break;
        case ESP_GATTS_SET_ATTR_VAL_EVT:
            /* TODO */
            break;
        default:
            printf(LOG_TAG "Got event: %d\n", bleNotif->event);
            break;
    }

}
