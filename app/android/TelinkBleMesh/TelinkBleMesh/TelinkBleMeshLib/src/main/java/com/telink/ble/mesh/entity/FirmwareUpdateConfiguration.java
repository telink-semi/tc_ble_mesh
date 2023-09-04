/********************************************************************************************************
 * @file FirmwareUpdateConfiguration.java
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
package com.telink.ble.mesh.entity;

import com.telink.ble.mesh.core.access.fu.BlobTransferType;
import com.telink.ble.mesh.core.access.fu.DistributorType;
import com.telink.ble.mesh.core.access.fu.FUCallback;
import com.telink.ble.mesh.core.access.fu.FUState;
import com.telink.ble.mesh.core.access.fu.UpdatePolicy;
import com.telink.ble.mesh.core.networking.ExtendBearerMode;

import java.util.Arrays;
import java.util.List;

/**
 * Created by kee on 2019/9/6.
 */

public class FirmwareUpdateConfiguration {

    /**
     * target devices for updating
     */
    private List<MeshUpdatingDevice> updatingDevices;

    /**
     * firmware data, will be null if isContinue is true
     */
    private byte[] firmwareData;


    /**
     * metadata
     */
    private byte[] metadata;

    /**
     * application key index
     */
    private int appKeyIndex;

    /**
     * group address for subscription message
     */
    private int groupAddress;

//    private int companyId = 0x0211;

//    private int firmwareId = 0xFF000021;

    /**
     * blob id
     */
    private long blobId = 0x8877665544332211L;


    /**
     * firmware id
     */
    private byte[] firmwareId = new byte[]{0x01, 0x02, 0x03, 0x04};

    /**
     * firmware index
     */
    private int firmwareIndex = 0;


    private ExtendBearerMode extendBearerMode;

    /**
     * 0 means using APP as distributor,
     * otherwise use direct connected device
     */
    private DistributorType distributorType = DistributorType.PHONE;

    private FUCallback callback;

    private UpdatePolicy updatePolicy = UpdatePolicy.VerifyAndApply;

    /**
     * is continue:
     */
    private boolean isContinue = false;

    /**
     * only used when isContinue is true
     */
    private int distributorAddress = 0;

    /**
     * direct connected address
     */
    private int proxyAddress;


    public FirmwareUpdateConfiguration(List<MeshUpdatingDevice> updatingDevices,
                                       byte[] firmwareData,
                                       byte[] metadata,
                                       int appKeyIndex,
                                       int groupAddress) {
        this.updatingDevices = updatingDevices;
        this.firmwareData = firmwareData;
        this.metadata = metadata;
        this.appKeyIndex = appKeyIndex;
        this.groupAddress = groupAddress;
    }

    public List<MeshUpdatingDevice> getUpdatingDevices() {
        return updatingDevices;
    }

    public byte[] getFirmwareData() {
        return firmwareData;
    }

    public int getAppKeyIndex() {
        return appKeyIndex;
    }

    public int getGroupAddress() {
        return groupAddress;
    }

    public long getBlobId() {
        return blobId;
    }

    public ExtendBearerMode getExtendBearerMode() {
        return extendBearerMode;
    }

    public void setExtendBearerMode(ExtendBearerMode extendBearerMode) {
        this.extendBearerMode = extendBearerMode;
    }

    public byte[] getMetadata() {
        return metadata;
    }

    public byte[] getFirmwareId() {
        return firmwareId;
    }

    public void setFirmwareId(byte[] firmwareId) {
        this.firmwareId = firmwareId;
    }

    public int getFirmwareIndex() {
        return firmwareIndex;
    }

    public void setFirmwareIndex(int firmwareIndex) {
        this.firmwareIndex = firmwareIndex;
    }

    public FUCallback getCallback() {
        return callback;
    }

    public void setCallback(FUCallback callback) {
        this.callback = callback;
    }

    public UpdatePolicy getUpdatePolicy() {
        return updatePolicy;
    }

    public DistributorType getDistributorType() {
        return distributorType;
    }

    public void setDistributorType(DistributorType distributorType) {
        this.distributorType = distributorType;
    }

    public void setUpdatePolicy(UpdatePolicy updatePolicy) {
        this.updatePolicy = updatePolicy;
    }

    public boolean isContinue() {
        return isContinue;
    }

    public void setContinue(boolean aContinue) {
        isContinue = aContinue;
    }

    public int getDistributorAddress() {
        return distributorAddress;
    }

    public void setDistributorAddress(int distributorAddress) {
        this.distributorAddress = distributorAddress;
    }

    public void dispatchTransferProgress(int progress, BlobTransferType transferType) {
        if (this.callback != null) {
            this.callback.onTransferProgress(progress, transferType);
        }
    }

    public void dispatchLogInfo(String tag, String log, int logLevel) {
        if (this.callback != null) {
            this.callback.onLog(tag, log, logLevel);
        }
    }

    public void dispatchFUState(FUState fuState, String info) {
        if (this.callback != null) {
            this.callback.onStateUpdated(fuState, info);
        }
    }

    public void dispatchDeviceState(MeshUpdatingDevice updatingDevice, String desc) {
        if (this.callback != null) {
            this.callback.onDeviceStateUpdate(updatingDevice, desc);
        }
    }

    public int getProxyAddress() {
        return proxyAddress;
    }

    public void setProxyAddress(int proxyAddress) {
        this.proxyAddress = proxyAddress;
    }

    @Override
    public String toString() {
        return "FirmwareUpdateConfiguration{" +
                "updatingDevices=" + updatingDevices.size() +
                ", firmwareData=" + (firmwareData == null ? 0 : firmwareData.length) +
                ", metadata=" + Arrays.toString(metadata) +
                ", appKeyIndex=" + appKeyIndex +
                ", groupAddress=" + groupAddress +
                ", blobId=" + blobId +
                ", firmwareId=" + Arrays.toString(firmwareId) +
                ", firmwareIndex=" + firmwareIndex +
                ", distributorType=" + distributorType +
                ", callback=" + callback +
                ", updatePolicy=" + updatePolicy +
                ", isContinue=" + isContinue +
                ", distributorAddress=" + distributorAddress +
                ", proxyAddress=" + proxyAddress +
                '}';
    }
}
