/********************************************************************************************************
 * @file GattRequest.java
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

package com.telink.ble.mesh.core.ble;


import com.telink.ble.mesh.util.Arrays;

import java.util.UUID;

/**
 * gatt request with callback
 */
public class GattRequest {

    public UUID serviceUUID;
    public UUID characteristicUUID;
    public UUID descriptorUUID;
    public RequestType type;
    public byte[] data;
    public Object tag;
    public int delay;
    public Callback callback;
    public int mtu;

    public GattRequest() {
        this(null, null, RequestType.WRITE);
    }

    public GattRequest(UUID serviceUUID, UUID characteristicUUID, RequestType type) {
        this(serviceUUID, characteristicUUID, type, null);
    }

    public GattRequest(UUID serviceUUID, UUID characteristicUUID, RequestType type,
                       byte[] data) {
        this(serviceUUID, characteristicUUID, type, data, null);
    }

    public GattRequest(UUID serviceUUID, UUID characteristicUUID, RequestType type,
                       byte[] data, Object tag) {

        this.serviceUUID = serviceUUID;
        this.characteristicUUID = characteristicUUID;
        this.type = type;
        this.data = data;
        this.tag = tag;
    }

    public static GattRequest newInstance() {
        return new GattRequest();
    }

    public void clear() {
        this.serviceUUID = null;
        this.characteristicUUID = null;
        this.descriptorUUID = null;
        this.data = null;
    }

    @Override
    public String toString() {
        String d;
        if (data != null) {
            d = Arrays.bytesToHexString(this.data);
        } else {
            d = "null";
        }
        return "{ tag : " + this.tag + ", type : " + this.type
                + " CHARACTERISTIC_UUID :" + characteristicUUID.toString() + " data: " + d + " delay :" + delay + "}";
    }

    public enum RequestType {
        READ, READ_DESCRIPTOR, WRITE, WRITE_NO_RESPONSE, WRITE_DESCRIPTOR, ENABLE_NOTIFY, DISABLE_NOTIFY, REQUEST_MTU
    }

    public interface Callback {

        void success(GattRequest request, Object obj);

        void error(GattRequest request, String errorMsg);

        /**
         * @return retry
         */
        boolean timeout(GattRequest request);
    }
}
