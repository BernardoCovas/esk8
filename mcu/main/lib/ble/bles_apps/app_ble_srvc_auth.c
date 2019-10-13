#include <esk8_ble_defs.h>
#include <esk8_ble_apps.h>
#include <esk8_log.h>
#include <esk8_auth.h>

#include <stdio.h>
#include <stdint.h>

#define SRVC_AUTH_NAME  "SRVC_AUTH"


static uint16_t         SRVC_AUTH_UUID              = ESK8_BLE_AUTH_UUID;

static uint16_t         SRVC_AUTH_KEY_UUID          = ESK8_BLE_AUTH_KEY_UUID;
static esk8_auth_key_t  SRVC_AUTH_KEY_VAL           = {0};
static uint16_t         SRVC_AUTH_KEY_DESC          = 0x0000;

static uint16_t         SRVC_AUTH_CHANGE_UUID       = ESK8_BLE_AUTH_CHANGE_UUID;
static esk8_auth_key_t  SRVC_AUTH_CHANGE_VAL        = {0};
static uint16_t         SRVC_AUTH_CHANGE_DESC       = 0x0000;

static uint16_t         SRVC_UUID_PRIMARY           = ESP_GATT_UUID_PRI_SERVICE;
static uint16_t         CHAR_UUID_DECLARE           = ESP_GATT_UUID_CHAR_DECLARE;
static uint16_t         CHAR_UUID_CONFIG            = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static uint8_t          CHAR_PROP_READ_WRITE        = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;


enum
{
    SRVC_IDX_AUTH_SRVC,
    SRVC_IDX_AUTH_KEY_CHAR,
    SRVC_IDX_AUTH_KEY_CHAR_VAL,
    SRVC_IDX_AUTH_KEY_DESC,         /* CCCD */
    SRVC_IDX_AUTH_CHANGE_CHAR,
    SRVC_IDX_AUTH_CHANGE_CHAR_VAL,
    SRVC_IDX_AUTH_CHANGE_DESC,      /* CCCD */

    SRVC_AUTH_NUM_ATTR
};


static esp_gatts_attr_db_t srvc_auth_attr_list[] =
{
    [SRVC_IDX_AUTH_SRVC] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&SRVC_UUID_PRIMARY, ESP_GATT_PERM_READ,
            sizeof(uint16_t), sizeof(uint16_t), (uint8_t*)&SRVC_AUTH_UUID
        },
    },

    [SRVC_IDX_AUTH_KEY_CHAR] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&CHAR_UUID_DECLARE, ESP_GATT_PERM_READ,
            sizeof(uint8_t), sizeof(uint8_t), &CHAR_PROP_READ_WRITE
        },
    },

    [SRVC_IDX_AUTH_KEY_CHAR_VAL] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&SRVC_AUTH_KEY_UUID, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
            sizeof(SRVC_AUTH_KEY_VAL), sizeof(SRVC_AUTH_KEY_VAL), SRVC_AUTH_KEY_VAL
        },
    },

    [SRVC_IDX_AUTH_KEY_DESC] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&CHAR_UUID_CONFIG, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
            sizeof(SRVC_AUTH_KEY_DESC), sizeof(SRVC_AUTH_KEY_DESC), (uint8_t*)&SRVC_AUTH_KEY_DESC
        },
    },

    [SRVC_IDX_AUTH_CHANGE_CHAR] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&CHAR_UUID_DECLARE, ESP_GATT_PERM_READ,
            sizeof(uint8_t), sizeof(uint8_t), &CHAR_PROP_READ_WRITE
        },
    },

    [SRVC_IDX_AUTH_CHANGE_CHAR_VAL] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&SRVC_AUTH_CHANGE_UUID, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
            sizeof(SRVC_AUTH_CHANGE_VAL), sizeof(SRVC_AUTH_CHANGE_VAL), SRVC_AUTH_CHANGE_VAL
        },
    },

    [SRVC_IDX_AUTH_CHANGE_DESC] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&CHAR_UUID_CONFIG, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
            sizeof(SRVC_AUTH_CHANGE_DESC), sizeof(SRVC_AUTH_CHANGE_DESC), (uint8_t*)&SRVC_AUTH_CHANGE_DESC
        },
    }
};

static void app_init(
    );

static void app_deinit(
    );

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

esk8_ble_app_t esk8_app_srvc_auth =
{
    .app_name = SRVC_AUTH_NAME,

    .app_init = app_init,
    .app_deinit = app_deinit,
    .app_conn_add = app_conn_add,
    .app_conn_del = app_conn_del,
    .app_conn_write = app_conn_write,
    .app_evt_cb = app_evt_cb,

    .attr_db = srvc_auth_attr_list,
    .attr_num = SRVC_AUTH_NUM_ATTR
};

static void app_init()
{
    esk8_log_D(ESK8_TAG_BLE, "app_init()\n");
}

static void app_deinit()
{
    esk8_log_D(ESK8_TAG_BLE, "app_deinit() \n");
}

static void app_conn_add(
    esk8_ble_conn_ctx_t* conn_ctx
)
{
    esk8_auth_hndl_t hndl;
    esk8_err_t err;

    err = esk8_auth_init(&hndl);
    if (err)
    {
        esk8_log_D(ESK8_TAG_BLE, "Got '%s' on auth init.\n",
            esk8_err_to_str(err));
        conn_ctx->ctx = NULL;
        return;
    }

    conn_ctx->ctx = (void*)hndl;
}

static void app_conn_del(
    esk8_ble_conn_ctx_t* conn_ctx
)
{
    esk8_log_D(ESK8_TAG_BLE, "app_conn_del() \n");
}

static void app_conn_write(
    esk8_ble_conn_ctx_t* conn_ctx,
    int                  attr_idx,
    size_t               len,
    uint8_t*             val
)
{
    esk8_log_D(ESK8_TAG_BLE, "app_conn_write() on idx: %d\n", attr_idx);
}

static void
app_evt_cb(
    esp_gatts_cb_event_t event,
    esp_ble_gatts_cb_param_t *param
)
{

}
