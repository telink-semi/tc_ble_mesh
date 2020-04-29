/* Includes ------------------------------------------------------------------*/

//#include <string.h>
#include "mible_api.h"
#include "mible_log.h"
#include "mible_dfu_flash.h"

/* Private variables ---------------------------------------------------------*/

static uint32_t dfu_flash_start, dfu_flash_size;

/* Exported functions --------------------------------------------------------*/

mible_status_t mible_dfu_flash_init(uint32_t flash_start, uint32_t flash_size)
{
    mible_status_t ret;

    dfu_flash_start = flash_start;
    dfu_flash_size = flash_size;

    ret = mible_nvm_init();
    MI_ERR_CHECK(ret);

    return ret;
}

mible_status_t mible_dfu_flash_read(void * buffer, uint32_t length, uint32_t offset)
{
    if (offset + length > dfu_flash_size) {
        MI_LOG_ERROR("DFU flash read out of range.\n");
        return MI_ERR_INVALID_PARAM;
    }

    return mible_nvm_read(buffer, length, dfu_flash_start + offset);
}

mible_status_t mible_dfu_flash_write(void * buffer, uint32_t length, uint32_t offset)
{
    if (offset + length > dfu_flash_size) {
        MI_LOG_ERROR("DFU flash write out of range.\n");
        return MI_ERR_INVALID_PARAM;
    }

    return mible_nvm_write(buffer, length, dfu_flash_start + offset);
}


