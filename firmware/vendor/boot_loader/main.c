/********************************************************************************************************
 * @file     main.c
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/

#include "proj/tl_common.h"
#include "proj/mcu/watchdog_i.h"
#include "vendor/common/user_config.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/sig_mesh/app_mesh.h"

#define DEBUG_PIN                (PWM_R)

// user define

// -- user define end

#define MCU_RAM_START_ADDR      0x840000    // for 825x: 0x840000, don't change.

unsigned int g_addr_load = 0;//DUAL_MODE_FW_ADDR_SIGMESH;

typedef int (*my_func_t)(void);
#define START_UP_FLAG		(0x544c4e4b)

volatile unsigned char T_DBG_CNT[8] = {0};

#if 0
static volatile u8 AAA_pause = 1;
#define DEBUG_PAUSE(s_cnt)    \
    do{\
        while (AAA_pause) {\
            reg_tmr_sta = FLD_TMR_STA_WD;/* wd_clear();*/ \
            static volatile u32 s_cnt;s_cnt++;\
        }\
        AAA_pause = 1;\
    }while(0)
#else
#define DEBUG_PAUSE(s_cnt)    
#endif

#if 0 // sha256 test code
void mbedtls_sha256( const unsigned char *input, size_t ilen,
             unsigned char output[32], int is224 );

void sha256_test_fun()
{
	#if 1
	//irq_disable();
	static u32 sha256_len ;
	flash_read_page(0x18,4,(u8*)(&sha256_len));
	static u8 sha256_out[32];
	static u32 sha256_tick0 =0;
	static u32 sha256_tick1 =0;
	static u32 sha256_tick1_inter =0;
	sha256_tick0 = clock_time();
	//static u8 A_debug_flash_data[48];
	//memcpy(A_debug_flash_data,(u8 *)0,48);
	//flash_read_page((u8*)0x00000,20,A_debug_data_flash);
	mbedtls_sha256((u8*)0x20, 256*1024, sha256_out, 0);
	sha256_tick1 = clock_time();
	sha256_tick1_inter = (sha256_tick1 - sha256_tick0)/16;
	while(1){
        gpio_toggle(DEBUG_PIN);
        WaitMs(1000);
		static volatile u32 AA_debug1=0;
		AA_debug1++;
	}
	#else
	char str_input[70]={'1','2','3','4','5','6','7','8','9','a',
						'1','2','3','4','5','6','7','8','9','a',
						'1','2','3','4','5','6','7','8','9','a',
						'1','2','3','4','5','6','7','8','9','a',
						'1','2','3','4','5','6','7','8','9','a',
						'1','2','3','4','5','6','7','8','9','a',
						'1','2','3','4','5','6','7','8','9','a'};
	static u8 sha256_out[32];
	mbedtls_sha256((u8*)str_input, sizeof(str_input), sha256_out, 0);
	#endif
}
#endif

_attribute_ram_code_ void irq_handler(void)
{
}

_attribute_ram_code_ int is_valid_fw_bootloader(u32 addr_fw)
{
    u32 startup_flag = 0;
    flash_read_page(addr_fw + 8, 4, (u8 *)&startup_flag);
    return (START_UP_FLAG == startup_flag);
}

_attribute_ram_code_ void boot_load_with_ota_check(u32 addr_load)
{
#if 1   // ota
    if(is_valid_fw_bootloader(FLASH_ADR_UPDATE_NEW_FW)){
		// Note: when it's not all ramcode cstartup,if enter here, it can't run none ramcode any more.
		// fw size max alway 256 now
        u32 fw_size_max = ((DUAL_MODE_FW_ADDR_SIGMESH == addr_load) ? DUAL_MODE_ZB_FW_SIZE_MAX_K/*FW_SIZE_MAX_K*/ : DUAL_MODE_ZB_FW_SIZE_MAX_K) *1024;
        u8 buff[256];
        flash_read_page (FLASH_ADR_UPDATE_NEW_FW, 256, buff);
        u32 fw_size = *(u32 *)(buff + 0x18);
        if(fw_size <= fw_size_max)
        {
            for (int i=0; i<fw_size; i+=256)
            {
                #if (BATT_CHECK_ENABLE)
                app_battery_power_check_and_sleep_handle(1);
                #endif
                if ((i & 0xfff) == 0)
                {
                    flash_erase_sector (addr_load + i);
                }
        
                flash_read_page (FLASH_ADR_UPDATE_NEW_FW + i, 256, buff);
                flash_write_page (addr_load + i, 256, buff);
            }
        }
        
        buff[0] = 0;
        flash_write_page (FLASH_ADR_UPDATE_NEW_FW + 8, 1, buff);   //clear OTA flag
    
        //erase the new firmware
        for (int i = 0; i < ((fw_size + 4095)/4096); i++) {
            #if (BATT_CHECK_ENABLE)
            app_battery_power_check_and_sleep_handle(1);
            #endif
            flash_erase_sector(FLASH_ADR_UPDATE_NEW_FW + i*4096);
        }
    }
#endif

    if(is_valid_fw_bootloader(addr_load)){
        u32 ramcode_size = 0;
        flash_read_page (addr_load + 0x0c, 2, (u8 *)&ramcode_size);
        ramcode_size *= 16;
        if(ramcode_size > FW_RAMCODE_SIZE_MAX){
            ramcode_size = FW_RAMCODE_SIZE_MAX; // error, should not run here
        }
        flash_read_page(addr_load, ramcode_size, (u8 *)MCU_RAM_START_ADDR); // copy ramcode
        
#if 0 // test
        u32 cnt = 100000;
        while (T_DBG_CNT[3] && (cnt--)) {
            gpio_toggle(DEBUG_PIN);
            WaitMs(100);
        }
#endif
        
        WRITE_REG8(0x602, 0x88);    // reboot from RAM
    }
}

_attribute_ram_code_ int main(void)
{
    T_DBG_CNT[0]++;

    /* cpu_clk_init */
    cpu_wakeup_init();
    clock_init(SYS_CLK_CRYSTAL);


    // gpio_set_func(DEBUG_PIN, AS_GPIO);  // default is GPIO
    gpio_set_output_en(DEBUG_PIN, 1);   //enable output
    gpio_set_input_en(DEBUG_PIN, 0);    //disable input
    gpio_write(DEBUG_PIN, 0);           //LED OFF
    //sha256_test_fun();

#if (BATT_CHECK_ENABLE)
    app_battery_power_check_and_sleep_handle(0); //battery check must do before OTA relative operation
#endif


    //WaitMs(500);
    //gpio_write(DEBUG_PIN, 0); //LED Off
    //WaitMs(500);
    //gpio_write(DEBUG_PIN, 1); //LED On
    //WaitMs(50);
    //gpio_write(DEBUG_PIN, 0); //LED Off

#if 1 //jump to selected firmware
    irq_disable();  // must, can't enter irq, because cstartup have been changed.
#if (MESH_USER_DEFINE_MODE == MESH_IRONMAN_MENLO_ENABLE)
	LOG_USER_MSG_INFO(0, 0,"[boot] Start from Bootloader", 0);
#endif

	u32 mesh_type = *(u32 *) FLASH_ADR_MESH_TYPE_FLAG;
	// don't check firmware valid here, because we should check ota valid before. 
	if((TYPE_TLK_ZIGBEE == mesh_type) || (TYPE_DUAL_MODE_ZIGBEE_RESET == mesh_type)){
	    g_addr_load = DUAL_MODE_FW_ADDR_ZIGBEE;
	}else{
	    #if 0 // no need, because zigbee will set mesh type and reboot after 6s
	    if((0xffffffff == mesh_type) && (!is_valid_fw_bootloader(DUAL_MODE_FW_ADDR_SIGMESH))){
	        g_addr_load = DUAL_MODE_FW_ADDR_ZIGBEE;
	    }else
	    #endif
	    {
	        g_addr_load = DUAL_MODE_FW_ADDR_SIGMESH;
	    }
	}
	
    T_DBG_CNT[2]++;
    boot_load_with_ota_check(g_addr_load);  // should reboot inside.
#endif

    // should not run here for ever.
    T_DBG_CNT[4]++;

    // my_func_t func = (my_func_t)(0xa000);
    // func();
    while (1) {
        gpio_toggle(DEBUG_PIN);
        WaitMs(500);
    }
}
