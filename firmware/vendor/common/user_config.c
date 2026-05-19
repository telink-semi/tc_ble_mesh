/********************************************************************************************************
 * @file    user_config.c
 *
 * @brief   This is the header file for BLE SDK
 *
 * @author  BLE GROUP
 * @date    06,2020
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#include "tl_common.h"

#ifndef WIN32
#if (0 == __TLSR_RISCV_EN__)
#if BLC_PM_DEEP_RETENTION_MODE_EN
	#if PM_DEEPSLEEP_RETENTION_ENABLE
_attribute_no_retention_bss_ 
	#endif
_align_4_ u8 irq_stk[__IRQ_STACK_SIZE__] = {0};
#endif
#endif

#if PM_DEEPSLEEP_RETENTION_ENABLE
__asm__(".equ __PM_DEEPSLEEP_RETENTION_ENABLE,    1");
#else
__asm__(".equ __PM_DEEPSLEEP_RETENTION_ENABLE,    0");
#endif
__asm__(".global     __PM_DEEPSLEEP_RETENTION_ENABLE");

#if FLASH_PLUS_ENABLE // && (0 == FW_START_BY_LEGACY_BOOTLOADER_EN) // dual mode with bootloader can set to 256K.
__asm__(".equ __FLASH_512K_ENABLE,    0");
#else
__asm__(".equ __FLASH_512K_ENABLE,    1");
#endif
__asm__(".global     __FLASH_512K_ENABLE");

#if __PROJECT_BOOTLOADER__
__asm__(".equ __FW_OFFSET,      0"); // must be 0
#elif (FW_LOAD_BY_BOOTLOADER_AND_NO_OTA_EN)
    #if (FW_START_BY_BOOTLOADER_ADDR == 0x20000)
__asm__(".equ __FW_OFFSET,      0x20000");
    #endif
#elif (FW_START_BY_LEGACY_BOOTLOADER_EN)
    #if (DUAL_MODE_FW_ADDR_SIGMESH == 0x80000)
__asm__(".equ __FW_OFFSET,      0x80000");  // must be equal to DUAL_MODE_FW_ADDR_SIGMESH
    #endif
#elif (FW_START_BY_LEGACY_BOOTLOADER_EN)
    #if (FW_START_BY_BOOTLOADER_ADDR == 0x10000)
__asm__(".equ __FW_OFFSET,      0x10000");  // must be equal to FW_START_BY_BOOTLOADER_ADDR
    #endif
#else
__asm__(".equ __FW_OFFSET,      0");
#endif
__asm__(".global     __FW_OFFSET");

#if (FW_START_BY_LEGACY_BOOTLOADER_EN)
__asm__(".equ __FW_START_BY_LEGACY_BOOTLOADER_EN,  1");
#else
__asm__(".equ __FW_START_BY_LEGACY_BOOTLOADER_EN,  0");
#endif
__asm__(".global     __FW_START_BY_LEGACY_BOOTLOADER_EN");

#if __PROJECT_BOOTLOADER__
__asm__(".equ __BOOT_LOADER_EN,         1");
#else
__asm__(".equ __BOOT_LOADER_EN,         0");
#endif
__asm__(".global     __BOOT_LOADER_EN");

#if (FW_RAMCODE_SIZE_MAX == 0x4000)
__asm__(".equ __FW_RAMCODE_SIZE_MAX,    0x4000");   // must be equal to FW_RAMCODE_SIZE_MAX
#endif
__asm__(".global     __FW_RAMCODE_SIZE_MAX");

#if (MCU_RUN_SRAM_WITH_CACHE_EN || MCU_RUN_SRAM_EN)
__asm__(".equ __MCU_RUN_SRAM_EN,         1");
#else
__asm__(".equ __MCU_RUN_SRAM_EN,         0");
#endif
__asm__(".global     __MCU_RUN_SRAM_EN");

#if (__TLSR_RISCV_EN__)
#if SPEECH_ENABLE
__asm__(".equ __SPEECH_ENABLE,         1");
#else
__asm__(".equ __SPEECH_ENABLE,         0");
#endif
__asm__(".global     __SPEECH_ENABLE");
#endif
#endif

