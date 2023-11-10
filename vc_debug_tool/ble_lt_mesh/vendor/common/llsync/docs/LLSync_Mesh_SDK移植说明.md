## LLSync MESH SDK接入指引

LLSync Mesh SDK基于BLE Mesh的Model层实现。移植适配前，请确保蓝牙芯片具备BLE Mesh协议栈且符合Sig Mesh标准。

### 一、移植SDK

1. 配置参数编写

   编辑`cfg/ble_qiot_mesh_cfg.h`文件，进行功能配置。

   ```c
    // 设备数据存储地址
    #define LLSYNC_MESH_RECORD_FLASH_ADDR         (0xFE000)
    #define LLSYNC_MESH_RECORD_FLASH_PAGESIZE     (4096)

    // 未配网设备广播总时长(ms)
    #define LLSYNC_MESH_UNNET_ADV_TOTAL_TIME       (10 * 60 * 1000)

    // 未配网广播时间间隔(ms)
    #define LLSYNC_MESH_UNNET_ADV_INTERVAL         (500)

    // 未配网广播单次时长(ms)
    #define LLSYNC_MESH_UNNET_ADV_DURATION         (150)

    // 超时时间内未完成配网，是否进入静默广播状态
    #define LLSYNC_MESH_SILENCE_ADV_ENABLE         (1)

    // 静默广播单次时长(ms)
    #define LLSYNC_MESH_SILENCE_ADV_DURATION       (150)

    // 静默广播时间间隔(ms)
    #define LLSYNC_MESH_SILENCE_ADV_INTERVAL       (60 * 1000)

    #define __ORDER_LITTLE_ENDIAN__ 1234
    #define __ORDER_BIG_ENDIAN__    3412
    #define __BYTE_ORDER__          __ORDER_LITTLE_ENDIAN__

    #define MESH_LOG_PRINT(...)                 printf(__VA_ARGS__)

    #define BLE_QIOT_USER_DEFINE_HEXDUMP        (0)
   ```

2. 设备接口适配
   
   `include/ble_qiot_import.h`中定义了`LLSync mesh SDK`依赖的设备`HAL`实现，需要您在自己的硬件平台上进行实现。

   ```c
   /* 定时器创建接口示例：
   typedef struct ble_esp32_timer_id_ {
       uint8_t       type;
       ble_timer_cb  handle;
       TimerHandle_t timer;
   } ble_esp32_timer_id;
   ble_timer_t llsync_mesh_timer_create(uint8_t type, ble_timer_cb timeout_handle)
   {
       ble_esp32_timer_id *p_timer = malloc(sizeof(ble_esp32_timer_id));
       if (NULL == p_timer) {
           return NULL;
       }
       p_timer->type   = type;
       p_timer->handle = timeout_handle;
       p_timer->timer  = NULL;
       return (ble_timer_t)p_timer;
   }
   */
   ble_timer_t llsync_mesh_timer_create(uint8_t type, ble_timer_cb timeout_handle);

    /* 定时器启动接口示例：
   ble_qiot_ret_status_t llsync_mesh_timer_start(ble_timer_t timer_id, uint32_t period)
   {
       ble_esp32_timer_id *p_timer = (ble_esp32_timer_id *)timer_id;
       if (NULL == p_timer->timer) {
           p_timer->timer =
               (ble_timer_t)xTimerCreate("ota_timer", period / portTICK_PERIOD_MS,
   									p_timer->type == BLE_TIMER_PERIOD_TYPE ? pdTRUE : pdFALSE, NULL, p_timer->handle);
       }
       xTimerReset(p_timer->timer, portMAX_DELAY);
       return BLE_QIOT_RS_OK;
   }
   */
   ble_qiot_ret_status_t llsync_mesh_timer_start(ble_timer_t timer_id, uint32_t period);

    /* 定时器停止接口示例：
   ble_qiot_ret_status_t llsync_timer_stop(ble_timer_t timer_id)
   {
       ble_esp32_timer_id *p_timer = (ble_esp32_timer_id *)timer_id;
       xTimerStop(p_timer->timer, portMAX_DELAY);
       return BLE_QIOT_RS_OK;
   }
   */
   ble_qiot_ret_status_t llsync_mesh_timer_stop(ble_timer_t timer_id);

    /* 获取设备信息示例:
    #define PRODUCT_ID  "9ESYQHPQF3"
    #define DEVICE_NAME "ty"
    #define SECRET_KEY  "uvGt+pro0A84Ckftsbd0XQ=="
    ble_qiot_ret_status_t llsync_mesh_dev_info_get(ble_device_info *dev_info)
    {
        char *address = (char *)esp_bt_dev_get_address();
        memcpy(dev_info->mac, address, 6);
        memcpy(dev_info->product_id, PRODUCT_ID, strlen(PRODUCT_ID));
        memcpy(dev_info->device_name, DEVICE_NAME, strlen(DEVICE_NAME));
        memcpy(dev_info->psk, SECRET_KEY, strlen(SECRET_KEY));

        return BLE_QIOT_RS_OK;
    }
    */
    ble_qiot_ret_status_t llsync_mesh_dev_info_get(ble_device_info_t *dev_info);

    /* flash操作示例:
    int llsync_mesh_flash_deal(llsync_flash_e type, uint32_t flash_addr, char *buf, uint16_t len)
    {
        int ret = 0;
        return len;
        if (LLSYNC_ADV_WRITE_FLASH == type) {
            ret = spi_flash_erase_range(flash_addr, BLE_QIOT_RECORD_FLASH_PAGESIZE);
            ret     = spi_flash_write(flash_addr, buf, len);
        } else if(LLSYNC_ADV_READ_FLASH == type){
            ret = spi_flash_read(flash_addr, buf, len);
        }

        return ret == ESP_OK ? len : ret;
    }
    */
    int llsync_mesh_flash_deal(e_llsync_flash_user type, uint32_t flash_addr, char *buf, uint16_t len);

    /* 广播操作示例:
    ble_qiot_ret_status_t llsync_mesh_adv_deal(llsync_adv_e type, uint8_t *data_buf, uint8_t data_len)
    {
        if (LLSYNC_ADV_START == type) {
            esp_err_t ret = esp_ble_gap_config_adv_data_raw(data_buf, data_len);
            if (ret) {
                ble_qiot_log_i("config adv data failed, error code = %x", ret);
            }
        } else if (LLSYNC_ADV_STOP == type){
            esp_ble_gap_stop_advertising();
        }

        return BLE_QIOT_RS_OK;
    }
    */
    ble_qiot_ret_status_t llsync_mesh_adv_deal(e_llsync_adv_user type, uint8_t *data_buf, uint8_t data_len);

    /* 扫描函数示例:
    ble_qiot_ret_status_t llsync_scan_deal(llsync_scan_e type)
    {
        if (LLSYNC_SCAN_START == type) {
            esp_ble_gap_set_scan_params(&ble_scan_params);
        } else if (LLSYNC_SCAN_STOP == type){
            esp_ble_gap_stop_scanning();
        }

        return BLE_QIOT_RS_OK;
    }
    */
    ble_qiot_ret_status_t llsync_scan_handle(e_llsync_scan_user type);
    
    /* vendor 数据发送接口：
    static void gen_vendor_data_status(uint32_t opcode, uint8_t *data, uint8_t data_len)
    {
        struct os_mbuf *msg = NET_BUF_SIMPLE(3+data_len);  //opcode长度+有效数据长度
        uint8_t *status;
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
    opcode为LLSYNC_MESH_VND_MODEL_OP_INDICATION时发往组地址
    opcode为LLSYNC_MESH_VND_MODEL_OP_STATUS时发往单地址
    */
    void llsync_mesh_vendor_data_send(uint32_t opcode, uint8_t *data, uint16_t data_len);
   ```

3. API调用
    
    include/ble_qiot_export.h中定义了LLSync mesh SDK对外提供的API。
    
    ```c
    /* mesh vendor modle数据解析接口，将收到的vendor model数据通过此接口送入SDK进行解析 */
    ble_qiot_ret_status_t llsync_mesh_recv_data_handle(uint32_t Opcode, uint8_t *data, uint16_t data_len);

    /* 扫描响应数据获取 */
    ble_qiot_ret_status_t llsync_mesh_scan_data_get(uint8_t *data, uint8_t *data_len);

    /* UUID获取接口 */
    int llsync_mesh_dev_uuid_get(uint8_t type, uint8_t *data, uint8_t data_len);

    /* Authvalue数值获取接口 */
    ble_qiot_ret_status_t llsync_mesh_auth_clac(uint8_t *random, uint8_t *auth_data);

    /* 设备数据主动上报接口 */
    void llsync_mesh_vendor_data_report(void);
    ```

4. Opcode定义

    sdk_src/include/ble_qiot_common.h中定义了相关opcode
    ```c
    #define LLSYNC_MESH_MODEL_OP_3(b0, cid)            ((((b0) << 16) | 0xc00000) | (cid))
    #define LLSYNC_MESH_VND_MODEL_OP_SET               LLSYNC_MESH_MODEL_OP_3(0x00, LLSYNC_MESH_CID_VENDOR)
    #define LLSYNC_MESH_VND_MODEL_OP_GET               LLSYNC_MESH_MODEL_OP_3(0x01, LLSYNC_MESH_CID_VENDOR)
    #define LLSYNC_MESH_VND_MODEL_OP_SET_UNACK         LLSYNC_MESH_MODEL_OP_3(0x02, LLSYNC_MESH_CID_VENDOR)
    #define LLSYNC_MESH_VND_MODEL_OP_STATUS            LLSYNC_MESH_MODEL_OP_3(0x03, LLSYNC_MESH_CID_VENDOR)
    #define LLSYNC_MESH_VND_MODEL_OP_INDICATION        LLSYNC_MESH_MODEL_OP_3(0x04, LLSYNC_MESH_CID_VENDOR)
    #define LLSYNC_MESH_VND_MODEL_OP_CONFIRMATION      LLSYNC_MESH_MODEL_OP_3(0x05, LLSYNC_MESH_CID_VENDOR)
    ```
   
### 二、注意事项
1. 在腾讯云平台进行设备创建时，注意device name为设备的mac地址；
2. 根据Authvalue计算规则，llsync_mesh_auth_clac需要调用两次，一次是Device发送Confirmation前使用Device产生的random进行Authvalue计算，一次是在收到Provisioner的Confirmation后，使用Provisioner的random进行Authvalue计算。
