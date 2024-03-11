/********************************************************************************************************
 * @file Provisioner.java
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
package com.telink.ble.mesh.model.json;

import com.telink.ble.mesh.core.MeshUtils;

import java.util.ArrayList;

import io.objectbox.annotation.Entity;
import io.objectbox.annotation.Id;
import io.objectbox.relation.ToMany;

@Entity
public class Provisioner {
    @Id
    public long id;

    public String provisionerName;

    public String UUID;


    public ToMany<AddressRange> allocatedUnicastRange;
    public ToMany<AddressRange> allocatedGroupRange;

    public ToMany<SceneRange> allocatedSceneRange;

    public Provisioner() {
    }

    public static Provisioner from(MeshStorage.Provisioner provisioner) {
        Provisioner pv = new Provisioner();
        pv.provisionerName = provisioner.provisionerName;
        pv.UUID = provisioner.UUID;
        for (MeshStorage.Provisioner.AddressRange unRange :
                provisioner.allocatedUnicastRange) {
            pv.allocatedUnicastRange.add(new AddressRange(MeshUtils.hexToIntB(unRange.lowAddress), MeshUtils.hexToIntB(unRange.highAddress)));
        }

        for (MeshStorage.Provisioner.AddressRange unRange :
                provisioner.allocatedGroupRange) {
            pv.allocatedGroupRange.add(new AddressRange(MeshUtils.hexToIntB(unRange.lowAddress), MeshUtils.hexToIntB(unRange.highAddress)));
        }

        for (MeshStorage.Provisioner.SceneRange sceneRange :
                provisioner.allocatedSceneRange) {
            pv.allocatedSceneRange.add(new SceneRange(MeshUtils.hexToIntB(sceneRange.firstScene), MeshUtils.hexToIntB(sceneRange.lastScene)));
        }
        return pv;
    }

    /**
     * convert provisioner to JSON
     *
     * @return
     */
    public MeshStorage.Provisioner convert() {
        MeshStorage.Provisioner provisioner = new MeshStorage.Provisioner();
        provisioner.UUID = UUID;
        provisioner.provisionerName = provisionerName;

        provisioner.allocatedUnicastRange = new ArrayList<>();
        for (AddressRange range : allocatedUnicastRange) {
            provisioner.allocatedUnicastRange.add(
                    new MeshStorage.Provisioner.AddressRange(String.format("%04X", range.low), String.format("%04X", range.high))
            );
        }

        provisioner.allocatedGroupRange = new ArrayList<>();
        for (AddressRange range : allocatedGroupRange) {
            provisioner.allocatedGroupRange.add(
                    new MeshStorage.Provisioner.AddressRange(String.format("%04X", range.low), String.format("%04X", range.high))
            );
        }

        provisioner.allocatedSceneRange = new ArrayList<>();
        for (SceneRange range : allocatedSceneRange) {
            provisioner.allocatedSceneRange.add(
                    new MeshStorage.Provisioner.SceneRange(String.format("%04X", range.firstScene), String.format("%04X", range.lastScene))
            );
        }
        return provisioner;
    }
}