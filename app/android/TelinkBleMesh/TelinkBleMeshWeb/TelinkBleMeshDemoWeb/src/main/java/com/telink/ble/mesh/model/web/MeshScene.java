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
