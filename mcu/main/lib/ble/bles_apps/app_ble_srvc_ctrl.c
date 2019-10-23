#include <esk8_log.h>
#include <esk8_bles_apps.h>
#include <esk8_ble_defs.h>
#include <esk8_onboard.h>

#include <stdint.h>
#include <stdbool.h>

#define SRVC_CTRL_NAME  "SRVC_CTRL"


static uint16_t SRVC_CTRL_UUID                 = ESK8_BLE_CTRL_UUID;

static uint16_t SRVC_CTRL_SPEED_UUID           = ESK8_BLE_CTRL_SPEED_UUID;
static uint8_t  SRVC_CTRL_SPEED_VAL[1]         = {0};
static uint16_t SRVC_CTRL_SPEED_DESC           = 0x0000;

static uint16_t SRVC_CTRL_PWR_UUID             = ESK8_BLE_CTRL_PWR_UUID;
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
enum
{
    SRVC_IDX_CTRL_SRVC,
    SRVC_IDX_CTRL_SPEED_CHAR,
    SRVC_IDX_CTRL_SPEED_CHAR_VAL,
    SRVC_IDX_CTRL_SPEED_DESC,   /* CCCD */
    SRVC_IDX_CTRL_PWR_CHAR,
    SRVC_IDX_CTRL_PWR_CHAR_VAL,
    SRVC_IDX_CTRL_PWR_DESC,     /* CCCD */

    SRVC_CTRL_NUM_ATTR
};


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

static int conn_id = -1;

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

esk8_blec_app_t esk8_app_srvc_ctrl =
{
    .app_name = SRVC_CTRL_NAME,

    .app_init   = app_init,
    .app_deinit = app_deinit,
    .app_conn_add = app_conn_add,
    .app_conn_del = app_conn_del,
    .app_conn_write = app_conn_write,
    .app_evt_cb = app_evt_cb,

    .attr_db = srvc_ctrl_attr_list,
    .attr_num = SRVC_CTRL_NUM_ATTR
};

static esk8_err_t
app_init(
)
{
    esk8_log_D(ESK8_TAG_BLE, "app_init()\n");
    conn_id = -1;
}

static esk8_err_t
app_deinit(
)
{
    esk8_log_D(ESK8_TAG_BLE, "app_deinit()\n");
}

static esk8_err_t
app_conn_add(
    esk8_ble_conn_ctx_t* conn_ctx
)
{
    if (conn_id > 0)
        return;

    conn_id = conn_ctx->conn_id;
    esk8_log_D(ESK8_TAG_BLE,
        "Registering conn id %d as controller.\n",
        conn_id
    );
}

static esk8_err_t
app_conn_del(
    esk8_ble_conn_ctx_t* conn_ctx
)
{
    if (conn_ctx->conn_id != conn_id)
        return;

    conn_id = -1;

    esk8_log_D(ESK8_TAG_BLE, "Controller disconnected. Resetting speed.\n");
    esk8_onboard_set_speed(0);
}

static esk8_err_t
app_conn_write(
    esk8_ble_conn_ctx_t* conn_ctx,
    int                  attr_idx,
    size_t               len,
    uint8_t*             val)
{
    esk8_err_t err;


    switch (attr_idx)
    {
    case SRVC_IDX_CTRL_SPEED_CHAR_VAL:
        if (len != 1)
            return;

        if (conn_id != conn_ctx->conn_id)
        {
            esk8_log_D(ESK8_TAG_BLE, 
                "Connection id %d tried to write."
                "Write allowed to %d only.\n",
                conn_ctx->conn_id,
                conn_id
            );

            break;
        }

        err = esk8_onboard_set_speed(*val);
        esk8_log_D(ESK8_TAG_BLE,
            "Got '%s' updating ble status to speed: %d\n",
            esk8_err_to_str(err), val[0]
        );

        break;

    default:
        break;
    }
}

static esk8_err_t
app_evt_cb(
    esp_gatts_cb_event_t event,
    esp_ble_gatts_cb_param_t *param
)
{
}