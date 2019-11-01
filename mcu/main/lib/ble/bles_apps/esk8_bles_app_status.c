#include <esk8_log.h>
#include <esk8_bms.h>
#include <esk8_ble_defs.h>
#include <esk8_bles_apps.h>

#include "../esk8_bles_apps_util.h"

#include <esp_gatts_api.h>
#include <esp_bt.h>


#define SRVC_STATUS_NAME    "SRVC_STAT"
#define LOG_TAG             ESK8_TAG_BLE "(SRVC_STAT):"


static uint16_t SRVC_STATUS_UUID                            = ESK8_BLE_STATUS_UUID;

static uint16_t SRVC_STATUS_SPEED_UUID                      = ESK8_BLE_STATUS_SPEED_UUID;
static uint8_t  SRVC_STATUS_SPEED_VAL[1]                    = {0};
static uint16_t SRVC_STATUS_SPEED_DESC                      = 0x0000;

static uint16_t SRVC_STATUS_BMS_SHALLOW_UUID                = ESK8_BLE_STATUS_BMS_SHALLOW_UUID;
static esk8_bms_status_t SRVC_STATUS_BMS_SHALLOW_VAL[ESK8_UART_BMS_CONF_NUM]     = {0};
static uint16_t SRVC_STATUS_BMS_SHALLOW_DESC                = 0x0000;

static uint16_t SRVC_STATUS_BMS_DEEP_UUID                   = ESK8_BLE_STATUS_BMS_DEEP_UUID;
static esk8_bms_deep_status_t SRVC_STATUS_BMS_DEEP_VAL[ESK8_UART_BMS_CONF_NUM]   = {0};
static uint16_t SRVC_STATUS_BMS_DEEP_DESC                   = 0x0000;

static uint16_t SRVC_UUID_PRIMARY                           = ESP_GATT_UUID_PRI_SERVICE;
static uint16_t CHAR_UUID_DECLARE                           = ESP_GATT_UUID_CHAR_DECLARE;
static uint16_t CHAR_UUID_CONFIG                            = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static uint8_t  CHAR_PROP_READ_NOTIFY                       = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;


/**
 * Indexes for the service attributes.
 */
enum
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
};

static esp_gatts_attr_db_t srvc_status_attr_list[] =
{
    [SRVC_IDX_STATUS_SRVC]   = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&SRVC_UUID_PRIMARY, ESP_GATT_PERM_READ,
            sizeof(uint16_t), sizeof(uint16_t), (uint8_t*)&SRVC_STATUS_UUID
        },
    },

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
            sizeof(SRVC_STATUS_BMS_SHALLOW_VAL), sizeof(SRVC_STATUS_BMS_SHALLOW_VAL), (uint8_t*)SRVC_STATUS_BMS_SHALLOW_VAL
        },
    },

    [SRVC_IDX_STATUS_BMS_SHALLOW_DESC]  = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&CHAR_UUID_CONFIG, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
            sizeof(SRVC_STATUS_BMS_SHALLOW_DESC), sizeof(SRVC_STATUS_BMS_SHALLOW_DESC), (uint8_t*)&SRVC_STATUS_BMS_SHALLOW_DESC
        },
    },

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
            sizeof(SRVC_STATUS_BMS_DEEP_VAL), sizeof(SRVC_STATUS_BMS_DEEP_VAL), (uint8_t*)SRVC_STATUS_BMS_DEEP_VAL
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

static esk8_err_t
app_init(
);

static esk8_err_t
app_deinit(
);

static esk8_err_t
app_conn_add(
    esk8_ble_conn_ctx_t* conn_ctx
);

static esk8_err_t
app_conn_del(
    esk8_ble_conn_ctx_t* conn_ctx
);

static esk8_err_t
app_conn_write(
    esk8_ble_conn_ctx_t* conn_ctx,
    int                  attr_idx,
    size_t               len,
    uint8_t*             val
);

static esk8_err_t
app_evt_cb(
    esp_gatts_cb_event_t event,
    esp_ble_gatts_cb_param_t *param
);

esk8_bles_app_t
esk8_app_srvc_status =
{
    .app_name       = SRVC_STATUS_NAME,
    .app_init       = app_init,
    .app_deinit     = app_deinit,
    .app_conn_add   = app_conn_add,
    .app_conn_del   = app_conn_del,
    .app_conn_write = app_conn_write,
    .app_evt_cb     = app_evt_cb,

    .attr_db        = srvc_status_attr_list,
    .attr_num       = SRVC_STATUS_NUM_ATTR
};

esk8_err_t
esk8_ble_app_status_speed(
    uint8_t speed_val
)
{
    ESK8_ERRCHECK_THROW(esk8_bles_apps_update(
        &esk8_app_srvc_status,
        SRVC_IDX_STATUS_SPEED_CHAR_VAL,
        1, &speed_val));

    ESK8_ERRCHECK_THROW(esk8_bles_apps_notify_all(
        &esk8_app_srvc_status,
        SRVC_IDX_STATUS_SPEED_CHAR_VAL,
        1, &speed_val));

    return ESK8_OK;
}

esk8_err_t
esk8_ble_app_status_bms_shallow(
    esk8_bms_status_t* stat,
    esk8_err_t         bms_err_code,
    int                bms_idx
)
{
    SRVC_STATUS_BMS_SHALLOW_VAL[bms_idx] = (*stat);

    if (esk8_app_srvc_status._attr_hndl_list)
    esp_ble_gatts_set_attr_value(
        esk8_app_srvc_status._attr_hndl_list[SRVC_IDX_STATUS_BMS_SHALLOW_CHAR_VAL],
        sizeof(SRVC_STATUS_BMS_SHALLOW_VAL), (uint8_t*)SRVC_STATUS_BMS_SHALLOW_VAL
    );

    size_t msg_size = sizeof(esk8_err_t) + sizeof(int);
    uint8_t* msg = malloc(msg_size);

    if (!msg)
        return ESK8_ERR_OOM;

    *((esk8_err_t*)msg) = bms_err_code;
    *((int*)&msg[sizeof(esk8_err_t)]) = bms_idx;

    esk8_err_t err_code = esk8_bles_apps_notify_all(
        &esk8_app_srvc_status,
        SRVC_IDX_STATUS_BMS_SHALLOW_CHAR_VAL,
        msg_size, msg
        );

    free(msg);
    return err_code;
}

esk8_err_t
esk8_ble_app_status_bms_deep(
    esk8_bms_deep_status_t* stat,
    esk8_err_t              bms_err_code,
    int                     bms_idx
)
{
    SRVC_STATUS_BMS_DEEP_VAL[bms_idx] = (*stat);

    esk8_bles_apps_update(
        &esk8_app_srvc_status,
        SRVC_IDX_STATUS_BMS_DEEP_CHAR_VAL,
        sizeof(SRVC_STATUS_BMS_DEEP_VAL),
        (uint8_t*)SRVC_STATUS_BMS_DEEP_VAL
    );

    size_t msg_size = sizeof(esk8_err_t) + sizeof(int);
    uint8_t* msg = malloc(msg_size);

    if (!msg)
        return ESK8_ERR_OOM;

    *((esk8_err_t*)msg) = bms_err_code;
    *((int*)&msg[sizeof(esk8_err_t)]) = bms_idx;

    esk8_err_t err_code = esk8_bles_apps_notify_all(
        &esk8_app_srvc_status,
        SRVC_IDX_STATUS_BMS_DEEP_CHAR_VAL,
        msg_size, msg
        );

    free(msg);
    return err_code;
}

static esk8_err_t
app_init()
{
    esk8_log_D(ESK8_TAG_BLE, "app_init()\n");
    memset(SRVC_STATUS_BMS_SHALLOW_VAL, 0, sizeof(SRVC_STATUS_BMS_SHALLOW_VAL));
    memset(SRVC_STATUS_BMS_DEEP_VAL   , 0, sizeof(SRVC_STATUS_BMS_DEEP_VAL   ));
    
    return ESK8_OK;
}

static esk8_err_t
app_deinit()
{
    esk8_log_D(ESK8_TAG_BLE, "app_deinit() \n");
    return ESK8_OK;
}

static esk8_err_t
app_conn_add(
    esk8_ble_conn_ctx_t* conn_ctx
)
{
    esk8_log_D(ESK8_TAG_BLE, "app_conn_add() \n");
    
    return ESK8_OK;
}

static esk8_err_t
app_conn_del(
    esk8_ble_conn_ctx_t* conn_ctx
)
{
    esk8_log_D(ESK8_TAG_BLE, "app_conn_del() \n");
    
    return ESK8_OK;
}

static esk8_err_t
app_conn_write(
    esk8_ble_conn_ctx_t* conn_ctx,
    int                  attr_idx,
    size_t               len,
    uint8_t*             val)
{
    esk8_log_D(ESK8_TAG_BLE, "app_conn_write() on idx: %d\n", attr_idx);
    
    return ESK8_OK;
}

static esk8_err_t
app_evt_cb(
    esp_gatts_cb_event_t event,
    esp_ble_gatts_cb_param_t *param
)
{ 
    return ESK8_OK;
}
