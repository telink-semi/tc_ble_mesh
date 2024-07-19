/********************************************************************************************************
 * @file MeshScene.java
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
 * 场景信息
 * </p>
 *
 * @author kee
 * @since 2023-05-31
 */
public class MeshScene implements Serializable {

    private static final long serialVersionUID = 1L;

    private Integer id;

    // 场景名称
    private String name;

    // mesh_network#id
    private Integer meshNetworkId;

    // mesh消息中的scene id值
    private Integer sceneId;

    // 描述信息
    private String info;

    // 创建时间
    private Long createTime;

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Integer getMeshNetworkId() {
        return meshNetworkId;
    }

    public void setMeshNetworkId(Integer meshNetworkId) {
        this.meshNetworkId = meshNetworkId;
    }

    public Integer getSceneId() {
        return sceneId;
    }

    public void setSceneId(Integer sceneId) {
        this.sceneId = sceneId;
    }

    public String getInfo() {
        return info;
    }

    public void setInfo(String info) {
        this.info = info;
    }

    public Long getCreateTime() {
        return createTime;
    }

    public void setCreateTime(Long createTime) {
        this.createTime = createTime;
    }

    @Override
    public String toString() {
        return "MeshScene{" +
                "id=" + id +
                ", name=" + name +
                ", meshNetworkId=" + meshNetworkId +
                ", sceneId=" + sceneId +
                ", info=" + info +
                "}";
    }
}
