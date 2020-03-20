package com.telink.ble.mesh.core.message.rp;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.updating.UpdatingMessage;

import java.nio.ByteBuffer;

public class ProvisioningPduSendMessage extends UpdatingMessage {

    byte outboundPDUNumber;

    /**
     * 16 bytes
     */
    private byte[] provisioningPDU;

    /**
     * @param destinationAddress server address
     */
    public static ProvisioningPduSendMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax,
                                                       byte outboundPDUNumber,
                                                       byte[] provisioningPDU) {
        ProvisioningPduSendMessage message = new ProvisioningPduSendMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        message.outboundPDUNumber = outboundPDUNumber;
        message.provisioningPDU = provisioningPDU;
        return message;
    }

    public ProvisioningPduSendMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.REMOTE_PROV_PDU_SEND.value;
    }

    @Override
    public int getResponseOpcode() {
//        return OPCODE_INVALID;
        return Opcode.REMOTE_PROV_PDU_OUTBOUND_REPORT.value;
    }

    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(1 + provisioningPDU.length)
                .put(outboundPDUNumber)
                .put(provisioningPDU).array();
    }

}
