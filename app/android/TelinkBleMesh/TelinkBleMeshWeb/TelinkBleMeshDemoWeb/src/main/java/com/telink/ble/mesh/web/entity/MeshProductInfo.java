/********************************************************************************************************
 * @file MeshProductInfo.java
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
import java.util.Objects;

/**
 * <p>
 * mesh产品信息, 由厂商上传
 * </p>
 *
 * @author kee
 * @since 2022-11-24
 */
// (value = "MeshProductInfo对象", description = "mesh产品信息, 由厂商上传")
public class MeshProductInfo implements Serializable {

    private static final long serialVersionUID = 1L;

    private Integer id;

    // ("创建该产品的厂商ID， manufacturer#id")
    private Integer manufactureId;

    // ("元素个数")
    private Integer elementCount;


    // ("产品的自定义ID， 显示在广播数据中")
    private Integer pid;

    // ("产品名称")
    private String name;

    private Long createTime;

    private Long updateTime;


    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public Integer getManufactureId() {
        return manufactureId;
    }

    public void setManufactureId(Integer manufactureId) {
        this.manufactureId = manufactureId;
    }

    public Integer getElementCount() {
        return elementCount;
    }

    public void setElementCount(Integer elementCount) {
        this.elementCount = elementCount;
    }

    public Integer getPid() {
        return pid;
    }

    public void setPid(Integer pid) {
        this.pid = pid;
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

    @Override
    public String toString() {
        return "MeshProductInfo{" +
                "id=" + id +
                ", manufactureId=" + manufactureId +
                ", elementCount=" + elementCount +
                ", productId=" + pid +
                ", name=" + name +
                ", createTime=" + createTime +
                ", updateTime=" + updateTime +
                "}";
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        MeshProductInfo that = (MeshProductInfo) o;
        return Objects.equals(id, that.id);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id);
    }
}
