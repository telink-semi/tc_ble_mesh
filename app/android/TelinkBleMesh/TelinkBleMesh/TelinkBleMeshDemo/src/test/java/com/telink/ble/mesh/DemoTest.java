/********************************************************************************************************
 * @file MeshTest.java
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
package com.telink.ble.mesh;


import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.model.GroupInfo;
import com.telink.ble.mesh.model.MeshAppKey;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.MeshNetKey;
import com.telink.ble.mesh.model.json.AddressRange;
import com.telink.ble.mesh.model.json.MeshStorageService;

import org.junit.Test;

import java.util.ArrayList;

/**
 * Created by kee on 2019/10/14.
 */

public class DemoTest {

    @Test
    public void testMeshStorageExport() {

        final int DEFAULT_LOCAL_ADDRESS = 0x0001;
        MeshInfo meshInfo = new MeshInfo();

        meshInfo.meshNetKeyList = new ArrayList<>();
        final int KEY_COUNT = 3;
        final String[] NET_KEY_NAMES = {"Default Net Key", "Sub Net Key 1", "Sub Net Key 2"};
        final String[] APP_KEY_NAMES = {"Default App Key", "Sub App Key 1", "Sub App Key 2"};
        final byte[] APP_KEY_VAL = MeshUtils.generateRandom(16);
        for (int i = 0; i < KEY_COUNT; i++) {
            meshInfo.meshNetKeyList.add(new MeshNetKey(NET_KEY_NAMES[i], i, MeshUtils.generateRandom(16)));
            meshInfo.appKeyList.add(new MeshAppKey(APP_KEY_NAMES[i],
                    i, APP_KEY_VAL, i));
        }

        meshInfo.ivIndex = 0;
        meshInfo.sequenceNumber = 0;
        meshInfo.nodes = new ArrayList<>();
        meshInfo.localAddress = DEFAULT_LOCAL_ADDRESS;
        meshInfo.provisionerUUID = MeshUtils.byteArrayToUuid((MeshUtils.generateRandom(16)));

        meshInfo.groups = new ArrayList<>();
        meshInfo.unicastRange = new ArrayList<>();
        meshInfo.unicastRange.add(new AddressRange(0x01, 0x400));
        meshInfo.addressTopLimit = 0x0400;
        String[] groupNames = {"Living room", "Kitchen"};
        GroupInfo group;
        for (int i = 0; i < groupNames.length; i++) {
            group = new GroupInfo();
            group.address = i | 0xC000;
            group.name = groupNames[i];
            meshInfo.groups.add(group);
        }

        String s = MeshStorageService.getInstance().meshToJsonString(meshInfo, meshInfo.meshNetKeyList);
        loge("JSON", s);
    }

    public static void loge(String tag, String msg) {
        if (tag == null || tag.length() == 0
                || msg == null || msg.length() == 0)
            return;

        int segmentSize = 3 * 1024;
        long length = msg.length();
        if (length <= segmentSize) {
            System.out.println(msg);
        } else {
            while (msg.length() > segmentSize) {
                String logContent = msg.substring(0, segmentSize);
                msg = msg.replace(logContent, "");
                System.out.println(logContent);
            }
            System.out.println(msg);
        }
    }

}
