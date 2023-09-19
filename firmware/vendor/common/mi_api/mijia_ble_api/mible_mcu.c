/*
 * mible_mcu.c
 *
 *  Created on: 2020Äê10ÔÂ26ÈÕ
 *      Author: mi
 */

#include <string.h>
#include "mible_mcu.h"
#include "mible_log.h"

__WEAK mible_status_t mible_mcu_cmd_send(mible_mcu_cmd_t cmd, void* arg)
{
    mible_status_t ret = MI_SUCCESS;
    switch(cmd) {
    case MIBLE_MCU_GET_VERSION:
        break;
    case MIBLE_MCU_READ_DFUINFO:
        break;
    case MIBLE_MCU_WRITE_DFUINFO:
        break;
    case MIBLE_MCU_WRITE_FIRMWARE:
        break;
    case MIBLE_MCU_VERIFY_FIRMWARE:
        break;
    case MIBLE_MCU_UPGRADE_FIRMWARE:
        break;
    case MIBLE_MCU_TRANSFER:
        break;
    default:
        break;
    }
    return ret;
}

__WEAK mible_status_t mible_mcu_cmd_wait(mible_mcu_cmd_t cmd, void* arg)
{
    mible_status_t ret = MI_SUCCESS;
    switch(cmd) {
    case MIBLE_MCU_GET_VERSION:
        ret = MI_ERR_NOT_FOUND;
        break;
    case MIBLE_MCU_READ_DFUINFO:
        ret = MI_ERR_NOT_FOUND;
        break;
    case MIBLE_MCU_WRITE_DFUINFO:
        break;
    case MIBLE_MCU_WRITE_FIRMWARE:
        break;
    case MIBLE_MCU_VERIFY_FIRMWARE:
        break;
    case MIBLE_MCU_UPGRADE_FIRMWARE:
        break;
    case MIBLE_MCU_TRANSFER:
        break;
    default:
        break;
    }
    return ret;
}

PT_THREAD(mible_mcu_get_info(pt_t * pt, uint8_t *buf))
{
    mible_status_t ret;
    PT_BEGIN(pt);

    if(MI_SUCCESS != mible_mcu_cmd_send(MIBLE_MCU_GET_VERSION, NULL))
        goto EXIT;

    PT_WAIT_UNTIL(pt, MI_ERR_BUSY != (ret = mible_mcu_cmd_wait(MIBLE_MCU_GET_VERSION, (void*)buf)));

    if(ret != MI_SUCCESS){
        MI_LOG_WARNING("mible_mcu_get_info fail %d\n", ret);
        goto EXIT;
    }

    PT_END(pt);
EXIT:
    PT_EXIT(pt);
}

PT_THREAD(mible_mcu_read_dfuinfo(pt_t * pt, uint8_t* m_dfu_info, uint8_t len))
{
    mible_status_t ret;
    PT_BEGIN(pt);

    if(MI_SUCCESS != mible_mcu_cmd_send(MIBLE_MCU_READ_DFUINFO, NULL))
        goto EXIT;
    PT_WAIT_UNTIL(pt, MI_ERR_BUSY != (ret = mible_mcu_cmd_wait(MIBLE_MCU_READ_DFUINFO, (void*)m_dfu_info)));

    if(ret != MI_SUCCESS){
        MI_LOG_WARNING("mible_mcu_read_dfuinfo fail %d\n", ret);
        goto EXIT;
    }

    PT_END(pt);
EXIT:
    PT_EXIT(pt);
}

PT_THREAD(mible_mcu_write_dfuinfo(pt_t * pt, uint8_t* m_dfu_info, uint8_t len))
{
    mible_status_t ret;
    PT_BEGIN(pt);

    if(MI_SUCCESS != mible_mcu_cmd_send(MIBLE_MCU_WRITE_DFUINFO, (void*)m_dfu_info))
        goto EXIT;
    PT_WAIT_UNTIL(pt, MI_ERR_BUSY != (ret = mible_mcu_cmd_wait(MIBLE_MCU_WRITE_DFUINFO, NULL)));

    if(ret != MI_SUCCESS){
        MI_LOG_WARNING("mible_mcu_write_dfuinfo fail %d\n", ret);
        goto EXIT;
    }

    PT_END(pt);
EXIT:
    PT_EXIT(pt);
}

PT_THREAD(mible_mcu_upgrade_firmware(pt_t * pt))
{
    mible_status_t ret;
    PT_BEGIN(pt);

    if(MI_SUCCESS != mible_mcu_cmd_send(MIBLE_MCU_UPGRADE_FIRMWARE, NULL))
        goto EXIT;
    PT_WAIT_UNTIL(pt, MI_ERR_BUSY != (ret = mible_mcu_cmd_wait(MIBLE_MCU_UPGRADE_FIRMWARE, NULL)));

    if(ret != MI_SUCCESS){
        MI_LOG_WARNING("mible_mcu_upgrade_firmware fail %d\n", ret);
        goto EXIT;
    }

    PT_END(pt);
EXIT:
    PT_EXIT(pt);
}

PT_THREAD(mible_mcu_verify_firmware(pt_t * pt))
{
    mible_status_t ret;
    PT_BEGIN(pt);

    if(MI_SUCCESS != mible_mcu_cmd_send(MIBLE_MCU_VERIFY_FIRMWARE, NULL))
        goto EXIT;
    PT_WAIT_UNTIL(pt, MI_ERR_BUSY != (ret = mible_mcu_cmd_wait(MIBLE_MCU_VERIFY_FIRMWARE, NULL)));

    if(ret != MI_SUCCESS){
        MI_LOG_WARNING("mible_mcu_verify_firmware fail %d\n", ret);
        goto EXIT;
    }

    PT_END(pt);
EXIT:
    PT_EXIT(pt);
}


PT_THREAD(mible_mcu_nvm_write(pt_t * pt, void * p_data, uint32_t length, uint32_t address))
{
    mible_status_t ret;
    mible_mcu_nvminfo_t nvminfo = {
        .address = address,
        .length = length,
        .p_data = p_data
    };
    PT_BEGIN(pt);
    MI_LOG_DEBUG("mible_mcu_nvm_write addr %08x, len %d\n", nvminfo.address, nvminfo.length);
    if(MI_SUCCESS != mible_mcu_cmd_send(MIBLE_MCU_WRITE_FIRMWARE, (void*)&nvminfo))
        goto EXIT;
    PT_WAIT_UNTIL(pt, MI_ERR_BUSY != (ret = mible_mcu_cmd_wait(MIBLE_MCU_WRITE_FIRMWARE, NULL)));

    if(ret != MI_SUCCESS){
        MI_LOG_WARNING("mible_mcu_nvm_write fail %d\n", ret);
        goto EXIT;
    }

    if(MI_SUCCESS != mible_mcu_cmd_send(MIBLE_MCU_TRANSFER, (void*)&nvminfo))
        goto EXIT;
    PT_WAIT_UNTIL(pt, MI_ERR_BUSY != (ret = mible_mcu_cmd_wait(MIBLE_MCU_TRANSFER, NULL)));

    if(ret != MI_SUCCESS){
        MI_LOG_WARNING("mible_mcu_nvm_write transfer fail %d\n", ret);
        goto EXIT;
    }

    PT_END(pt);
EXIT:
    PT_EXIT(pt);
}
