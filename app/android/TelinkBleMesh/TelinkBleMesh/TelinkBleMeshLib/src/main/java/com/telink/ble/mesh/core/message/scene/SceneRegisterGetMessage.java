package com.telink.ble.mesh.core.message.scene;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.lighting.LightingMessage;

/**
 * Created by kee on 2019/9/19.
 */

public class SceneRegisterGetMessage extends LightingMessage {

    public static SceneRegisterGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        SceneRegisterGetMessage message = new SceneRegisterGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    public SceneRegisterGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.SCENE_REG_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.SCENE_REG_STATUS.value;
    }
}
