#include <esk8_ble.h>
#include <esk8_log.h>
#include <esk8_auth.h>
#include <app_ble_srvc.h>

#include <stdint.h>


#define SRVC_AUTH_NAME  "Auth Service"
#define LOG_TAG         ESK8_TAG_BLE "(AUTH):"


       uint16_t SRVC_AUTH_UUID              = 0xE8A0;

       uint16_t SRVC_AUTH_KEY_UUID          = 0xE8A1;
static uint8_t  SRVC_AUTH_KEY_VAL[1]        = {0};
static uint16_t SRVC_AUTH_KEY_DESC          = 0x0000;

       uint16_t SRVC_AUTH_CHANGE_UUID       = 0xE8A2;
static uint8_t  SRVC_AUTH_CHANGE_VAL[1]     = {0};
static uint16_t SRVC_AUTH_CHANGE_DESC       = 0x0000;

static uint16_t SRVC_UUID_PRIMARY           = ESP_GATT_UUID_PRI_SERVICE;
static uint16_t CHAR_UUID_DECLARE           = ESP_GATT_UUID_CHAR_DECLARE;
static uint16_t CHAR_UUID_CONFIG            = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static uint8_t  CHAR_PROP_READ_WRITE        = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;

/**
 * Indexes for the
 * control service attributes.
 */
typedef enum
{
    SRVC_IDX_AUTH_SRVC,
    SRVC_IDX_AUTH_KEY_CHAR,
    SRVC_IDX_AUTH_KEY_CHAR_VAL,
    SRVC_IDX_AUTH_KEY_DESC,         /* CCCD */
    SRVC_IDX_AUTH_CHANGE_CHAR,
    SRVC_IDX_AUTH_CHANGE_CHAR_VAL,
    SRVC_IDX_AUTH_CHANGE_DESC,      /* CCCD */

    SRVC_AUTH_NUM_ATTR
}
esk8_app_attr_idx_t;

static uint16_t srvc_auth_attr_hndl_list[SRVC_AUTH_NUM_ATTR];
static esp_gatts_attr_db_t srvc_auth_attr_list[] =
{
    /**
     * Service
     */
    [SRVC_IDX_AUTH_SRVC] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&SRVC_UUID_PRIMARY, ESP_GATT_PERM_READ,
            sizeof(uint16_t), sizeof(uint16_t), (uint8_t*)&SRVC_AUTH_UUID
        },
    },

    /**
     * Key input
     */
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
            ESP_UUID_LEN_16, (uint8_t*)&SRVC_AUTH_KEY_UUID, ESP_GATT_PERM_WRITE,
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

    /**
     * Key change input
     */
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

static void srvc_auth_evt_cb(esk8_ble_notif_t* bleNotif);
static esk8_auth_hndl_t auth_hndl = NULL;

/* Declared in service header, defined here */
esk8_ble_app_t app_srvc_auth =
{
    .app_serviceName = SRVC_AUTH_NAME,
    .attr_list       = srvc_auth_attr_list,
    .attr_hndlList   = srvc_auth_attr_hndl_list,
    .attr_numAttr    = SRVC_AUTH_NUM_ATTR,
    .app_evtFunc     = srvc_auth_evt_cb,
};

static void srvc_auth_evt_cb(esk8_ble_notif_t* bleNotif)
{
    switch (bleNotif->event)
    {
    case ESP_GATTS_CONNECT_EVT:
        // If it is initialized we ignore
        if (auth_hndl)
            break;

        ESK8_ERRCHECK_LOG(LOG_TAG, esk8_auth_init(&auth_hndl));
        break;

    case ESP_GATTS_DISCONNECT_EVT:
        if (auth_hndl)
            ESK8_ERRCHECK_LOG(LOG_TAG, esk8_auth_deinit(&auth_hndl));
        break;

    case ESP_GATTS_WRITE_EVT:
    {
        esk8_err_t err_code;
        int idx;
        err_code = esk8_ble_get_idx_from_handle(
            &app_srvc_auth,
            bleNotif->param->write.handle,
            &idx);

        if (err_code)
        {
            printf(LOG_TAG "Got err: %s on write evt, get_idx_from_hndl.\n",
                esk8_err_to_str(err_code));
            break;
        } else
        {
            printf(LOG_TAG "Write evt to idx: %d\n", idx);
        }

        switch (idx)
        {
        case SRVC_IDX_AUTH_KEY_CHAR_VAL:

            if (bleNotif->param->write.len < sizeof(esk8_auth_key_t))
            {
                printf(LOG_TAG "Wrong key len: %d\n", bleNotif->param->write.len);
                break;
            }

            printf(LOG_TAG "Wring key len: %d\n", bleNotif->param->write.len);
            err_code = esk8_auth_auth(&auth_hndl, bleNotif->param->write.value);
            printf(LOG_TAG "Auth result: %s\n", esk8_err_to_str(err_code));
            break;

        default:
            break;
        }

        break;
    }

    case ESP_GATTS_EXEC_WRITE_EVT:
        break;

    default:
        printf(LOG_TAG "Got event: %d\n", bleNotif->event);
        break;
    }
}