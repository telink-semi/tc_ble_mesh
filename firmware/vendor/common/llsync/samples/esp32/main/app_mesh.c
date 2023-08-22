/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOSConfig.h"
/* BLE */
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "mesh/mesh.h"

#include "ble_qiot_utils_mesh.h"
#include "ble_qiot_common.h"
#include "ble_qiot_import.h"
#include "ble_qiot_export.h"

// #define BLE_MESH_VENDOR_MODEL_SERVER_ID (0x0000)
// #define BLE_MESH_VENDOR_MODEL_CLIENT_ID (0x0001)

static const char *tag = "mesh";
void               ble_store_config_init(void);

static uint8_t dev_uuid[16] = {0};

#define BT_DBG_ENABLED (MYNEWT_VAL(BLE_MESH_DEBUG))

/* Company ID */
#define CID_VENDOR       0x013A
#define STANDARD_TEST_ID 0x01
#define TEST_ID          0x01
static int recent_test_id = STANDARD_TEST_ID;

#define FAULT_ARR_SIZE 2

static bool has_reg_fault = true;

static struct bt_mesh_cfg_srv cfg_srv = {
    .relay  = BT_MESH_RELAY_DISABLED,
    .beacon = BT_MESH_BEACON_ENABLED,
#if MYNEWT_VAL(BLE_MESH_FRIEND)
    .frnd = BT_MESH_FRIEND_ENABLED,
#endif
#if MYNEWT_VAL(BLE_MESH_GATT_PROXY)
    .gatt_proxy = BT_MESH_GATT_PROXY_ENABLED,
#else
    .gatt_proxy = BT_MESH_GATT_PROXY_NOT_SUPPORTED,
#endif
    .default_ttl = 7,

    /* 3 transmissions with 20ms interval */
    .net_transmit     = BT_MESH_TRANSMIT(2, 20),
    .relay_retransmit = BT_MESH_TRANSMIT(2, 20),
};

struct bt_mesh_model   sg_model;
struct bt_mesh_msg_ctx sg_ctx;

static int fault_get_cur(struct bt_mesh_model *model, uint8_t *test_id, uint16_t *company_id, uint8_t *faults,
                         uint8_t *fault_count)
{
    uint8_t reg_faults[FAULT_ARR_SIZE] = {[0 ... FAULT_ARR_SIZE - 1] = 0xff};

    ESP_LOGI(tag, "fault_get_cur() has_reg_fault %u\n", has_reg_fault);

    *test_id    = recent_test_id;
    *company_id = CID_VENDOR;

    *fault_count = min(*fault_count, sizeof(reg_faults));
    memcpy(faults, reg_faults, *fault_count);

    return 0;
}

static int fault_get_reg(struct bt_mesh_model *model, uint16_t company_id, uint8_t *test_id, uint8_t *faults,
                         uint8_t *fault_count)
{
    if (company_id != CID_VENDOR) {
        return -BLE_HS_EINVAL;
    }

    ESP_LOGI(tag, "fault_get_reg() has_reg_fault %u\n", has_reg_fault);

    *test_id = recent_test_id;

    if (has_reg_fault) {
        uint8_t reg_faults[FAULT_ARR_SIZE] = {[0 ... FAULT_ARR_SIZE - 1] = 0xff};

        *fault_count = min(*fault_count, sizeof(reg_faults));
        memcpy(faults, reg_faults, *fault_count);
    } else {
        *fault_count = 0;
    }

    return 0;
}

static int fault_clear(struct bt_mesh_model *model, uint16_t company_id)
{
    if (company_id != CID_VENDOR) {
        return -BLE_HS_EINVAL;
    }

    has_reg_fault = false;

    return 0;
}

static int fault_test(struct bt_mesh_model *model, uint8_t test_id, uint16_t company_id)
{
    if (company_id != CID_VENDOR) {
        return -BLE_HS_EINVAL;
    }

    if (test_id != STANDARD_TEST_ID && test_id != TEST_ID) {
        return -BLE_HS_EINVAL;
    }

    recent_test_id = test_id;
    has_reg_fault  = true;
    bt_mesh_fault_update(bt_mesh_model_elem(model));

    return 0;
}

static const struct bt_mesh_health_srv_cb health_srv_cb = {
    .fault_get_cur = &fault_get_cur,
    .fault_get_reg = &fault_get_reg,
    .fault_clear   = &fault_clear,
    .fault_test    = &fault_test,
};

static struct bt_mesh_health_srv health_srv = {
    .cb = &health_srv_cb,
};

static struct bt_mesh_model_pub health_pub;

static void health_pub_init(void)
{
    health_pub.msg = BT_MESH_HEALTH_FAULT_MSG(0);
}

static struct bt_mesh_model_pub gen_level_pub;
static struct bt_mesh_model_pub gen_onoff_pub;
static struct bt_mesh_model_pub gen_vendor_pub;

static void vendor_model_init(void)
{
    gen_vendor_pub.msg = BT_MESH_HEALTH_FAULT_MSG(1024);
}

static uint8_t gen_on_off_state;
static int16_t gen_level_state;

static void gen_onoff_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(3);
    uint8_t        *status;

    ESP_LOGI(tag, "#mesh-onoff STATUS\n");

    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_2(0x82, 0x04));
    status  = net_buf_simple_add(msg, 1);
    *status = gen_on_off_state;

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        ESP_LOGI(tag, "#mesh-onoff STATUS: send status failed\n");
    }

    os_mbuf_free_chain(msg);
}

static void gen_onoff_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    ESP_LOGI(tag, "#mesh-onoff GET\n");

    gen_onoff_status(model, ctx);
}

static void gen_onoff_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    ESP_LOGI(tag, "#mesh-onoff SET\n");

    gen_on_off_state = buf->om_data[0];

    gen_onoff_status(model, ctx);
}

static void gen_onoff_set_unack(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    ESP_LOGI(tag, "#mesh-onoff SET-UNACK\n");

    gen_on_off_state = buf->om_data[0];
}

static const struct bt_mesh_model_op gen_onoff_op[] = {
    {BT_MESH_MODEL_OP_2(0x82, 0x01), 0, gen_onoff_get},
    {BT_MESH_MODEL_OP_2(0x82, 0x02), 2, gen_onoff_set},
    {BT_MESH_MODEL_OP_2(0x82, 0x03), 2, gen_onoff_set_unack},
    BT_MESH_MODEL_OP_END,
};

/////////////////////////////////////////////////start///////////////////////////////////////////////////////

enum {
    GEN_VENDOR_BIND_RESP,
    GEN_VENDOR_DATA_RESPONSE,
    GEN_VENDOR_DATA_SEND,
};

static void gen_vendor_data_status(uint32_t opcode, uint8_t *data, uint8_t data_len)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(3 + data_len);  // opcode长度+有效数据长度
    uint8_t        *status;

    ESP_LOGI(tag, "#mesh-vendor STATUS\n");

    bt_mesh_model_msg_init(msg, opcode);

    status = net_buf_simple_add(msg, data_len);

    memcpy(status, data, data_len);

    if (bt_mesh_model_send(&sg_model, &sg_ctx, msg, NULL, NULL)) {
        ESP_LOGI(tag, "#mesh-onoff STATUS: send status failed\n");
    }

    os_mbuf_free_chain(msg);
}

void llsync_mesh_vendor_data_send(uint32_t opcode, uint8_t *data, uint16_t data_len)
{
    gen_vendor_data_status(opcode, data, data_len);
}

static void vendor_model_publish(uint32_t opcode, uint8_t *data, uint8_t data_len)
{
    struct os_mbuf *msg = sg_model.pub->msg;
    int             err;
    uint8_t        *status;

    bt_mesh_model_msg_init(msg, opcode);

    status = net_buf_simple_add(msg, data_len);

    memcpy(status, data, data_len);

    err = bt_mesh_model_publish(&sg_model);
    if (err) {
        BT_ERR("bt_mesh_model_publish err %d", err);
    }
}

void llsync_mesh_vendor_data_publish(uint32_t opcode, uint8_t *data, uint16_t data_len)
{
    vendor_model_publish(opcode, data, data_len);
}

static void gen_vendor_data_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    uint8_t ret = 0;

    ESP_LOGI(tag, "gen_vendor_data_set data len:%d\n", buf->om_len);

    memcpy(&sg_model, model, sizeof(struct bt_mesh_model));
    memcpy(&sg_ctx, ctx, sizeof(struct bt_mesh_msg_ctx));

    llsync_mesh_recv_data_handle(LLSYNC_MESH_VND_MODEL_OP_SET, buf->om_data, buf->om_len);
}

static void gen_vendor_data_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    ;
    uint8_t  temp_buf[128] = {0};
    uint16_t data_len      = 0;

    memcpy(&sg_model, model, sizeof(struct bt_mesh_model));
    memcpy(&sg_ctx, ctx, sizeof(struct bt_mesh_msg_ctx));

    llsync_mesh_recv_data_handle(LLSYNC_MESH_VND_MODEL_OP_GET, buf->om_data, buf->om_len);

    // gen_vendor_data_status(model, ctx, GEN_VENDOR_DATA_RESPONSE, temp_buf, data_len);
}

static void gen_vendor_set_unack(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    memcpy(&sg_model, model, sizeof(struct bt_mesh_model));
    memcpy(&sg_ctx, ctx, sizeof(struct bt_mesh_msg_ctx));

    ESP_LOGI(tag, "gen_vendor_data_setunack data len:%d\n", buf->om_len);

    llsync_mesh_recv_data_handle(LLSYNC_MESH_VND_MODEL_OP_SET_UNACK, buf->om_data, buf->om_len);
    return;
}

static void gen_vendor_recv_ack(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    memcpy(&sg_model, model, sizeof(struct bt_mesh_model));
    memcpy(&sg_ctx, ctx, sizeof(struct bt_mesh_msg_ctx));

    llsync_mesh_recv_data_handle(LLSYNC_MESH_VND_MODEL_OP_CONFIRMATION, buf->om_data, buf->om_len);
}

///////////////////////////////////////////////end//////////////////////////////////////////////////////////

/**
 * @brief SIG 定义的标准模型
 *
 */
static struct bt_mesh_model root_models[] = {
    BT_MESH_MODEL_CFG_SRV(&cfg_srv),
    BT_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_SRV, gen_onoff_op, &gen_onoff_pub, NULL),
};

static const struct bt_mesh_model_op vendor_op[] = {
    {LLSYNC_MESH_VND_MODEL_OP_SET, 0, gen_vendor_data_set},
    {LLSYNC_MESH_VND_MODEL_OP_GET, 0, gen_vendor_data_get},
    {LLSYNC_MESH_VND_MODEL_OP_SET_UNACK, 0, gen_vendor_set_unack},
    {LLSYNC_MESH_VND_MODEL_OP_CONFIRMATION, 0, gen_vendor_recv_ack},
    BT_MESH_MODEL_OP_END,
};

/**
 * @brief 用户自定义模型
 *
 */
static struct bt_mesh_model vnd_models[] = {
    BT_MESH_MODEL_VND(LLSYNC_MESH_CID_VENDOR, LLSYNC_MESH_VENDOR_MODEL_SERVER_ID, vendor_op, &gen_vendor_pub, NULL),
};

/**
 * @brief 元素集合，有其他元素就接着添加
 * root model 是SIG定义的标准 model
 * vnd model 是厂商自定义 model
 */
static struct bt_mesh_elem elements[] = {
    BT_MESH_ELEM(0, root_models, vnd_models),
};

/**
 * @brief 初始化BLE MESH节点
 * BLE MESH一个node(节点)会有多个elements(元素)，每个element下面会有多个model
 * CID 公司ID
 */
static const struct bt_mesh_comp comp = {
    .cid        = CID_VENDOR,
    .elem       = elements,
    .elem_count = ARRAY_SIZE(elements),
};

static int output_number(bt_mesh_output_action_t action, uint32_t number)
{
    ESP_LOGI(tag, "----------------------OOB Number: %u\n", number);

    return 0;
}

// 入网成功回调
static void prov_complete(u16_t net_idx, u16_t addr)
{
    ESP_LOGI(tag, "---Local node provisioned, primary address 0x%04x\n", addr);
}

const u8_t static_test[20] = "6bcdabcdabcdabcD";

static const struct bt_mesh_prov prov = {
    .uuid           = dev_uuid,
    .static_val     = static_test,
    .static_val_len = 16,
    .complete       = prov_complete,
};

static void blemesh_on_reset(int reason)
{
    BLE_HS_LOG(ERROR, "Resetting state; reason=%d\n", reason);
}

uint8_t data_uuid[32] = {0};

static void blemesh_on_sync(void)
{
    int err;
    // ble_addr_t addr;

    ESP_LOGI(tag, "Bluetooth initialized\n");

    /* Use NRPA */
    // err = ble_hs_id_gen_rnd(1, &addr);
    // assert(err == 0);
    // err = ble_hs_id_set_rnd(addr.val);
    // assert(err == 0);

    err = bt_mesh_init(BLE_ADDR_PUBLIC, &prov, &comp);
    if (err) {
        ESP_LOGI(tag, "Initializing mesh failed (err %d)\n", err);
        return;
    }

    ESP_LOGI(tag, "Mesh initialized\n");

    if (IS_ENABLED(CONFIG_SETTINGS)) {
        settings_load();
    }

    if (bt_mesh_is_provisioned()) {
        ESP_LOGI(tag, "Mesh network restored from flash\n");
    }
}

void blemesh_host_task(void *param)
{
    ble_hs_cfg.reset_cb        = blemesh_on_reset;
    ble_hs_cfg.sync_cb         = blemesh_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    health_pub_init();
    vendor_model_init();
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void app_main()
{
    /* Initialize NVS — it is used to store PHY calibration data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(tag, "--------------------llsync mesh init---------------------\n");
    ESP_ERROR_CHECK(esp_nimble_hci_and_controller_init());
    nimble_port_init();

    ble_svc_gap_init();
    ble_svc_gatt_init();

    llsync_mesh_dev_uuid_get(LLSYNC_MESH_UNNET_ADV_BIT, dev_uuid, 16);

    // register Mesh Proxy Service
    bt_mesh_register_gatt();
    /* XXX Need to have template for store */
    ble_store_config_init();

    llsync_mesh_init();

    nimble_port_freertos_init(blemesh_host_task);
}
