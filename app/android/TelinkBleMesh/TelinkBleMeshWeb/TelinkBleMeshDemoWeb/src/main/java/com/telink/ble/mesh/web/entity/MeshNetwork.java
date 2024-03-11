package com.telink.ble.mesh.web.entity;


import java.io.Serializable;


/**
 * @author kee
 * @since 2022-11-25
 */
public class MeshNetwork implements Serializable {

    private static final long serialVersionUID = 1L;

    private Integer id;

    private String networkSchema;

    private String version;

    private String uuid;

    private String name;

    // ("创建的时间 timestamp")
    private Long createTime;

    // ("创建该网络的用户ID")
    private Integer createUserId;

    private Integer ivIndex;

    // ("已分配的节点地址， 可用地址为值+1")
    private Integer nodeAddressIndex;

    // ("已分配的provisioner地址，可用地址为值+1")
    private Integer provisionerAddressIndex;


    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public String getNetworkSchema() {
        return networkSchema;
    }

    public void setNetworkSchema(String networkSchema) {
        this.networkSchema = networkSchema;
    }

    public String getVersion() {
        return version;
    }

    public void setVersion(String version) {
        this.version = version;
    }

    public String getUuid() {
        return uuid;
    }

    public void setUuid(String uuid) {
        this.uuid = uuid;
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

    public Integer getCreateUserId() {
        return createUserId;
    }

    public void setCreateUserId(Integer createUserId) {
        this.createUserId = createUserId;
    }

    public Integer getIvIndex() {
        return ivIndex;
    }

    public void setIvIndex(Integer ivIndex) {
        this.ivIndex = ivIndex;
    }

    public Integer getNodeAddressIndex() {
        return nodeAddressIndex;
    }

    public void setNodeAddressIndex(Integer nodeAddressIndex) {
        this.nodeAddressIndex = nodeAddressIndex;
    }

    public Integer getProvisionerAddressIndex() {
        return provisionerAddressIndex;
    }

    public void setProvisionerAddressIndex(Integer provisionerAddressIndex) {
        this.provisionerAddressIndex = provisionerAddressIndex;
    }

    @Override
    public String toString() {
        return "MeshNetwork{" +
                "id=" + id +
                ", networkSchema=" + networkSchema +
                ", version=" + version +
                ", uuid=" + uuid +
                ", name=" + name +
                ", createTime=" + createTime +
                ", createUserId=" + createUserId +
                ", ivIndex=" + ivIndex +
                ", nodeAddressIndex=" + nodeAddressIndex +
                ", provisionerAddressIndex=" + provisionerAddressIndex +
                "}";
    }
}
