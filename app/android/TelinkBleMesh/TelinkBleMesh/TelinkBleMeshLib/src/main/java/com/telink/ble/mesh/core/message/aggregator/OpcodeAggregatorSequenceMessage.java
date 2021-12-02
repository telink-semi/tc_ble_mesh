package com.telink.ble.mesh.core.message.aggregator;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdatingMessage;
import com.telink.ble.mesh.core.networking.AccessLayerPDU;
import com.telink.ble.mesh.core.networking.AccessType;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;


/**
 * An OPCODES_AGGREGATOR_SEQUENCE message is an acknowledged message
 * that the Opcodes Aggregator Client uses to encapsulate a sequence of access messages to be processed by the Opcodes Aggregator Server.
 * <p>
 * The response to an OPCODES_AGGREGATOR_SEQUENCE message is an OPCODES_AGGREGATOR_STATUS message.
 *
 * @see OpcodeAggregatorStatusMessage
 */
public class OpcodeAggregatorSequenceMessage extends MeshMessage {


//    public int elementAddress;
//    private List<MeshMessage> messageList;
//    public List<AggregatorItem> items;

    /**
     * @param accessType  message type, defines the encryption key
     * @param appKeyIndex used when access type is AccessType.APPLICATION
     */
    public OpcodeAggregatorSequenceMessage(int destinationAddress,
                                           AccessType accessType,
                                           int appKeyIndex,
                                           byte[] params
    ) {
        this.destinationAddress = destinationAddress;
        this.accessType = accessType;
        this.appKeyIndex = appKeyIndex;
        this.params = params;
    }


    @Override
    public int getOpcode() {
        return Opcode.OP_AGGREGATOR_SEQUENCE.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.OP_AGGREGATOR_STATUS.value;
    }

}
