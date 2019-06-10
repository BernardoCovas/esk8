#include <e_ride_ble.h>
#include <app_ble_srvc.h>

#include <esp_gatts_api.h>

#define SRVC_STATUS_NAME    "Status Service"

static uint16_t SRVC_STATUS_UUID                = 0xAA00;
static uint16_t SRVC_STATUS_SPEED_UUID          = 0xAA01;
static uint8_t  SRVC_STATUS_SPEED_VAL[]         = {255};
static uint16_t SRVC_STATUS_SPEED_DESC          = 0x0000;
static uint16_t SRVC_STATUS_BATT_UUID           = 0xAA02;
static uint8_t  SRVC_STATUS_BATT_VAL[]          = {255, 255, 0, 0, 0};
static uint16_t SRVC_STATUS_BATT_DESC           = 0x0000;

static uint16_t SRVC_UUID_PRIMARY               = ESP_GATT_UUID_PRI_SERVICE;
static uint16_t CHAR_UUID_DECLARE               = ESP_GATT_UUID_CHAR_DECLARE;
static uint16_t CHAR_UUID_CONFIG                = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static uint8_t  CHAR_PROP_READ_NOTIFY           = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;


static void srvc_status_evt_cb(e_ride_ble_notif_t* bleNotif);


void app_srvc_status_update_speed(uint8_t speedVal);
void app_srvc_status_update_battery(uint8_t* batVals, uint8_t numBat);


/**
 * Indexes for the service attributes.
 */
typedef enum
{
    SRVC_IDX_STATUS_SRVC,
    SRVC_IDX_STATUS_SPEED_CHAR,
    SRVC_IDX_STATUS_SPEED_CHAR_VAL,
    SRVC_IDX_STATUS_SPEED_DESC, /* CCCD */
    SRVC_IDX_STATUS_BATT_CHAR,
    SRVC_IDX_STATUS_BATT_CHAR_VAL,
    SRVC_IDX_STATUS_BATT_DESC, /* CCCD */
    SRVC_STATUS_NUM_ATTR
}
e_ride_app_attr_idx_t;


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
     * Battery
     */
    [SRVC_IDX_STATUS_BATT_CHAR]  = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&CHAR_UUID_DECLARE, ESP_GATT_PERM_READ,
            sizeof(uint8_t), sizeof(uint8_t), &CHAR_PROP_READ_NOTIFY
        },
    },

    [SRVC_IDX_STATUS_BATT_CHAR_VAL]  = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&SRVC_STATUS_BATT_UUID, ESP_GATT_PERM_READ,
            sizeof(uint8_t), sizeof(uint8_t), SRVC_STATUS_BATT_VAL
        },
    },

    [SRVC_IDX_STATUS_BATT_DESC]  = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t*)&CHAR_UUID_CONFIG, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
            sizeof(SRVC_STATUS_BATT_DESC), sizeof(SRVC_STATUS_BATT_DESC), (uint8_t*)&SRVC_STATUS_BATT_DESC
        },
    }
};

static uint16_t srvc_status_attr_hndl_list[SRVC_STATUS_NUM_ATTR];


e_ride_ble_app_t app_srvc_status =
{
    .app_serviceName = SRVC_STATUS_NAME,
    .attr_list       = srvc_status_attr_list,
    .attr_hndlList   = srvc_status_attr_hndl_list,
    .attr_numAttr    = SRVC_STATUS_NUM_ATTR,
    .app_evtFunc     = &srvc_status_evt_cb,
};


/**
 * Updates the internal speed value
 * that is used as response to
 * the speed characteristic.
 */
void app_srvc_status_update_speed(

    uint8_t speedVal

)
{
    esp_ble_gatts_send_indicate(
        app_srvc_status._app_appHndl,
        app_srvc_status._app_connId,
        srvc_status_attr_hndl_list[SRVC_IDX_STATUS_SPEED_CHAR_VAL],
        sizeof(speedVal),
        (uint8_t*)&speedVal,
        false
    );
}


/**
 * Updates the internal battery value
 * that is used as response to
 * the battery characteristic.
 */
void app_srvc_status_update_battery(

    uint8_t*    batVals,
    uint8_t     numBat

)
{

}


void srvc_status_evt_cb(e_ride_ble_notif_t* bleNotif)
{
    switch (bleNotif->event)
    {
        /**
         * TODO
         */
        default:
            break;
    }

    printf("[ %s ] Got event: %d\n", __func__ , bleNotif->event);
}
