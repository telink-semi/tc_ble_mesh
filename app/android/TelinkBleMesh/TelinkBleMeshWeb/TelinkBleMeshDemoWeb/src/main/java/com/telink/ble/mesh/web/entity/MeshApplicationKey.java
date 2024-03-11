package com.telink.ble.mesh.web.entity;


import java.io.Serializable;

/**
 * <p>
 * app key
 * </p>
 *
 * @author kee
 * @since 2022-11-24
 */
public class MeshApplicationKey implements Serializable {

    private static final long serialVersionUID = 1L;

    public Integer id;

    public String name;

    public Integer networkId;

    public Integer index;

    public String key;

    public String oldKey;

    public Integer networkKeyId;

    public Long createTime;

    public Long updateTime;

    public String description;


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

    public Integer getNetworkId() {
        return networkId;
    }

    public void setNetworkId(Integer networkId) {
        this.networkId = networkId;
    }

    public Integer getIndex() {
        return index;
    }

    public void setIndex(Integer index) {
        this.index = index;
    }

    public String getKey() {
        return key;
    }

    public void setKey(String key) {
        this.key = key;
    }

    public String getOldKey() {
        return oldKey;
    }

    public void setOldKey(String oldKey) {
        this.oldKey = oldKey;
    }

    public Integer getNetworkKeyId() {
        return networkKeyId;
    }

    public void setNetworkKeyId(Integer networkKeyId) {
        this.networkKeyId = networkKeyId;
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

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    @Override
    public String toString() {
        return "MeshApplicationKey{" +
        "id=" + id +
        ", name=" + name +
        ", networkId=" + networkId +
        ", index=" + index +
        ", key=" + key +
        ", oldKey=" + oldKey +
        ", networkKeyId=" + networkKeyId +
        ", createTime=" + createTime +
        ", updateTime=" + updateTime +
        ", description=" + description +
        "}";
    }
}
