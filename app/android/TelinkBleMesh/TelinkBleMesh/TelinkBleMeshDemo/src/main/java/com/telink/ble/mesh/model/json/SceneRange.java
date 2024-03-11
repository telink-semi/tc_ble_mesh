/********************************************************************************************************
 * @file SceneRange.java
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

import io.objectbox.annotation.Entity;
import io.objectbox.annotation.Id;

@Entity
public class SceneRange {
    @Id
    public long id;
    public int firstScene;
    public int lastScene;

    public SceneRange() {
    }

    public SceneRange(int firstScene, int lastScene) {
        this.firstScene = firstScene;
        this.lastScene = lastScene;
    }


}