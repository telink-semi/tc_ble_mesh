/********************************************************************************************************
 * @file ForwardingTable.java
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
package com.telink.ble.mesh.core.message.directforwarding;

/**
 * This is a class representing a Forwarding Table used in networking.
 * Created by kee on 2019/8/23.
 */
public class ForwardingTable {

    /**
     * Forwarding Table Entry Header
     * 16 bits
     */
    public int entryHeader;

    /**
     * Path Origin unicast address range
     * variable (16 or 24)
     */
    public int originUnicastAddrRange;

    /**
     * Current number of entries in the list of dependent nodes of the Path Origin
     * variable (8 or 16)
     */
    public int dependentOriginListSize;


    /**
     * Index of the bearer toward the Path Origin
     * 16 bits
     */
    public int bearerTowardPathOrigin;


    /**
     * Path Target unicast address range
     * variable (16 or 24)
     */
    public int targetUnicastAddrRange;


    /**
     * Multicast destination address
     * 16 bits
     */
    public int multicastDestination;

    /**
     * Current number of entries in the list of dependent nodes of the Path Target
     * variable (8 or 16)
     */
    public int dependentTargetListSize;

    /**
     * Index of the bearer toward the Path Target
     * 16 bits
     */
    public int bearerTowardPathTarget;

}
