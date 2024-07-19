/********************************************************************************************************
 * @file MeshProvisioner.java
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
package com.telink.ble.mesh.web.entity;


import java.io.Serializable;


/**
 * <p>
 *
 * </p>
 *
 * @author kee
 * @since 2022-11-24
 */
public class MeshProvisioner implements Serializable {

    private static final long serialVersionUID = 1L;

    public Integer id;

    public String name;

    public Integer userId;

    public String clientId;

    public Long createTime;

    public Integer address;

    public Integer sequenceNumber;

    public Integer networkId;

    @Override
    public String toString() {
        return "MeshProvisioner{" +
                "id=" + id +
                ", userId=" + userId +
                ", clientId=" + clientId +
                ", createTime=" + createTime +
                ", address=" + address +
                ", sequenctNumber=" + sequenceNumber +
                "}";
    }
}
