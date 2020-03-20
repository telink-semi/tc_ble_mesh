package com.telink.ble.mesh.core.message.rp;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * Created by kee on 2019/8/20.
 */

public class ProvisioningPDUOutboundReportMessage extends StatusMessage implements Parcelable {

    private byte outboundPDUNumber;


    public ProvisioningPDUOutboundReportMessage() {
    }


    protected ProvisioningPDUOutboundReportMessage(Parcel in) {
        outboundPDUNumber = in.readByte();
    }

    public static final Creator<ProvisioningPDUOutboundReportMessage> CREATOR = new Creator<ProvisioningPDUOutboundReportMessage>() {
        @Override
        public ProvisioningPDUOutboundReportMessage createFromParcel(Parcel in) {
            return new ProvisioningPDUOutboundReportMessage(in);
        }

        @Override
        public ProvisioningPDUOutboundReportMessage[] newArray(int size) {
            return new ProvisioningPDUOutboundReportMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        this.outboundPDUNumber = params[0];
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(outboundPDUNumber);
    }

    public byte getOutboundPDUNumber() {
        return outboundPDUNumber;
    }
}
