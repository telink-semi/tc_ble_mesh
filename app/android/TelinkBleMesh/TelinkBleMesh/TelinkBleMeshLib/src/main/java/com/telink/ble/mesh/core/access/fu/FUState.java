/********************************************************************************************************
 * @file FUState.java
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
package com.telink.ble.mesh.core.access.fu;


/**
 * firmware-update state
 */
public enum FUState {
    IDLE(0x0100,
            "initiating distributor ..."
    ),

    INITIATING(0,
            "initiating distributor ..."
    ),

    INITIATE_SUCCESS(1,
            "initiate distributor success"
    ),

    INITIATE_FAIL(2,
            "initiate distributor fail"
    ),

    DISTRIBUTING_BY_PHONE(3,
            "distributing by phone..."),

    DISTRIBUTING_BY_DEVICE(4,
            "distributing by device..."),

    /**
     * applying devices update
     */
    UPDATE_APPLYING(7,
            "firmware update applying"),

    /**
     * confirm distribute
     */
    DISTRIBUTE_CONFIRMING(8,
            "getting distribute status util complete"),

    /**
     * recheck firmware id
     */
    UPDATE_RECHECKING(9,
            "firmware update rechecking..."),


    UPDATE_COMPLETE(10,
            "update complete"),

    UPDATE_FAIL(11,
            "update fail"),

    TRANSFER_START(12,
            "blob transfer started"),
    ;

    public final int value;

    public final String desc;

    FUState(int value, String desc) {
        this.value = value;
        this.desc = desc;
    }
}
