/********************************************************************************************************
 * @file ModelPublicationGetMessage.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *
 *******************************************************************************************************/
package com.telink.ble.mesh.core.message.config;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * The Config Model Publication Get is an acknowledged message used to get the publish address and parameters of an outgoing message that originates from a model.
 * The response to a Config Model Publication Get message is a Config Model Publication Status message.
 *
 * @see ModelPublicationStatusMessage
 */
public class ModelPublicationGetMessage extends ConfigMessage {


    /**
     * Address of the element
     * 16 bits
     */
    public int elementAddress;

    /**
     * 16 or 32 bits
     */
    public int modelId;

    /**
     * if sig model
     * #modelId
     */
    public boolean sig = true;

    public static ModelPublicationGetMessage getSimple(int destinationAddress, int elementAddress, int modelId) {
        ModelPublicationGetMessage message = new ModelPublicationGetMessage(destinationAddress);
        message.elementAddress = elementAddress;
        message.modelId = modelId;
        return message;
    }

    public ModelPublicationGetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return Opcode.CFG_MODEL_PUB_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_MODEL_PUB_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        int len = sig ? 4 : 6;
        ByteBuffer bf = ByteBuffer.allocate(len).order(ByteOrder.LITTLE_ENDIAN);
        bf.putShort((short) elementAddress);
        if (sig) {
            bf.putShort((short) modelId);
        } else {
            bf.putInt(modelId);
        }
        return bf.array();
    }

}
