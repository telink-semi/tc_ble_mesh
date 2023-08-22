/********************************************************************************************************
 * @file ProvisioningPubKeyPDU.java
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
package com.telink.ble.mesh.core.provisioning.pdu;


/**
 * Created by kee on 2019/7/18.
 */

public class ProvisioningPubKeyPDU implements ProvisioningStatePDU {
    private static final int LEN = 64;
    // 32 bytes
    public byte[] x;
    public byte[] y;

    private byte[] rawData;

    public static ProvisioningPubKeyPDU fromBytes(byte[] data) {
        if (data.length != LEN) return null;

        ProvisioningPubKeyPDU pubKeyPDU = new ProvisioningPubKeyPDU();
        pubKeyPDU.rawData = data;
        pubKeyPDU.x = new byte[32];
        pubKeyPDU.y = new byte[32];

        System.arraycopy(data, 0, pubKeyPDU.x, 0, 32);
        System.arraycopy(data, 32, pubKeyPDU.y, 0, 32);

        return pubKeyPDU;
    }

    @Override
    public byte[] toBytes() {
        if (rawData != null) return rawData;
        if (x == null || y == null) return null;
        byte[] re = new byte[LEN];
        System.arraycopy(x, 0, re, 0, x.length);
        System.arraycopy(y, 0, re, 32, y.length);
        return re;
    }

    @Override
    public byte getState() {
        return ProvisioningPDU.TYPE_PUBLIC_KEY;
    }
}
