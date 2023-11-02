/********************************************************************************************************
 * @file ConnectionFilter.java
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
package com.telink.ble.mesh.entity;

/**
 * The ConnectionFilter class represents a filter used to specify the criteria for filtering connections.
 * It provides constants for different types of filters such as mesh address, MAC address, and device name.
 * The type of filter and the target object to be filtered are specified in the constructor.
 * The advFilter property is used to specify additional filtering criteria for mesh advertisements.
 * <p>
 * Example usage:
 * ConnectionFilter filter = new ConnectionFilter(ConnectionFilter.TYPE_MESH_ADDRESS, "00:11:22:33:44:55");
 * filter.advFilter = MeshAdvFilter.BEACON;
 *
 * @see MeshAdvFilter
 */
public class ConnectionFilter {
    public static final int TYPE_MESH_ADDRESS = 0;
    public static final int TYPE_MAC_ADDRESS = 1;
    public static final int TYPE_DEVICE_NAME = 2;
    public int type;
    public Object target;
    public MeshAdvFilter advFilter = MeshAdvFilter.ANY;

    /**
     * Constructs a ConnectionFilter object with the specified filter type and target object.
     *
     * @param type   The type of filter (e.g., TYPE_MESH_ADDRESS, TYPE_MAC_ADDRESS, TYPE_DEVICE_NAME)
     * @param target The target object to be filtered
     */
    public ConnectionFilter(int type, Object target) {
        this.type = type;
        this.target = target;
    }
}