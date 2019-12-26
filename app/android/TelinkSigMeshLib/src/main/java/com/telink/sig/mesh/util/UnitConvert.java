/********************************************************************************************************
 * @file UnitConvert.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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
package com.telink.sig.mesh.util;

import java.util.Calendar;

/**
 * Created by kee on 2017/11/28.
 */

public final class UnitConvert {
    public static int lum2level(int lum) {
        if (lum > 100) {
            lum = 100;
        }
        return (-32768 + (65535 * lum) / 100);
    }

    public static int level2lum(short level) {

        int re = (((level + 32768) * 100) / 65535);
        TelinkLog.d("lightness 2 lum: " + level + " re: " + re);
        return re;
    }

    /**
     * @param lum 0-100
     * @return 0-65535
     */
    public static int lum2lightness(int lum) {
        return 65535 * lum / 100;
    }

    /**
     * TEMP_MIN	 800
     * TEMP_MAX	 20000
     *
     * @param temp100 0-100
     * @return 800-2000
     */
    public static int temp100ToTemp(int temp100) {
        if (temp100 > 100) {
            temp100 = 100;
        }
        return (800 + ((20000 - 800) * temp100) / 100);
    }


    public static int getZoneOffset() {
        Calendar cal = Calendar.getInstance();
        // zone offset and daylight offset
        return (cal.get(Calendar.ZONE_OFFSET) + cal.get(Calendar.DST_OFFSET))/ 60 / 1000 / 15 + 64;
    }
}
