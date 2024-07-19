/********************************************************************************************************
 * @file MeshNetworkShareLink.java
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
 * 由mesh网路创建者建立， 在创建分享链接时使用
 * </p>
 *
 * @author kee
 * @since 2023-04-18
 */
public class MeshNetworkShareLink implements Serializable {

    private static final long serialVersionUID = 1L;

    private Integer id;

    // ("分享创建时间")
    private Long createTime;

    // ("分享结束时间")
    private Long endTime;

    private Integer createUserId;

    private Integer networkId;

    // ("最大允许数量， 0为不限制")
    private Integer maxCount;

    // ("当前数量， 如果大于等于max， 则不允许继续加入")
    private Integer currentCount;


    // ("状态，  0-关闭， 1-开启， 2-删除（不可见）")
    private Integer state;

    public Integer getState() {
        return state;
    }

    public void setState(Integer state) {
        this.state = state;
    }

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public Long getCreateTime() {
        return createTime;
    }

    public void setCreateTime(Long createTime) {
        this.createTime = createTime;
    }

    public Long getEndTime() {
        return endTime;
    }

    public void setEndTime(Long endTime) {
        this.endTime = endTime;
    }

    public Integer getCreateUserId() {
        return createUserId;
    }

    public void setCreateUserId(Integer createUserId) {
        this.createUserId = createUserId;
    }

    public Integer getNetworkId() {
        return networkId;
    }

    public void setNetworkId(Integer networkId) {
        this.networkId = networkId;
    }

    public Integer getMaxCount() {
        return maxCount;
    }

    public void setMaxCount(Integer maxCount) {
        this.maxCount = maxCount;
    }

    public Integer getCurrentCount() {
        return currentCount;
    }

    public void setCurrentCount(Integer currentCount) {
        this.currentCount = currentCount;
    }

    @Override
    public String toString() {
        return "MeshNetworkShareLink{" +
                "id=" + id +
                ", createTime=" + createTime +
                ", endTime=" + endTime +
                ", createUserId=" + createUserId +
                ", networkId=" + networkId +
                ", maxCount=" + maxCount +
                ", currentCount=" + currentCount +
                ", state=" + state +
                "}";
    }
}
