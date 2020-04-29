package com.telink.ble.mesh.core.message.lighting;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/20.
 */

public class LightnessStatusMessage extends StatusMessage implements Parcelable {

    private static final int DATA_LEN_COMPLETE = 5;

    private int presentLightness;

    /**
     * The target value of the Lightness state (optional).
     */
    private int targetLightness;

    private byte remainingTime;

    /**
     * tag of is complete message
     */
    private boolean isComplete = false;

    public LightnessStatusMessage() {
    }

    protected LightnessStatusMessage(Parcel in) {
        presentLightness = in.readInt();
        targetLightness = in.readInt();
        remainingTime = in.readByte();
        isComplete = in.readByte() != 0;
    }

    public static final Creator<LightnessStatusMessage> CREATOR = new Creator<LightnessStatusMessage>() {
        @Override
        public LightnessStatusMessage createFromParcel(Parcel in) {
            return new LightnessStatusMessage(in);
        }

        @Override
        public LightnessStatusMessage[] newArray(int size) {
            return new LightnessStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        this.presentLightness = MeshUtils.bytes2Integer(params, 0, 2, ByteOrder.LITTLE_ENDIAN);
        if (params.length == DATA_LEN_COMPLETE) {
            this.isComplete = true;
            this.targetLightness = MeshUtils.bytes2Integer(params, 2, 2, ByteOrder.LITTLE_ENDIAN);
            this.remainingTime = params[4];
        }
    }

    public int getPresentLightness() {
        return presentLightness;
    }

    public int getTargetLightness() {
        return targetLightness;
    }

    public byte getRemainingTime() {
        return remainingTime;
    }

    public boolean isComplete() {
        return isComplete;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(presentLightness);
        dest.writeInt(targetLightness);
        dest.writeByte(remainingTime);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }
}
