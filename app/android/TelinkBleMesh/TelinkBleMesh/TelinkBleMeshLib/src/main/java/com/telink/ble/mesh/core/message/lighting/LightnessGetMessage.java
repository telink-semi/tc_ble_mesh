package com.telink.ble.mesh.core.message.lighting;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.OnOffGetMessage;

/**
 * Created by kee on 2019/9/19.
 */

public class LightnessGetMessage extends LightingMessage {

    public static LightnessGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        LightnessGetMessage message = new LightnessGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    public LightnessGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.LIGHTNESS_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.LIGHTNESS_STATUS.value;
    }
}
