package com.telink.ble.mesh.core.message.lighting;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * There is no target info in status message
 *
 * @see HslTargetStatusMessage
 * Created by kee on 2019/8/20.
 */
public class HslStatusMessage extends StatusMessage implements Parcelable {

    private static final int DATA_LEN_COMPLETE = 7;

    private int lightness;

    private int hue;

    private int saturation;

    private byte remainingTime;

    /**
     * tag of is complete message
     */
    private boolean isComplete = false;

    public HslStatusMessage() {
    }

    protected HslStatusMessage(Parcel in) {
        lightness = in.readInt();
        hue = in.readInt();
        saturation = in.readInt();
        remainingTime = in.readByte();
        isComplete = in.readByte() != 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(lightness);
        dest.writeInt(hue);
        dest.writeInt(saturation);
        dest.writeByte(remainingTime);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }

    @Override
    public int describeContents() {
        return 0;
    }

    public static final Creator<HslStatusMessage> CREATOR = new Creator<HslStatusMessage>() {
        @Override
        public HslStatusMessage createFromParcel(Parcel in) {
            return new HslStatusMessage(in);
        }

        @Override
        public HslStatusMessage[] newArray(int size) {
            return new HslStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.lightness = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        this.hue = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        this.saturation = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        if (params.length == DATA_LEN_COMPLETE) {
            this.isComplete = true;
            remainingTime = params[index];
        }
    }

    public int getLightness() {
        return lightness;
    }

    public int getHue() {
        return hue;
    }

    public int getSaturation() {
        return saturation;
    }

    public byte getRemainingTime() {
        return remainingTime;
    }

    public boolean isComplete() {
        return isComplete;
    }
}
