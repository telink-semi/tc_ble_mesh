/********************************************************************************************************
 * @file MeshNodePublish.java
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

import com.telink.ble.mesh.entity.ModelPublication;

import java.io.Serializable;

/**
 * <p>
 *
 * </p>
 *
 * @author kee
 * @since 2023-12-19
 */
// "MeshNodePublish对象"
public class MeshNodePublish implements Serializable {

    private static final long serialVersionUID = 1L;

    private Integer id;

    private Integer nodeId;

    private Integer elementAddress;

    private Integer publishAddress;

    // application key index
    private Integer appKeyIndex;

    // Value of the Friendship Credential Flag, 1 bit
    private Integer credentialFlag;

    private Integer ttl;

    private Integer period;

    private Integer retransmitCount;

    private Integer retransmitIntervalSteps;

    private Integer modelId;

    // bool, 是否为SIG model
    private Integer isSigModel;

    private Long createTime;

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public Integer getNodeId() {
        return nodeId;
    }

    public void setNodeId(Integer nodeId) {
        this.nodeId = nodeId;
    }

    public Integer getElementAddress() {
        return elementAddress;
    }

    public void setElementAddress(Integer elementAddress) {
        this.elementAddress = elementAddress;
    }

    public Integer getPublishAddress() {
        return publishAddress;
    }

    public void setPublishAddress(Integer publishAddress) {
        this.publishAddress = publishAddress;
    }

    public Integer getAppKeyIndex() {
        return appKeyIndex;
    }

    public void setAppKeyIndex(Integer appKeyIndex) {
        this.appKeyIndex = appKeyIndex;
    }

    public Integer getCredentialFlag() {
        return credentialFlag;
    }

    public void setCredentialFlag(Integer credentialFlag) {
        this.credentialFlag = credentialFlag;
    }

    public Integer getTtl() {
        return ttl;
    }

    public void setTtl(Integer ttl) {
        this.ttl = ttl;
    }

    public Integer getPeriod() {
        return period;
    }

    public void setPeriod(Integer period) {
        this.period = period;
    }

    public Integer getRetransmitCount() {
        return retransmitCount;
    }

    public void setRetransmitCount(Integer retransmitCount) {
        this.retransmitCount = retransmitCount;
    }

    public Integer getRetransmitIntervalSteps() {
        return retransmitIntervalSteps;
    }

    public void setRetransmitIntervalSteps(Integer retransmitIntervalSteps) {
        this.retransmitIntervalSteps = retransmitIntervalSteps;
    }

    public Integer getModelId() {
        return modelId;
    }

    public void setModelId(Integer modelId) {
        this.modelId = modelId;
    }

    public Integer getIsSigModel() {
        return isSigModel;
    }

    public void setIsSigModel(Integer isSigModel) {
        this.isSigModel = isSigModel;
    }

    public Long getCreateTime() {
        return createTime;
    }

    public void setCreateTime(Long createTime) {
        this.createTime = createTime;
    }

    public static MeshNodePublish from(ModelPublication publication) {
        if (publication == null) return null;
        MeshNodePublish publish = new MeshNodePublish();
        publish.elementAddress = publication.elementAddress;
        publish.publishAddress = publication.publishAddress;
        publish.appKeyIndex = publication.appKeyIndex;
        publish.credentialFlag = publication.credentialFlag;
        publish.ttl = publication.ttl;
        publish.period = publication.period & 0xFF;
        publish.retransmitCount = publication.retransmitCount;
        publish.retransmitIntervalSteps = publication.retransmitIntervalSteps;
        publish.modelId = publication.modelId;
        publish.isSigModel = publication.sig ? 1 : 0;
        return publish;
    }

    @Override
    public String toString() {
        return "MeshNodePublish{" +
                "id=" + id +
                ", nodeId=" + nodeId +
                ", elementAddress=" + elementAddress +
                ", publishAddress=" + publishAddress +
                ", appKeyIndex=" + appKeyIndex +
                ", credentialFlag=" + credentialFlag +
                ", ttl=" + ttl +
                ", period=" + period +
                ", retransmitCount=" + retransmitCount +
                ", retransmitIntervalSteps=" + retransmitIntervalSteps +
                ", modelId=" + modelId +
                ", sig=" + isSigModel +
                "}";
    }
}
