/********************************************************************************************************
 * @file ProxyConfigurationPDU.java
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
package com.telink.ble.mesh.core.proxy;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.networking.NetworkLayerPDU;
import com.telink.ble.mesh.core.networking.NonceGenerator;

import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/26.
 */

public class ProxyConfigurationPDU extends NetworkLayerPDU {

    public static final byte ctl = 1;

    public static final byte ttl = 0;

    public static final byte dst = 0x00;

    public ProxyConfigurationPDU(NetworkEncryptionSuite encryptionSuite) {
        super(encryptionSuite);
    }

    @Override
    protected byte[] generateNonce() {
        byte[] seqNo = MeshUtils.integer2Bytes(getSeq(), 3, ByteOrder.BIG_ENDIAN);
        return NonceGenerator.generateProxyNonce(seqNo, getSrc(), this.encryptionSuite.ivIndex);
    }
}
