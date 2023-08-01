package com.telink.ble.mesh.entity;

import android.os.Parcel;
import android.os.Parcelable;

import java.io.Serializable;
import java.util.List;


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

    public Element() {
    }

    protected Element(Parcel in) {
        location = in.readInt();
        sigNum = in.readInt();
        vendorNum = in.readInt();
    }

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

    public boolean containModel(int sigModelId) {
        if (sigModels == null || sigModels.size() == 0) return false;
        for (int modelId : sigModels) {
            if (sigModelId == modelId) return true;
        }
        return false;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(location);
        dest.writeInt(sigNum);
        dest.writeInt(vendorNum);
    }
}
