/********************************************************************************************************
 * @file MeshMessage.java
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
package com.telink.ble.mesh.core.message;

import com.telink.ble.mesh.core.networking.AccessLayerPDU;
import com.telink.ble.mesh.core.networking.AccessType;

/**
 * Created by kee on 2019/8/14.
 * This class represents a mesh message.
 */
public class MeshMessage {

    /**
     * access message
     */
    public static final int CTL_ACCESS = 0;

    /**
     * control message
     * never used by api
     */
    public static final int CTL_CONTROL = 1;

    /**
     * default retry count
     */
    public static final int DEFAULT_RETRY_CNT = 2;

    /**
     * default szmic
     */
    public static final int DEFAULT_ASZMIC = 0;

    /**
     * invalid opcode
     */
    public static final int OPCODE_INVALID = -1;

    public static final int DEFAULT_TTL = 10;

    /**
     * ms
     */
    public static final int DEFAULT_RETRY_INTERVAL = 1280;

    /**
     * message opcode
     * {@link AccessLayerPDU#opcode}
     */
    protected int opcode;

    /**
     * message params
     * {@link AccessLayerPDU#params}
     */
    protected byte[] params;

    /**
     * device key or appliction key determined by accessType {@link #accessType}
     */
    protected byte[] accessKey;

    /**
     * access typeValue
     *
     * @see AccessType#DEVICE for config message
     * @see AccessType#APPLICATION for common message
     */
    protected AccessType accessType = AccessType.APPLICATION;

    /**
     * only used when accessType==APPLICATION
     */
    protected int appKeyIndex;

    /**
     * 0: 4 bytes trans mic
     * 1: 8 bytes trans mic
     */
    protected int szmic = DEFAULT_ASZMIC;

    /**
     * source address
     * Will be assigned by local address
     */
    protected int sourceAddress;

    /**
     * dest address
     */
    protected int destinationAddress;

    protected int ctl = CTL_ACCESS;

    protected int ttl = DEFAULT_TTL;

    protected int retryCnt = DEFAULT_RETRY_CNT;

    protected long retryInterval = DEFAULT_RETRY_INTERVAL;

    protected int responseMax = 0;

    /**
     * if message contains tid
     *
     * @see com.telink.ble.mesh.core.networking.NetworkingController#tid
     */
//    protected boolean containsTid = false;


//    boolean reliable = false;

    /**
     * message response opcode
     * defines message is reliable {@link #isReliable()}
     */
    protected int responseOpcode = OPCODE_INVALID;

    /**
     * tid position in params
     * if message dose not contains tid, tid position should be invalid value (such as -1)
     */
    protected int tidPosition = -1;

    protected boolean isSegmented = false;

    /**
     * Checks if the message is reliable.
     * if true, message will be cached and checked completion by message response or retryCnt == 0
     * if false, message will not be cached and checked
     */
    public boolean isReliable() {
        return getResponseOpcode() != OPCODE_INVALID;
    }

    /**
     * Gets the response opcode of the message.
     *
     * @return the response opcode.
     */
    public int getResponseOpcode() {
        return responseOpcode;
    }

    /**
     * Sets the response opcode of the message.
     *
     * @param responseOpcode the response opcode to set.
     */
    public void setResponseOpcode(int responseOpcode) {
        this.responseOpcode = responseOpcode;
    }

    /**
     * Gets the position of the transaction identifier (tid) in the message parameters.
     *
     * @return the tid position.
     */
    public int getTidPosition() {
        return tidPosition;
    }

    /**
     * Sets the position of the transaction identifier (tid) in the message parameters.
     *
     * @param tidPosition the tid position to set.
     */
    public void setTidPosition(int tidPosition) {
        this.tidPosition = tidPosition;
    }

    /**
     * Gets the opcode of the message.
     *
     * @return the opcode.
     */
    public int getOpcode() {
        return opcode;
    }

    /**
     * Sets the opcode of the message.
     *
     * @param opcode the opcode to set.
     */
    public void setOpcode(int opcode) {
        this.opcode = opcode;
    }

    /**
     * Gets the parameters of the message.
     *
     * @return the parameters.
     */
    public byte[] getParams() {
        return params;
    }

    /**
     * Sets the parameters of the message.
     *
     * @param params the parameters to set.
     */
    public void setParams(byte[] params) {
        this.params = params;
    }

    /**
     * Gets the retry count for sending the message.
     *
     * @return the retry count.
     */
    public int getRetryCnt() {
        return retryCnt;
    }

    /**
     * Sets the retry count for sending the message.
     *
     * @param retryCnt the retry count to set.
     */
    public void setRetryCnt(int retryCnt) {
        this.retryCnt = retryCnt;
    }

    /**
     * Gets the maximum number of responses expected for the message.
     *
     * @return the maximum number of responses.
     */
    public int getResponseMax() {
        return responseMax;
    }

    /**
     * Sets the maximum number of responses expected for the message.
     *
     * @param responseMax the maximum number of responses to set.
     */
    public void setResponseMax(int responseMax) {
        this.responseMax = responseMax;
    }

    /**
     * Gets the access type of the message.
     *
     * @return the access type.
     */
    public AccessType getAccessType() {
        return accessType;
    }

    /**
     * Sets the access type of the message.
     *
     * @param accessType the access type to set.
     */
    public void setAccessType(AccessType accessType) {
        this.accessType = accessType;
    }

    /**
     * Gets the control type of the message.
     *
     * @return the control type.
     */
    public int getCtl() {
        return this.ctl;
    }

    /**
     * Sets the control type of the message.
     *
     * @param ctl the control type to set.
     * @deprecated This method is deprecated and should not be used.
     */
    public void setCtl(int ctl) {
        this.ctl = ctl;
    }

    /**
     * Gets the time to live value of the message.
     *
     * @return the time to live value.
     */
    public int getTtl() {
        return ttl;
    }

    /**
     * Sets the time to live value of the message.
     *
     * @param ttl the time to live value to set.
     */
    public void setTtl(int ttl) {
        this.ttl = ttl;
    }

    /**
     * Gets the szmic value of the message.
     *
     * @return the szmic value.
     */
    public int getSzmic() {
        return szmic;
    }

    /**
     * Sets the szmic value of the message.
     *
     * @param szmic the szmic value to set.
     */
    public void setSzmic(int szmic) {
        this.szmic = szmic;
    }

    /**
     * Gets the access key of the message.
     *
     * @return the access key.
     */
    public byte[] getAccessKey() {
        return accessKey;
    }

    /**
     * Sets the access key of the message.
     *
     * @param accessKey the access key to set.
     */
    public void setAccessKey(byte[] accessKey) {
        this.accessKey = accessKey;
    }

    /**
     * Gets the source address of the message.
     *
     * @return the source address.
     */
    public int getSourceAddress() {
        return sourceAddress;
    }

    /**
     * Sets the source address of the message.
     *
     * @param sourceAddress the source address to set.
     */
    public void setSourceAddress(int sourceAddress) {
        this.sourceAddress = sourceAddress;
    }

    /**
     * Returns the destination address of the message.
     *
     * @return the destination address
     */
    public int getDestinationAddress() {
        return destinationAddress;
    }

    /**
     * Sets the destination address of the message.
     *
     * @param destinationAddress the destination address to set
     */
    public void setDestinationAddress(int destinationAddress) {
        this.destinationAddress = destinationAddress;
    }

    /**
     * Returns the application key index of the message.
     *
     * @return the application key index
     */
    public int getAppKeyIndex() {
        return appKeyIndex;
    }

    /**
     * Sets the application key index of the message.
     *
     * @param appKeyIndex the application key index to set
     */
    public void setAppKeyIndex(int appKeyIndex) {
        this.appKeyIndex = appKeyIndex;
    }

    /**
     * Checks if the message is segmented.
     *
     * @return true if the message is segmented, false otherwise
     */
    public boolean isSegmented() {
        return isSegmented;
    }

    /**
     * Sets whether the message is segmented or not.
     *
     * @param segmented true if the message is segmented, false otherwise
     */
    public void setSegmented(boolean segmented) {
        isSegmented = segmented;
    }

    /**
     * Returns the retry interval of the message.
     *
     * @return the retry interval
     */
    public long getRetryInterval() {
        return retryInterval;
    }

    /**
     * Sets the retry interval of the message.
     *
     * @param retryInterval the retry interval to set
     */
    public void setRetryInterval(long retryInterval) {
        this.retryInterval = retryInterval;
    }
}
