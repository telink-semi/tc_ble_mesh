/********************************************************************************************************
 * @file FDCapabilitiesStatusMessage.java
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
package com.telink.ble.mesh.core.message.firmwaredistribution;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * Firmware Distribution Capabilities Status
 * response to a Firmware Distributor Capabilities Get message
 *
 * @see FDCapabilitiesGetMessage
 */
public class FDCapabilitiesStatusMessage extends StatusMessage implements Parcelable {

    /**
     * Max Distribution Receivers List Size
     * Maximum number of entries in the Distribution Receivers List state
     * 2 bytes
     */
    private int maxReceiversListSize;

    /**
     * Max Firmware Images List Size
     * Maximum number of entries in the Firmware Images List state
     * 2 bytes
     */
    public int maxImagesListSize;

    /**
     * Max Firmware Image Size
     * Maximum size of one firmware image (in octets)
     * 4 bytes
     */
    private int maxImageSize;

    /**
     * Max Upload Space
     * Total space dedicated to storage of firmware images (in octets)
     * 4 bytes
     */
    private int maxUploadSpace;

    /**
     * Remaining Upload Space
     * Remaining available space in firmware image storage (in octets)
     * 4 bytes
     */
    private int remainingUploadSpace;


    /**
     * Out-of-Band Retrieval Supported
     * Value of the Out-of-Band Retrieval Supported state
     * 1 byte
     */
    private int oobRetrievalSupported;

    /**
     * Supported URI Scheme Names
     * Value of the Supported URI Scheme Names state
     * Variable
     */
    private byte[] uriSchemeNames;


    /**
     * Default constructor for the FDCapabilitiesStatusMessage class.
     */
    public FDCapabilitiesStatusMessage() {
    }

    /**
     * Constructor for the FDCapabilitiesStatusMessage class that takes a Parcel as input.
     * Used for deserialization.
     *
     * @param in The Parcel to read the values from.
     */
    protected FDCapabilitiesStatusMessage(Parcel in) {
        maxReceiversListSize = in.readInt();
        maxImagesListSize = in.readInt();
        maxImageSize = in.readInt();
        maxUploadSpace = in.readInt();
        remainingUploadSpace = in.readInt();
        oobRetrievalSupported = in.readInt();
        uriSchemeNames = in.createByteArray();
    }

    /**
     * Creator object for the FDCapabilitiesStatusMessage class.
     * Used for parceling.
     */
    public static final Creator<FDCapabilitiesStatusMessage> CREATOR = new Creator<FDCapabilitiesStatusMessage>() {
        @Override
        public FDCapabilitiesStatusMessage createFromParcel(Parcel in) {
            return new FDCapabilitiesStatusMessage(in);
        }

        @Override
        public FDCapabilitiesStatusMessage[] newArray(int size) {
            return new FDCapabilitiesStatusMessage[size];
        }
    };

    /**
     * Parses the byte array representation of the FDCapabilitiesStatusMessage object.
     *
     * @param params The byte array containing the values to parse.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.maxReceiversListSize = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        this.maxImagesListSize = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        maxImageSize = MeshUtils.bytes2Integer(params, index, 4, ByteOrder.LITTLE_ENDIAN);
        index += 4;
        maxUploadSpace = MeshUtils.bytes2Integer(params, index, 4, ByteOrder.LITTLE_ENDIAN);
        index += 4;
        remainingUploadSpace = MeshUtils.bytes2Integer(params, index, 4, ByteOrder.LITTLE_ENDIAN);
        index += 4;
        oobRetrievalSupported = params[index++] & 0xFF;
        if (oobRetrievalSupported == DistributorCapabilities.OOBRetrievalSupported.SUPPORTED.value) {
            uriSchemeNames = new byte[params.length - index];
            System.arraycopy(params, index, uriSchemeNames, 0, uriSchemeNames.length);
        } else {
            uriSchemeNames = null;
        }
    }

    /**
     * Gets the maximum number of entries in the Distribution Receivers List state.
     *
     * @return The maximum number of entries in the Distribution Receivers List state.
     */
    public int getMaxReceiversListSize() {
        return maxReceiversListSize;
    }

    /**
     * Sets the maximum number of entries in the Distribution Receivers List state.
     *
     * @param maxReceiversListSize The maximum number of entries in the Distribution Receivers List state.
     */
    public void setMaxReceiversListSize(int maxReceiversListSize) {
        this.maxReceiversListSize = maxReceiversListSize;
    }

    /**
     * Gets the maximum number of entries in the Firmware Images List state.
     *
     * @return The maximum number of entries in the Firmware Images List state.
     */
    public int getMaxImagesListSize() {
        return maxImagesListSize;
    }

    /**
     * Sets the maximum number of entries in the Firmware Images List state.
     *
     * @param maxImagesListSize The maximum number of entries in the Firmware Images List state.
     */
    public void setMaxImagesListSize(int maxImagesListSize) {
        this.maxImagesListSize = maxImagesListSize;
    }

    /**
     * Gets the maximum size of one firmware image (in octets).
     *
     * @return The maximum size of one firmware image (in octets).
     */
    public int getMaxImageSize() {
        return maxImageSize;
    }

    /**
     * Sets the maximum size of one firmware image (in octets).
     *
     * @param maxImageSize The maximum size of one firmware image (in octets).
     */
    public void setMaxImageSize(int maxImageSize) {
        this.maxImageSize = maxImageSize;
    }

    /**
     * Gets the total space dedicated to storage of firmware images (in octets).
     *
     * @return The total space dedicated to storage of firmware images (in octets).
     */
    public int getMaxUploadSpace() {
        return maxUploadSpace;
    }

    /**
     * Sets the total space dedicated to storage of firmware images (in octets).
     *
     * @param maxUploadSpace The total space dedicated to storage of firmware images (in octets).
     */
    public void setMaxUploadSpace(int maxUploadSpace) {
        this.maxUploadSpace = maxUploadSpace;
    }

    /**
     * Gets the remaining available space in firmware image storage (in octets).
     *
     * @return The remaining available space in firmware image storage (in octets).
     */
    public int getRemainingUploadSpace() {
        return remainingUploadSpace;
    }

    /**
     * Sets the remaining available space in firmware image storage (in octets).
     *
     * @param remainingUploadSpace The remaining available space in firmware image storage (in octets).
     */
    public void setRemainingUploadSpace(int remainingUploadSpace) {
        this.remainingUploadSpace = remainingUploadSpace;
    }

    /**
     * Gets the value indicating whether out-of-band retrieval is supported.
     *
     * @return The value indicating whether out-of-band retrieval is supported.
     */
    public int getOobRetrievalSupported() {
        return oobRetrievalSupported;
    }

    /**
     * Sets the value indicating whether out-of-band retrieval is supported.
     *
     * @param oobRetrievalSupported The value indicating whether out-of-band retrieval is supported.
     */
    public void setOobRetrievalSupported(int oobRetrievalSupported) {
        this.oobRetrievalSupported = oobRetrievalSupported;
    }

    /**
     * Gets the supported URI scheme names.
     *
     * @return The supported URI scheme names.
     */
    public byte[] getUriSchemeNames() {
        return uriSchemeNames;
    }

    /**
     * Sets the supported URI scheme names.
     *
     * @param uriSchemeNames The supported URI scheme names.
     */
    public void setUriSchemeNames(byte[] uriSchemeNames) {
        this.uriSchemeNames = uriSchemeNames;
    }

    /**
     * Returns a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     *
     * @return Always returns 0.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Flattens the object into a Parcel.
     *
     * @param dest  The Parcel in which the object should be written.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(maxReceiversListSize);
        dest.writeInt(maxImagesListSize);
        dest.writeInt(maxImageSize);
        dest.writeInt(maxUploadSpace);
        dest.writeInt(remainingUploadSpace);
        dest.writeInt(oobRetrievalSupported);
        dest.writeByteArray(uriSchemeNames);
    }
}