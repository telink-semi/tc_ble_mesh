package com.telink.ble.mesh.web.entity;


import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.Serializable;


/**
 * <p>
 * MeshProductVersionInfo对象
 * 产品的版本信息， 由管理员在后台上传，用于app 对设备进行OTA升级
 * </p>
 *
 * @author kee
 * @since 2023-05-18
 */
public class MeshProductVersionInfo implements Serializable {

    private static final long serialVersionUID = 1L;

    private Integer id;

    // 该版本名称， 例如： 不包含mesh OTA的版本
    private String name;

    // mesh_product_info#id
    private Integer productId;

    // 版本信息， 取composition data 中的 version id 值
    private Integer vid;

    // bin文件路径， 可用于下载文件
    private String binFilePath;

    // 元素个数，从上传的 bin_file中解析
    private Integer elementCount;

    // composition data， 从bin文件中解析获取
    private String compositionData;

    // 描述信息
    private String info;

    // 创建时间
    private Long createTime;

    // 更新时间
    private Long updateTime;

    // 状态： 0-正常， others-待定
    private Integer state;

    // ("composition data 信息")
    private CompositionData compositionDataObj;

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

    public Integer getProductId() {
        return productId;
    }

    public void setProductId(Integer productId) {
        this.productId = productId;
    }

    public Integer getVid() {
        return vid;
    }

    public void setVid(Integer vid) {
        this.vid = vid;
    }

    public String getBinFilePath() {
        return binFilePath;
    }

    public void setBinFilePath(String binFilePath) {
        this.binFilePath = binFilePath;
    }

    public Integer getElementCount() {
        return elementCount;
    }

    public void setElementCount(Integer elementCount) {
        this.elementCount = elementCount;
    }

    public String getCompositionData() {
        return compositionData;
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

    public Long getUpdateTime() {
        return updateTime;
    }

    public void setUpdateTime(Long updateTime) {
        this.updateTime = updateTime;
    }

    public Integer getState() {
        return state;
    }

    public void setState(Integer state) {
        this.state = state;
    }


    public void setCompositionData(String compositionData) {
        MeshLogger.d("versionInfo#setCompositionData - " + compositionData);
        this.compositionData = compositionData;
        this.compositionDataObj = CompositionData.from(
                Arrays.hexToBytes(compositionData)
        );
    }

    public CompositionData getCompositionDataObj() {
        return compositionDataObj;
    }

    public void setCompositionDataObj(CompositionData compositionDataObj) {
        this.compositionDataObj = compositionDataObj;
    }

    @Override
    public String toString() {
        return "MeshProductVersionInfo{" +
                "id=" + id +
                ", name=" + name +
                ", productId=" + productId +
                ", vid=" + vid +
                ", binFilePath=" + binFilePath +
                ", elementCount=" + elementCount +
                ", compositionData=" + compositionData +
                ", info=" + info +
                ", createTime=" + createTime +
                ", updateTime=" + updateTime +
                ", state=" + state +
                "}";
    }
}
