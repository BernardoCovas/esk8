#include <esk8_blec_apps.h>
#include <esk8_ble_defs.h>


static esk8_err_t
app_init(
    uint16_t gattc_if
);

static esk8_err_t
app_deinit(
    uint16_t gattc_if
);

static esk8_err_t
app_conn_add(
    esk8_blec_dev_t* dev,
    uint16_t conn_id
);

static esk8_err_t
app_conn_del(
    esk8_blec_dev_t* dev,
    uint16_t conn_id
);

static esk8_err_t
app_conn_notif(
    esk8_blec_dev_t* dev,
    int attr_idx,
    size_t len,
    uint8_t* val
);

static esk8_err_t
app_conn_read(
    esk8_blec_dev_t* dev,
    int attr_idx,
    size_t len,
    uint8_t* val
);

static esk8_err_t
app_evt_cb(
    esp_gattc_cb_event_t event,
    esp_ble_gattc_cb_param_t *param
);


esp_gattc_db_elem_t db[] = {
    {
        .type               = ESK8_BLE_TYPE_AUTH,
        .properties         = ESK8_BLE_PROP_AUTH,
        .uuid.uuid.uuid16   = ESK8_BLE_UUID_AUTH,
        .uuid.len           = ESP_UUID_LEN_16
    },
    {
        .type               = ESK8_BLE_TYPE_AUTH_KEY,
        .properties         = ESK8_BLE_PROP_AUTH_KEY,
        .uuid.uuid.uuid16   = ESK8_BLE_UUID_AUTH_KEY,
        .uuid.len           = ESP_UUID_LEN_16
    },
    {
        .type               = ESK8_BLE_TYPE_AUTH_CHANGE,
        .properties         = ESK8_BLE_PROP_AUTH_CHANGE,
        .uuid.uuid.uuid16   = ESK8_BLE_UUID_AUTH_CHANGE,
        .uuid.len           = ESP_UUID_LEN_16
    },
    {
        .type               = ESK8_BLE_TYPE_STATUS,
        .properties         = ESK8_BLE_PROP_STATUS,
        .uuid.uuid.uuid16   = ESK8_BLE_UUID_STATUS,
        .uuid.len           = ESP_UUID_LEN_16
    },
    {
        .type               = ESK8_BLE_TYPE_STATUS_SPEED,
        .properties         = ESK8_BLE_PROP_STATUS_SPEED,
        .uuid.uuid.uuid16   = ESK8_BLE_UUID_STATUS_SPEED,
        .uuid.len           = ESP_UUID_LEN_16
    },
    {
        .type               = ESK8_BLE_TYPE_STATUS_BMS_SHALLOW,
        .properties         = ESK8_BLE_PROP_STATUS_BMS_SHALLOW,
        .uuid.uuid.uuid16   = ESK8_BLE_UUID_STATUS_BMS_SHALLOW,
        .uuid.len           = ESP_UUID_LEN_16
    },
    {
        .type               = ESK8_BLE_TYPE_STATUS_BMS_DEEP,
        .properties         = ESK8_BLE_PROP_STATUS_BMS_DEEP,
        .uuid.uuid.uuid16   = ESK8_BLE_UUID_STATUS_BMS_DEEP,
        .uuid.len           = ESP_UUID_LEN_16
    },
    {
        .type               = ESK8_BLE_TYPE_CTRL,
        .properties         = ESK8_BLE_PROP_CTRL,
        .uuid.uuid.uuid16   = ESK8_BLE_UUID_CTRL,
        .uuid.len           = ESP_UUID_LEN_16
    },
    {
        .type               = ESK8_BLE_TYPE_CTRL_SPEED,
        .properties         = ESK8_BLE_PROP_CTRL_SPEED,
        .uuid.uuid.uuid16   = ESK8_BLE_UUID_CTRL_SPEED,
        .uuid.len           = ESP_UUID_LEN_16
    },
    {
        .type               = ESK8_BLE_TYPE_CTRL_PWR,
        .properties         = ESK8_BLE_PROP_CTRL_PWR,
        .uuid.uuid.uuid16   = ESK8_BLE_UUID_CTRL_PWR,
        .uuid.len           = ESP_UUID_LEN_16
    }
};


esk8_blec_app_t
esk8_blec_app_board =
{
    .app_name = "blec_app_board",

    .app_init = app_init,
    .app_deinit = app_deinit,
    .app_conn_add = app_conn_add,
    .app_conn_del = app_conn_del,
    .app_conn_notif = app_conn_notif,
    .app_conn_read = app_conn_read,
    .app_evt_cb = app_evt_cb,

    .ble_elm_n = sizeof(db) / sizeof(*db),
    .ble_elm_l = db
};


static esk8_err_t
app_init(
    uint16_t gattc_if
)
{
    return ESK8_OK;
}


static esk8_err_t
app_deinit(
    uint16_t gattc_if
)
{
    return ESK8_OK;
}


static esk8_err_t
app_conn_add(
    esk8_blec_dev_t* dev,
    uint16_t conn_id
)
{
    return ESK8_OK;
}


static esk8_err_t
app_conn_del(
    esk8_blec_dev_t* dev,
    uint16_t conn_id
)
{
    return ESK8_OK;
}


static esk8_err_t
app_conn_notif(
    esk8_blec_dev_t* dev,
    int attr_idx,
    size_t len,
    uint8_t* val
)
{
    return ESK8_OK;
}


static esk8_err_t
app_conn_read(
    esk8_blec_dev_t* dev,
    int attr_idx,
    size_t len,
    uint8_t* val
)
{
    return ESK8_OK;
}


static esk8_err_t
app_evt_cb(
    esp_gattc_cb_event_t event,
    esp_ble_gattc_cb_param_t *param
)
{
    return ESK8_OK;
}
