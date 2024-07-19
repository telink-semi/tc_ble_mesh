/********************************************************************************************************
 * @file DemoTest.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date     Sep. 30, 2017
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *******************************************************************************************************/
package com.telink.ble.mesh;


import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.model.GroupInfo;
import com.telink.ble.mesh.model.MeshAppKey;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.MeshNetKey;
import com.telink.ble.mesh.model.UnitConvert;
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

    @Test
    public void testLum() {
        System.out.println(UnitConvert.lum2lightness(1)); // 656
        System.out.println(UnitConvert.lum2lightness(100)); // 65535
    }

    public void testCode(){
        System.out.println("▉"); // ▌
    }
}
