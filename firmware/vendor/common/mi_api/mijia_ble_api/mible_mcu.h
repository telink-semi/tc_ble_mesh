/*
 * mible_mcu.h
 *
 *  Created on: 2020Äê10ÔÂ26ÈÕ
 *      Author: mi
 */

#ifndef MIJIA_BLE_API_MIBLE_MCU_H_
#define MIJIA_BLE_API_MIBLE_MCU_H_

#include "mi_config.h"
#include "mible_port.h"
#include "mible_type.h"
#include "third_party/pt/pt.h"

typedef enum {
    MIBLE_MCU_GET_VERSION,
    MIBLE_MCU_READ_DFUINFO,
    MIBLE_MCU_WRITE_DFUINFO,
    MIBLE_MCU_WRITE_FIRMWARE,
    MIBLE_MCU_VERIFY_FIRMWARE,
    MIBLE_MCU_UPGRADE_FIRMWARE,
    MIBLE_MCU_TRANSFER,
} mible_mcu_cmd_t;

typedef struct {
    uint32_t address;
    uint32_t length;
    void* p_data;
} mible_mcu_nvminfo_t;

PT_THREAD(mible_mcu_get_info(pt_t * pt, uint8_t *buf));

PT_THREAD(mible_mcu_read_dfuinfo(pt_t * pt, uint8_t* m_dfu_info, uint8_t len));

PT_THREAD(mible_mcu_write_dfuinfo(pt_t * pt, uint8_t* m_dfu_info, uint8_t len));

PT_THREAD(mible_mcu_upgrade_firmware(pt_t* pt));

PT_THREAD(mible_mcu_nvm_write(pt_t * pt, void * p_data, uint32_t length, uint32_t address));

PT_THREAD(mible_mcu_verify_firmware(pt_t * pt));

mible_status_t mible_mcu_cmd_send(mible_mcu_cmd_t cmd, void* arg);

mible_status_t mible_mcu_cmd_wait(mible_mcu_cmd_t cmd, void* arg);

#endif /* MIJIA_BLE_API_MIBLE_MCU_H_ */
