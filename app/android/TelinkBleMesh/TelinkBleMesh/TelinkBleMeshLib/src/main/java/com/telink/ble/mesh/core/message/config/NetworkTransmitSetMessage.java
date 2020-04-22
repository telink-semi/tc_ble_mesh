package com.telink.ble.mesh.core.message.config;

import com.telink.ble.mesh.core.message.Opcode;

/**
 * set network transmit params
 * Network transmit params are used when node sending network pdu from self -- source address is self --
 * Meanwhile relay params are used when relay network pdu, generally smaller than network transmit
 * Created by kee on 2020/03/20.
 */

public class NetworkTransmitSetMessage extends ConfigMessage {


    // networkTransmitCount, default is 5
    private int count;


    // networkTransmitIntervalSteps, default is 2
    // transmission interval = (Network Transmit Interval Steps + 1) * 10
    private int intervalSteps;

    public NetworkTransmitSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    public void setCount(int count) {
        this.count = count;
    }

    public void setIntervalSteps(int intervalSteps) {
        this.intervalSteps = intervalSteps;
    }

    @Override
    public int getOpcode() {
        return Opcode.CFG_NW_TRANSMIT_SET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_NW_TRANSMIT_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return new byte[]{
                (byte) ((count & 0b111) | (intervalSteps << 3))
        };
    }


}
