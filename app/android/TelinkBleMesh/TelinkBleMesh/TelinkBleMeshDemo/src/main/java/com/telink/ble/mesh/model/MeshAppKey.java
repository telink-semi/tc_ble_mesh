/********************************************************************************************************
 * @file MeshAppKey.java
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

import io.objectbox.annotation.Entity;
import io.objectbox.annotation.Id;

/**
 * application key used for access layer encryption
 */
@Entity
public class MeshAppKey implements MeshKey, Serializable {
    @Id
    public long id;
    public String name;
    public int index;
    public byte[] key;
    public int boundNetKeyIndex;

    public MeshAppKey() {
    }

    public MeshAppKey(String name, int index, byte[] key, int boundNetKeyIndex) {
        this.name = name;
        this.index = index;
        this.key = key;
        this.boundNetKeyIndex = boundNetKeyIndex;
    }

    @Override
    public String getName() {
        return this.name;
    }

    @Override
    public int getIndex() {
        return this.index;
    }

    @Override
    public byte[] getKey() {
        return this.key;
    }
}
