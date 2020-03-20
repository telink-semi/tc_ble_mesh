/********************************************************************************************************
 * @file ProvisionDataGenerator.java
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
package com.telink.sig.mesh.light;

import com.telink.sig.mesh.util.Arrays;
import com.telink.sig.mesh.util.TelinkLog;

/**
 * Created by kee on 2017/8/30.
 */

public class ProvisionDataGenerator {

    /**
     * u8  net_work_key[16];
     * u8  key_index[2];
     * u8  ivi_update_flag;
     * u8  iv_index[4];
     * u16  unicast_address;
     */

    /*public static byte[] getProvisionData(byte[] netKey, int adr) {
        return getProvisionData(netKey, KEY_INDEX, IV_UPDATE_FLAG, IV_INDEX, adr);
        *//*byte[] pvData = new byte[25];
        System.arraycopy(netKey, 0, pvData, 0, 16);

        System.arraycopy(KEY_INDEX, 0, pvData, 16, KEY_INDEX.length);
        System.arraycopy(IV_UPDATE_FLAG, 0, pvData, 18, IV_UPDATE_FLAG.length);

//        Integer.toHexString()

        System.arraycopy(IV_INDEX, 0, pvData, 19, IV_INDEX.length);

        pvData[23] = (byte) (adr & 0xFF);
        pvData[24] = (byte) ((adr >> 8) & 0xFF);

        TelinkLog.d("provData:" + Arrays.bytesToHexString(pvData, ":")
                + " address:" + Integer.toHexString(adr));*//*

//        return pvData;
    }*/

    public static byte[] getProvisionData(byte[] netKey, int keyIndex, byte ivUpdateFlag, int ivIndex, int adr) {
        byte[] pvData = new byte[25];
        System.arraycopy(netKey, 0, pvData, 0, 16);

        // key index
        pvData[16] = (byte) (keyIndex & 0xFF);
        pvData[17] = (byte) ((keyIndex >> 8) & 0xFF);
        // iv update flag
        pvData[18] = ivUpdateFlag;

        pvData[19] = (byte) ((ivIndex >> 24) & 0xFF);
        pvData[20] = (byte) ((ivIndex >> 16) & 0xFF);
        pvData[21] = (byte) ((ivIndex >> 8) & 0xFF);
        pvData[22] = (byte) ((ivIndex) & 0xFF);


        pvData[23] = (byte) (adr & 0xFF);
        pvData[24] = (byte) ((adr >> 8) & 0xFF);

        return pvData;
    }

    private byte[] provData = new byte[]{

            // key
            (byte) 0x11, (byte) 0x22, (byte) 0xc2, (byte) 0xc3,
            (byte) 0xc4, (byte) 0xc5, (byte) 0xc6, (byte) 0xc7,
            (byte) 0xd8, (byte) 0xd9, (byte) 0xda, (byte) 0xdb,
            (byte) 0xdc, (byte) 0xdd, (byte) 0xde, (byte) 0xdf,
            // key index
            (byte) 0x03, (byte) 0x04,
            // iv index
            (byte) 0x11, (byte) 0x22, (byte) 0x33, (byte) 0x44,

            // iv update flag
            (byte) 0x01,

            // unicast_address
            (byte) 0x55, (byte) 0x66
    };
}
