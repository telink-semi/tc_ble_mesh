package com.telink.ble.mesh.model;


import java.io.Serializable;

/**
 * Created by kee on 2018/12/18.
 */
public class PublishModel implements Serializable {

    public static final int CREDENTIAL_FLAG_DEFAULT = 0b1;

    public static final int RFU_DEFAULT = 0x0C;


    // 采用固件端的默认配置
    public static final int TTL_DEFAULT = 0xFF;

    /**
     * default retransmit: 0x15
     * 0b 00010 101
     * count: 0x05, step: 0x02
     */
    public static final int RETRANSMIT_COUNT_DEFAULT = 0x05;


    public static final int RETRANSMIT_INTERVAL_STEP_DEFAULT = 0x02;



    public int elementAddress;

    public int modelId;

    public int address;

//    public byte[] params;

    public int period;

    public int ttl;

    public int credential;

    public int transmit;


    public PublishModel(int elementAddress, int modelId, int address, int period) {
        this(elementAddress, modelId, address, period, TTL_DEFAULT, CREDENTIAL_FLAG_DEFAULT,
                (byte) ((RETRANSMIT_COUNT_DEFAULT & 0b111) | (RETRANSMIT_INTERVAL_STEP_DEFAULT << 3)));
    }


    public PublishModel(int elementAddress, int modelId, int address, int period, int ttl, int credential, int transmit) {
        this.elementAddress = elementAddress;
        this.modelId = modelId;
        this.address = address;
        this.period = period;
        this.ttl = ttl;
        this.credential = credential;
        this.transmit = transmit;
    }

    // 高5
    public int getTransmitInterval() {
        return (transmit & 0xFF) >> 3;
    }

    // 低3
    public int getTransmitCount() {
        return (transmit & 0xFF) & 0b111;
    }


}
