package com.telink.ble.mesh.core.message.config;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/23.
 */

public class HeartbeatPublicationSetMessage extends ConfigMessage {

    private int destination;

    private byte countLog;

    private byte periodLog;

    private byte hbTtl;

    // 2 bytes
    private int features;

    // 2 bytes
    private int netKeyIndex;

    public HeartbeatPublicationSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return Opcode.HEARTBEAT_PUB_SET.value;
    }

    @Override
    public byte[] getParams() {
        ByteBuffer byteBuffer = ByteBuffer.allocate(9).order(ByteOrder.LITTLE_ENDIAN);
        byteBuffer.putShort((short) destination)
                .put(countLog)
                .put(periodLog)
                .put(hbTtl)
                .putShort((short) features)
                .putShort((short) netKeyIndex);
        return byteBuffer.array();
    }

    public void setDestination(int destination) {
        this.destination = destination;
    }

    public void setCountLog(byte countLog) {
        this.countLog = countLog;
    }

    public void setPeriodLog(byte periodLog) {
        this.periodLog = periodLog;
    }

    public void setHbTtl(byte hbTtl) {
        this.hbTtl = hbTtl;
    }

    public void setFeatures(int features) {
        this.features = features;
    }

    public void setNetKeyIndex(int netKeyIndex) {
        this.netKeyIndex = netKeyIndex;
    }
}
