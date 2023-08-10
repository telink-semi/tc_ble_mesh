/********************************************************************************************************
 * @file Scene.java
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

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.model.db.MeshInfoService;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

import io.objectbox.annotation.Entity;
import io.objectbox.annotation.Id;

/**
 * scene
 * Created by kee on 2018/10/8.
 */

@Entity
public class Scene implements Serializable {

    @Id
    public long id;

    /**
     * scene name
     */
    public String name = "Telink-Scene";

    /**
     * scene id
     */
    public int sceneId;

    /**
     * element address(unicast address) list, hex big endian
     */
    public List<String> addressList = new ArrayList<>();

    public void save(int address) {
        if (addressList == null) {
            addressList = new ArrayList<>();
        }
        addressList.add(MeshUtils.intToHex2(address));
        MeshInfoService.getInstance().updateScene(this);
    }

    public void remove(int address) {
        String formatAdr = MeshUtils.intToHex2(address);
        addressList.remove(formatAdr);
    }

    public boolean remove(NodeInfo nodeInfo) {
        boolean anyRemoved = false;
        if (nodeInfo.compositionData != null) {
            int adr;
            for (int i = 0; i < nodeInfo.compositionData.elements.size(); i++) {
                adr = i + nodeInfo.meshAddress;
                if (this.contains(adr)) {
                    remove(adr);
                    anyRemoved = true;
                }
            }
        }
        return anyRemoved;
    }

    public boolean contains(int address) {
        if (addressList == null || addressList.size() == 0) return false;
        String formatAdr = MeshUtils.intToHex2(address);
        return addressList.contains(formatAdr);
    }
}
