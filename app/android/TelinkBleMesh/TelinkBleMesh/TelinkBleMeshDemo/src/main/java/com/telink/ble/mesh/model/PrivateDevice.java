/********************************************************************************************************
 * @file PrivateDevice.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2017
 *
 * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
package com.telink.ble.mesh.model;

import com.telink.ble.mesh.entity.BindingDevice;

/**
 * used in default-bind and fast-provision mode
 * vid , pid and composition raw data
 * {@link BindingDevice#isDefaultBound()}
 * Created by kee on 2019/2/27.
 */
public enum PrivateDevice {

    PANEL(0x0211, 0x07, "panel",
            new byte[]{(byte) 0x11, (byte) 0x02,
                    (byte) 0x07, (byte) 0x00,
                    (byte) 0x32, (byte) 0x37,
                    (byte) 0x69, (byte) 0x00, (byte) 0x07, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x11, (byte) 0x02, (byte) 0x00, (byte) 0x00
                    , (byte) 0x02, (byte) 0x00, (byte) 0x03, (byte) 0x00, (byte) 0x04, (byte) 0x00, (byte) 0x05, (byte) 0x00, (byte) 0x00, (byte) 0xfe, (byte) 0x01, (byte) 0xfe, (byte) 0x02, (byte) 0xfe, (byte) 0x00, (byte) 0xff
                    , (byte) 0x01, (byte) 0xff, (byte) 0x00, (byte) 0x12, (byte) 0x01, (byte) 0x12, (byte) 0x00, (byte) 0x10, (byte) 0x03, (byte) 0x12, (byte) 0x04, (byte) 0x12, (byte) 0x06, (byte) 0x12, (byte) 0x07, (byte) 0x12
                    , (byte) 0x11, (byte) 0x02, (byte) 0x00, (byte) 0x00, (byte) 0x11, (byte) 0x02, (byte) 0x01, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x05, (byte) 0x01, (byte) 0x00, (byte) 0x10, (byte) 0x03, (byte) 0x12
                    , (byte) 0x04, (byte) 0x12, (byte) 0x06, (byte) 0x12, (byte) 0x07, (byte) 0x12, (byte) 0x11, (byte) 0x02, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x05, (byte) 0x01, (byte) 0x00, (byte) 0x10
                    , (byte) 0x03, (byte) 0x12, (byte) 0x04, (byte) 0x12, (byte) 0x06, (byte) 0x12, (byte) 0x07, (byte) 0x12, (byte) 0x11, (byte) 0x02, (byte) 0x00, (byte) 0x00}),

    CT(0x0211, 0x01, "ct",
            new byte[]{
                    (byte) 0x11, (byte) 0x02,
                    (byte) 0x01, (byte) 0x00,
                    (byte) 0x32, (byte) 0x37,
                    (byte) 0x69, (byte) 0x00, (byte) 0x07, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x19, (byte) 0x01, (byte) 0x00, (byte) 0x00
                    , (byte) 0x02, (byte) 0x00, (byte) 0x03, (byte) 0x00, (byte) 0x04, (byte) 0x00, (byte) 0x05, (byte) 0x00, (byte) 0x00, (byte) 0xfe, (byte) 0x01, (byte) 0xfe, (byte) 0x02, (byte) 0xfe, (byte) 0x00, (byte) 0xff
                    , (byte) 0x01, (byte) 0xff, (byte) 0x00, (byte) 0x12, (byte) 0x01, (byte) 0x12, (byte) 0x00, (byte) 0x10, (byte) 0x02, (byte) 0x10, (byte) 0x04, (byte) 0x10, (byte) 0x06, (byte) 0x10, (byte) 0x07, (byte) 0x10
                    , (byte) 0x03, (byte) 0x12, (byte) 0x04, (byte) 0x12, (byte) 0x06, (byte) 0x12, (byte) 0x07, (byte) 0x12, (byte) 0x00, (byte) 0x13, (byte) 0x01, (byte) 0x13, (byte) 0x03, (byte) 0x13, (byte) 0x04, (byte) 0x13
                    , (byte) 0x11, (byte) 0x02, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x02, (byte) 0x00, (byte) 0x02, (byte) 0x10, (byte) 0x06, (byte) 0x13}),
    HSL(0x0211, 0x02, "hsl",
            new byte[]{
                    (byte) 0x11, (byte) 0x02, // cid
                    (byte) 0x02, (byte) 0x00, // pid
                    (byte) 0x33, (byte) 0x31, // vid
                    (byte) 0x69, (byte) 0x00, (byte) 0x07, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x11, (byte) 0x01, (byte) 0x00, (byte) 0x00, (byte) 0x02, (byte) 0x00, (byte) 0x03, (byte) 0x00, (byte) 0x04, (byte) 0x00,
                    (byte) 0x00, (byte) 0xFE, (byte) 0x01, (byte) 0xFE, (byte) 0x00, (byte) 0xFF, (byte) 0x01, (byte) 0xFF, (byte) 0x00, (byte) 0x10, (byte) 0x02, (byte) 0x10, (byte) 0x04, (byte) 0x10, (byte) 0x06, (byte) 0x10,
                    (byte) 0x07, (byte) 0x10, (byte) 0x00, (byte) 0x13, (byte) 0x01, (byte) 0x13, (byte) 0x07, (byte) 0x13, (byte) 0x08, (byte) 0x13, (byte) 0x11, (byte) 0x02, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
                    (byte) 0x02, (byte) 0x00, (byte) 0x02, (byte) 0x10, (byte) 0x0A, (byte) 0x13, (byte) 0x00, (byte) 0x00, (byte) 0x02, (byte) 0x00, (byte) 0x02, (byte) 0x10, (byte) 0x0B, (byte) 0x13
            }),
    LPN(0x0211, 0x0201, "lpn",
            new byte[]{
                    (byte) 0x11, (byte) 0x02, // cid
                    0x01, 0x02,// pid
                    (byte) 0x33, (byte) 0x33, // vid
                    0x69, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x05, 0x01, 0x00, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00, 0x10, 0x02, 0x10, 0x11, 0x02, 0x00, 0x00
            }),

    SWITCH(0x0211, 0x0301, "switch",
                new byte[]{
                        0x11, 0x02,
                        0x01, 0x03,
                        0x33, 0x35,
                        0x69, 0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0x02, 0x00, 0x00,
                        0x02, 0x00, 0x03, 0x00, 0x00, 0x10, 0x01, 0x10, 0x11, 0x02, 0x00, 0x00, 0x11, 0x02, 0x01, 0x00,
                        0x00, 0x00, 0x02, 0x01, 0x00, 0x10, 0x01, 0x10, 0x11, 0x02, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01,
                        0x00, 0x10, 0x01, 0x10, 0x11, 0x02, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x10, 0x01, 0x10,
                        0x11, 0x02, 0x00, 0x00
    });


    PrivateDevice(int vid, int pid, String name, byte[] cpsData) {
        this.vid = vid;
        this.pid = pid;
        this.name = name;
        this.cpsData = cpsData;
    }

    private final int vid;
    private final int pid;
    private final String name;
    private final byte[] cpsData;

    public int getVid() {
        return vid;
    }

    public int getPid() {
        return pid;
    }

    public String getName() {
        return name;
    }

    public byte[] getCpsData() {
        return cpsData;
    }

    /**
     * check private device
     *
     * @param deviceUUID deviceUUID from scanRecord
     * @return preset device
     */
    public static PrivateDevice filter(byte[] deviceUUID) {
        if (deviceUUID.length < 3) return null;
        int vid = (deviceUUID[0] & 0xFF) + (((deviceUUID[1] & 0xFF) << 8));
        int pid = deviceUUID[2] & 0xFF;
        PrivateDevice[] values = PrivateDevice.values();
        for (PrivateDevice device :
                values) {
            if (device.vid == vid && device.pid == pid) {
                return device;
            }
        }
        return null;

    }
}
