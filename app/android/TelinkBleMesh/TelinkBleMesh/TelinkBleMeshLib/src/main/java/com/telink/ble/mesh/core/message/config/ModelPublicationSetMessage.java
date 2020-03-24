package com.telink.ble.mesh.core.message.config;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.entity.ModelPublication;

/**
 * Created by kee on 2019/8/12.
 */

public class ModelPublicationSetMessage extends ConfigMessage {

    private ModelPublication modelPublication;

    public ModelPublicationSetMessage(int destinationAddress, ModelPublication modelPublication) {
        super(destinationAddress);
        this.modelPublication = modelPublication;
        this.responseOpcode = Opcode.CFG_MODEL_PUB_STATUS.value;
        this.responseMax = 1;
    }

    public ModelPublicationSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return Opcode.CFG_MODEL_PUB_SET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_MODEL_PUB_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return modelPublication.toBytes();
    }


}
