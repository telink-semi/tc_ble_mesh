/********************************************************************************************************
 * @file ProvisioningRecordsListPDU.java
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
package com.telink.ble.mesh.core.provisioning.pdu;


import com.telink.ble.mesh.core.MeshUtils;

import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;


/**
 * Created by kee on 2019/7/18.
 */

public class ProvisioningRecordsListPDU implements ProvisioningStatePDU {


    public byte[] rawData;
    /**
     * Bitmask indicating the provisioning extensions supported by the device
     * 2 bytes
     */
    public int provisioningExtensions;

    /**
     * Lists the Record IDs of the provisioning records stored on the device
     */
    public List<Integer> recordsList;

    public static ProvisioningRecordsListPDU fromBytes(byte[] data) {

        int index = 0;
        ProvisioningRecordsListPDU recordsListPDU = new ProvisioningRecordsListPDU();
        recordsListPDU.rawData = data;
        recordsListPDU.provisioningExtensions = MeshUtils.bytes2Integer(data, index, 2, ByteOrder.BIG_ENDIAN);
        index += 2;

        if (data.length > index) {
            final int listSize = (data.length - index) / 2;
            recordsListPDU.recordsList = new ArrayList<>(listSize);
            int recordID;
            for (int i = 0; i < listSize; i++) {
                recordID = MeshUtils.bytes2Integer(data, index, 2, ByteOrder.BIG_ENDIAN);
                recordsListPDU.recordsList.add(recordID);
                index += 2;
            }
        }
        return recordsListPDU;

    }


    @Override
    public byte getState() {
        return ProvisioningPDU.TYPE_RECORDS_LIST;
    }

    @Override
    public byte[] toBytes() {
        return rawData;
    }
}
