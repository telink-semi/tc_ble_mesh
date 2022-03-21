/********************************************************************************************************
 * @file FUCache.java
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

import com.telink.ble.mesh.core.access.fu.UpdatePolicy;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;

import java.io.Serializable;
import java.util.List;

/**
 * firmware update cache,
 * saved when distribution started by remote device,
 * retrieved when mesh reconnect success
 */
public class FUCache implements Serializable {

    /**
     * distributor address, should be valid unicast address
     */
    public int distAddress;

    /**
     * firmware update policy
     */
    public UpdatePolicy updatePolicy;

    /**
     * updating devices
     */
    public List<MeshUpdatingDevice> updatingDeviceList;

    /**
     * target firmware id
     */
    public byte[] firmwareId;

    @Override
    public String toString() {
        return "FUCache{" +
                "distAddress=" + distAddress +
                ", updatePolicy=" + updatePolicy +
                ", updatingDeviceList=" + updatingDeviceList.size() +
                '}';
    }
}
