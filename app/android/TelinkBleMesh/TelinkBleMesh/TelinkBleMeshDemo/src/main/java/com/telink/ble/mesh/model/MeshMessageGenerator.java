package com.telink.ble.mesh.model;

import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.config.HeartbeatPublicationSetMessage;
import com.telink.ble.mesh.core.message.generic.OnOffSetMessage;

/**
 * Generate sample mesh message
 *
 * @see com.telink.ble.mesh.foundation.MeshService#sendMeshMessage(MeshMessage)
 * Created by kee on 2019/9/16.
 */

public class MeshMessageGenerator {

    /********************* generic *********************/

    public static MeshMessage onOffSetMsg(int nodeAddress, byte onOff, int appKeyIndex) {
        OnOffSetMessage message = new OnOffSetMessage(nodeAddress, appKeyIndex);
        message.onOff = onOff;
        message.setContainsTid(true);
        message.setResponseOpcode(Opcode.G_ONOFF_STATUS.value);
        return message;
    }


    /********************* configuration *********************/

    public static MeshMessage heartbeatPubSetMsg(int nodeAdr, int netKeyIndex, int hbTargetAddress) {
        HeartbeatPublicationSetMessage message = new HeartbeatPublicationSetMessage(nodeAdr);
        message.setCountLog((byte) 0xFF);
        message.setPeriodLog((byte) 0x02);
        message.setHbTtl((byte) 0x01);
        message.setFeatures(0b0111);
        message.setNetKeyIndex(netKeyIndex);
        message.setDestination(hbTargetAddress);
        return message;
    }


}
