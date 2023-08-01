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

import java.io.Serializable;
import java.util.Iterator;

import io.objectbox.annotation.Entity;
import io.objectbox.annotation.Id;
import io.objectbox.relation.ToMany;

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

    public ToMany<SceneState> states;


    public void saveFromDeviceInfo(NodeInfo deviceInfo) {
        for (SceneState state : states) {
            if (state.nodeInfo.getTargetId() == deviceInfo.id) {
                state.onOff = deviceInfo.getOnlineState().st;
                state.lum = deviceInfo.lum;
                state.temp = deviceInfo.temp;
                return;
            }
        }
        SceneState state = new SceneState();
        state.nodeInfo.setTarget(deviceInfo);
        state.onOff = deviceInfo.getOnlineState().st;
        state.lum = deviceInfo.lum;
        state.temp = deviceInfo.temp;
        states.add(state);
    }

    public void removeByAddress(long id) {

        Iterator<SceneState> iterator = states.iterator();
        while (iterator.hasNext()) {
            if (iterator.next().nodeInfo.getTargetId() == id) {
                iterator.remove();
                return;
            }
        }
    }

    public boolean contains(NodeInfo device) {
        for (SceneState inner : states) {
            if (inner.nodeInfo.getTargetId() == device.id) {
                return true;
            }
        }
        return false;
    }
}
