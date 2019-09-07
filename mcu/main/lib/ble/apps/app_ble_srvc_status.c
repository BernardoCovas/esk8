#include <esk8_log.h>
#include <esk8_bms.h>
#include <esk8_ble_apps.h>

#include <esp_gatts_api.h>

#define SRVC_STATUS_NAME    "SRVC_STAT"
#define LOG_TAG             ESK8_TAG_BLE "(SRVC_STAT):"


static uint16_t SRVC_STATUS_UUID                            = 0xE8E0;

static uint16_t SRVC_STATUS_SPEED_UUID                      = 0xE8E1;
static uint8_t  SRVC_STATUS_SPEED_VAL[1]                    = {0};
static uint16_t SRVC_STATUS_SPEED_DESC                      = 0x0000;

static uint16_t SRVC_STATUS_BMS_SHALLOW_UUID                = 0xE8E2;
static esk8_bms_status_t SRVC_STATUS_BMS_SHALLOW_VAL[ESK8_UART_BMS_CONF_NUM]     = {0};
static uint16_t SRVC_STATUS_BMS_SHALLOW_DESC                = 0x0000;

static uint16_t SRVC_STATUS_BMS_DEEP_UUID                   = 0xE8E3;
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

static void app_init();

static void app_deinit();

static void app_conn_add(
    esk8_ble_conn_ctx_t* conn_ctx);

static void app_conn_del(
    esk8_ble_conn_ctx_t* conn_ctx);

static void app_conn_write(
    esk8_ble_conn_ctx_t* conn_ctx,
    int                  attr_idx,
    size_t               len,
    uint8_t*             val);

static void app_evt_cb(
    esp_gatts_cb_event_t event,
    esp_ble_gatts_cb_param_t *param);

esk8_ble_app_t esk8_app_srvc_status =
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

static void
app_init()
{
    printf(LOG_TAG "app_init()\n");
}

static void
app_deinit()
{
    printf(LOG_TAG  "app_deinit() \n");
}

static void
app_conn_add(
    esk8_ble_conn_ctx_t* conn_ctx
)
{
    printf(LOG_TAG  "app_conn_add() \n");
}

static void
app_conn_del(
    esk8_ble_conn_ctx_t* conn_ctx
)
{
    printf(LOG_TAG  "app_conn_del() \n");
}

static void app_conn_write(
    esk8_ble_conn_ctx_t* conn_ctx,
    int                  attr_idx,
    size_t               len,
    uint8_t*             val)
{
    printf(LOG_TAG  "app_conn_write() on idx: %d\n", attr_idx);
}

static void
app_evt_cb(
    esp_gatts_cb_event_t event,
    esp_ble_gatts_cb_param_t *param
)
{
    printf(LOG_TAG  "app_evt_cb(). Evt: %d\n", (int)event);
}