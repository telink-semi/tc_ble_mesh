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
