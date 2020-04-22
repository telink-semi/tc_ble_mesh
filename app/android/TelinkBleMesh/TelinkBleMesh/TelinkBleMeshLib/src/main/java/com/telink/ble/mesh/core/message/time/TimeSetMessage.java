package com.telink.ble.mesh.core.message.time;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * time set
 * Created by kee on 2019/8/14.
 */
public class TimeSetMessage extends GenericMessage {

    /**
     * TAI seconds
     * 40 bits
     * The current TAI time in seconds
     */
    private long taiSeconds;

    /**
     * 8 bits
     * The sub-second time in units of 1/256th second
     */
    private byte subSecond;

    /**
     * 8 bits
     * The estimated uncertainty in 10 millisecond steps
     */
    private byte uncertainty;

    /**
     * 1 bit
     * 0 = No Time Authority, 1 = Time Authority
     */
    private byte timeAuthority;

    /**
     * TAI-UTC Delta
     * 15 bits
     * Current difference between TAI and UTC in seconds
     */
    private int delta;

    /**
     * Time Zone Offset
     * 8 bits
     * The local time zone offset in 15-minute increments
     */
    private int zoneOffset;

    public static TimeSetMessage getSimple(int address, int appKeyIndex, long taiSeconds, int zoneOffset, int rspMax) {
        TimeSetMessage message = new TimeSetMessage(address, appKeyIndex);
        message.taiSeconds = taiSeconds;
        message.zoneOffset = zoneOffset;
        message.setResponseMax(rspMax);
        return message;
    }

    public TimeSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.TIME_STATUS.value;
    }

    @Override
    public int getOpcode() {
        return Opcode.TIME_SET.value;
    }

    @Override
    public byte[] getParams() {
        ByteBuffer byteBuffer = ByteBuffer.allocate(10).order(ByteOrder.LITTLE_ENDIAN);
        byteBuffer.put((byte) (taiSeconds))
                .put((byte) (taiSeconds >> 8))
                .put((byte) (taiSeconds >> 16))
                .put((byte) (taiSeconds >> 24))
                .put((byte) (taiSeconds >> 32))
                .put(subSecond)
                .put(uncertainty)
                .putShort((short) ((delta << 1) | timeAuthority))
                .put((byte) (zoneOffset));
        return byteBuffer.array();
    }

    public void setTaiSeconds(long taiSeconds) {
        this.taiSeconds = taiSeconds;
    }

    public void setSubSecond(byte subSecond) {
        this.subSecond = subSecond;
    }

    public void setUncertainty(byte uncertainty) {
        this.uncertainty = uncertainty;
    }

    public void setTimeAuthority(byte timeAuthority) {
        this.timeAuthority = timeAuthority;
    }

    public void setDelta(int delta) {
        this.delta = delta;
    }

    public void setZoneOffset(int zoneOffset) {
        this.zoneOffset = zoneOffset;
    }
}
