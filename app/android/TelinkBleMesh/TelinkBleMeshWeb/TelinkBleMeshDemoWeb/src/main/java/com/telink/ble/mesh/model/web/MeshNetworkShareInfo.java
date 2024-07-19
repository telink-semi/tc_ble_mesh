/********************************************************************************************************
 * @file MeshNetworkShareInfo.java
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
package com.telink.ble.mesh.model.web;

import java.io.Serializable;

/**
 * <p>
 * 网络分享表
 * </p>
 *
 * @author kee
 * @since 2023-04-12
 */
public class MeshNetworkShareInfo implements Serializable {

    private static final long serialVersionUID = 1L;

    // (value = "id", type = IdType.AUTO)
    private Integer id;

    // "网络ID， mesh_network#id"
    private Integer networkId;

    // "申请的用户id： user#id"
    private Integer applyUserId;

    // "状态： 0-申请中； 1-正常； 2-被禁用；"
    /**
     * @see NetworkShareState
     */
    private Integer state;

    // "创建的时间戳"
    private Long createTime;

    // "更新的时间戳"
    private Long updateTime;

    private String applyUserName;

    /**
     * 网络名称
     */
    private String networkName;

    /**
     * 网络创建者
     */
    private String networkCreator;

    public String getApplyUserName() {
        return applyUserName;
    }

    public void setApplyUserName(String applyUserName) {
        this.applyUserName = applyUserName;
    }

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

    public Integer getApplyUserId() {
        return applyUserId;
    }

    public void setApplyUserId(Integer applyUserId) {
        this.applyUserId = applyUserId;
    }

    public Integer getState() {
        return state;
    }

    public void setState(Integer state) {
        this.state = state;
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

    public String getNetworkName() {
        return networkName;
    }

    public void setNetworkName(String networkName) {
        this.networkName = networkName;
    }

    public String getNetworkCreator() {
        return networkCreator;
    }

    public void setNetworkCreator(String networkCreator) {
        this.networkCreator = networkCreator;
    }

    @Override
    public String toString() {
        return "MeshNetworkShareInfo{" +
                "id=" + id +
                ", networkId=" + networkId +
                ", applyUserId=" + applyUserId +
                ", state=" + state +
                ", createTime=" + createTime +
                ", updateTime=" + updateTime +
                "}";
    }
}
