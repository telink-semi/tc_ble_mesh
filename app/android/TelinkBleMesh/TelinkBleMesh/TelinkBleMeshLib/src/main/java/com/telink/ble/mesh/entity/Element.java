/********************************************************************************************************
 * @file Element.java
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

import android.os.Parcel;
import android.os.Parcelable;

import java.io.Serializable;
import java.util.List;

/**
 * Represents an element in a Bluetooth mesh network.
 * Implements the Serializable and Parcelable interfaces for object serialization and passing between activities.
 */
public class Element implements Serializable, Parcelable {
    /**
     * 2 bytes
     * Contains a location descriptor
     */
    public int location;

    /**
     * 1 byte
     * Contains a count of SIG Model IDs in this element
     */
    public int sigNum;

    /**
     * 1 byte
     * Contains a count of Vendor Model IDs in this element
     */
    public int vendorNum;

    /**
     * Contains a sequence of NumS SIG Model IDs
     */
    public List<Integer> sigModels;

    /**
     * Contains a sequence of NumV Vendor Model IDs
     */
    public List<Integer> vendorModels;

    /**
     * not in composition data, only for UI selection
     */

    public boolean selected;

    /**
     * Default constructor for the Element class.
     */
    public Element() {
    }

    /**
     * Parcelable constructor for the Element class.
     *
     * @param in The Parcel object containing the serialized Element data.
     */
    protected Element(Parcel in) {
        location = in.readInt();
        sigNum = in.readInt();
        vendorNum = in.readInt();
    }

    /**
     * Creator constant for the Parcelable interface.
     */
    public static final Creator<Element> CREATOR = new Creator<Element>() {
        @Override
        public Element createFromParcel(Parcel in) {
            return new Element(in);
        }

        @Override
        public Element[] newArray(int size) {
            return new Element[size];
        }
    };

    /**
     * Checks if the element contains a specific SIG Model ID.
     *
     * @param sigModelId The SIG Model ID to check for.
     * @return true if the element contains the specified SIG Model ID, false otherwise.
     */
    public boolean containModel(int sigModelId) {
        if (sigModels == null || sigModels.size() == 0) return false;
        for (int modelId : sigModels) {
            if (sigModelId == modelId) return true;
        }
        return false;
    }

    /**
     * Parcelable method for describing the contents of the Element object.
     *
     * @return 0 as there are no special contents to describe.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Parcelable method for writing the Element object to a Parcel.
     *
     * @param dest  The Parcel object to write the Element data to.
     * @param flags Additional flags for controlling the writing process.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(location);
        dest.writeInt(sigNum);
        dest.writeInt(vendorNum);
    }
}
