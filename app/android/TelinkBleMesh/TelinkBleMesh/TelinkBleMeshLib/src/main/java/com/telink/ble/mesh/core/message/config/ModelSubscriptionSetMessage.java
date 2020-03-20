package com.telink.ble.mesh.core.message.config;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * grouping
 * Created by kee on 2019/8/12.
 */

public class ModelSubscriptionSetMessage extends ConfigMessage {

    private static final int PARAM_LEN_SIG = 6;

    private static final int PARAM_LEN_VENDOR = 8;

    public static final int MODE_ADD = 0;

    public static final int MODE_DELETE = 1;

    private int mode = MODE_ADD;

    private int elementAddress;

    /**
     * group address
     */
    private int address;

    /**
     * 2 or 4 bytes
     * determined by sig
     */
    private int modelIdentifier;

    /**
     * is sig or vendor
     */
    private boolean isSig = true;


    public static ModelSubscriptionSetMessage getSimple(int destinationAddress, int mode, int elementAddress, int groupAddress, int modelId, boolean isSig) {
        ModelSubscriptionSetMessage message = new ModelSubscriptionSetMessage(destinationAddress);
        message.mode = mode;
        message.elementAddress = elementAddress;
        message.address = groupAddress;
        message.modelIdentifier = modelId;
        message.isSig = isSig;
        return message;
    }

    public ModelSubscriptionSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return mode == MODE_ADD ? Opcode.CFG_MODEL_SUB_ADD.value : Opcode.CFG_MODEL_SUB_DEL.value ;
    }


    @Override
    public byte[] getParams() {
        if (isSig) {
            return ByteBuffer.allocate(PARAM_LEN_SIG).order(ByteOrder.LITTLE_ENDIAN)
                    .putShort((short) elementAddress)
                    .putShort((short) address)
                    .putShort((short) modelIdentifier)
                    .array();
        } else {
            return ByteBuffer.allocate(PARAM_LEN_VENDOR).order(ByteOrder.LITTLE_ENDIAN)
                    .putShort((short) elementAddress)
                    .putShort((short) address)
                    .putInt(modelIdentifier)
                    .array();
        }

    }

    public void setMode(int mode) {
        this.mode = mode;
    }

    public void setElementAddress(int elementAddress) {
        this.elementAddress = elementAddress;
    }

    public void setAddress(int address) {
        this.address = address;
    }

    public void setModelIdentifier(int modelIdentifier) {
        this.modelIdentifier = modelIdentifier;
    }

    public void setSig(boolean sig) {
        isSig = sig;
    }
}
