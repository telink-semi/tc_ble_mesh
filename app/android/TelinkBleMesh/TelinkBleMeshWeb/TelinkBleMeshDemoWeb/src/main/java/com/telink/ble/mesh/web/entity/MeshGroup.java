/********************************************************************************************************
 * @file MeshGroup.java
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
 * mesh分组信息
 * </p>
 *
 * @author kee
 * @since 2022-11-29
 */
public class MeshGroup implements Serializable {

    private static final long serialVersionUID = 1L;

    public Integer id;

    // ("mesh_network#id")
    public Integer networkId;

    // ("创建该分组的用户ID， user#id")
    public Integer createUserId;

    public Integer address;

    // ("分组名称")
    public String name;

    // ("创建时间")
    public Long createTime;

    // ("更新信息")
    public Long updateTime;

    // ("创建该分组的provisioner")
    public Integer createProvisionerId;

    public boolean selected = false;


    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public Integer getNetworkId() {
        return networkId;
    }

    public void setNetworkId(Integer networkId) {
        this.networkId = networkId;
    }

    public Integer getCreateUserId() {
        return createUserId;
    }

    public void setCreateUserId(Integer createUserId) {
        this.createUserId = createUserId;
    }

    public Integer getAddress() {
        return address;
    }

    public void setAddress(Integer address) {
        this.address = address;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Long getCreateTime() {
        return createTime;
    }

    public void setCreateTime(Long createTime) {
        this.createTime = createTime;
    }

    public Long getUpdateTime() {
        return updateTime;
    }

    public void setUpdateTime(Long updateTime) {
        this.updateTime = updateTime;
    }

    public Integer getCreateProvisionerId() {
        return createProvisionerId;
    }

    public void setCreateProvisionerId(Integer createProvisionerId) {
        this.createProvisionerId = createProvisionerId;
    }

    @Override
    public String toString() {
        return "MeshGroup{" +
        "id=" + id +
        ", networkId=" + networkId +
        ", createUserId=" + createUserId +
        ", address=" + address +
        ", name=" + name +
        ", createTime=" + createTime +
        ", updateTime=" + updateTime +
        ", createProvisionerId=" + createProvisionerId +
        "}";
    }
}
