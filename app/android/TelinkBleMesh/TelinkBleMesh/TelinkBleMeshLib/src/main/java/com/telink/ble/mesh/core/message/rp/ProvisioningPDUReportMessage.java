package com.telink.ble.mesh.core.message.rp;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;
import com.telink.ble.mesh.util.Arrays;


/**
 * Created by kee on 2019/8/20.
 */

public class ProvisioningPDUReportMessage extends StatusMessage implements Parcelable {

    private byte inboundPDUNumber;

    private byte[] provisioningPDU;

    public ProvisioningPDUReportMessage() {
    }


    protected ProvisioningPDUReportMessage(Parcel in) {
        inboundPDUNumber = in.readByte();
        provisioningPDU = in.createByteArray();
    }

    public static final Creator<ProvisioningPDUReportMessage> CREATOR = new Creator<ProvisioningPDUReportMessage>() {
        @Override
        public ProvisioningPDUReportMessage createFromParcel(Parcel in) {
            return new ProvisioningPDUReportMessage(in);
        }

        @Override
        public ProvisioningPDUReportMessage[] newArray(int size) {
            return new ProvisioningPDUReportMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        this.inboundPDUNumber = params[0];
        if (params.length > 1) {
            int pduLen = params.length - 1;
            provisioningPDU = new byte[pduLen];
            System.arraycopy(params, 1, this.provisioningPDU, 0, pduLen);
        }
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(inboundPDUNumber);
        dest.writeByteArray(provisioningPDU);
    }

    public byte getInboundPDUNumber() {
        return inboundPDUNumber;
    }

    public byte[] getProvisioningPDU() {
        return provisioningPDU;
    }

    @Override
    public String toString() {
        return "ProvisioningPDUReportMessage{" +
                "inboundPDUNumber=" + inboundPDUNumber +
                ", provisioningPDU=" + Arrays.bytesToHexString(provisioningPDU) +
                '}';
    }
}
