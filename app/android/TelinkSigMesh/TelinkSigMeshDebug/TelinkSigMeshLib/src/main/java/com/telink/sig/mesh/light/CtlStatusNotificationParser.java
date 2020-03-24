/********************************************************************************************************
 * @file     CtlStatusNotificationParser.java 
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
package com.telink.sig.mesh.light;

import com.telink.sig.mesh.TelinkApplication;
import com.telink.sig.mesh.util.TelinkLog;
import com.telink.sig.mesh.util.UnitConvert;

public final class CtlStatusNotificationParser {

    private CtlStatusNotificationParser() {
    }

    public static CtlStatusNotificationParser create() {
        return new CtlStatusNotificationParser();
    }


    public CtlInfo parse(byte[] data) {
//        if (data == null || data.length < 2) return null;
        int index = 0;
        try {
            CtlInfo ctlInfo = new CtlInfo();

            int lum = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);

            int temp = (data[index++] & 0xFF) | ((data[index] & 0xFF) << 8);

            ctlInfo.lum = TelinkApplication.getInstance().getMeshLib().lightness2Lum(lum);
            ctlInfo.temp = TelinkApplication.getInstance().getMeshLib().tempToTemp100(temp);

            TelinkLog.d("ctl parse: lum--" + ctlInfo.lum + " temp--" + ctlInfo.temp);
            return ctlInfo;

        } catch (IndexOutOfBoundsException ie) {
            ie.printStackTrace();
            return null;
        }
    }

    public final class CtlInfo {
        public int lum;
        public int temp;

    }

}
