package com.telink.ble.mesh.model.web;


import com.telink.ble.mesh.model.Scheduler;
import com.telink.ble.mesh.model.SchedulerRegister;
import com.telink.ble.mesh.util.Arrays;

import java.io.Serializable;

/**
 * <p>
 * 闹钟数据
 * </p>
 *
 * @author kee
 * @since 2023-06-01
 */
// ("mesh_scheduler")
// 闹钟数据
public class MeshScheduler implements Serializable {

    private static final long serialVersionUID = 1L;

    private Integer id;

    // scheduler name
    private String name;

    // mesh_node#id
    private Integer meshNodeId;

    // hex string
    private String params;

    // mesh_scene#id
    private Integer meshSceneId;

    // 创建时间
    private Long createTime;

    // 描述信息
    private String info;

    // 闹钟索引, 与params第一个字节的值一致
    private Integer schedulerIndex;

    // 从params中解析获取
    private Scheduler scheduler;

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

    public Integer getMeshNodeId() {
        return meshNodeId;
    }

    public void setMeshNodeId(Integer meshNodeId) {
        this.meshNodeId = meshNodeId;
    }

    public String getParams() {
        return params;
    }

    public void setParams(String params) {
        this.params = params;
        byte[] bf = Arrays.hexToBytes(params);
        scheduler = Scheduler.fromBytes(bf);
    }

    public Integer getMeshSceneId() {
        return meshSceneId;
    }

    public void setMeshSceneId(Integer meshSceneId) {
        this.meshSceneId = meshSceneId;
    }

    public Long getCreateTime() {
        return createTime;
    }

    public void setCreateTime(Long createTime) {
        this.createTime = createTime;
    }

    public String getInfo() {
        return info;
    }

    public void setInfo(String info) {
        this.info = info;
    }

    public Integer getSchedulerIndex() {
        return schedulerIndex;
    }

    public void setSchedulerIndex(Integer schedulerIndex) {
        this.schedulerIndex = schedulerIndex;
    }

    public Scheduler getScheduler() {
        return scheduler;
    }

    public SchedulerRegister getRegister() {
        if (scheduler == null) return null;
        return scheduler.getRegister();
    }

    @Override
    public String toString() {
        return "MeshScheduler{" +
                "id=" + id +
                ", name='" + name + '\'' +
                ", meshNodeId=" + meshNodeId +
                ", params='" + params + '\'' +
                ", meshSceneId=" + meshSceneId +
                ", createTime=" + createTime +
                ", info='" + info + '\'' +
                ", index=" + schedulerIndex +
                '}';
    }
}
