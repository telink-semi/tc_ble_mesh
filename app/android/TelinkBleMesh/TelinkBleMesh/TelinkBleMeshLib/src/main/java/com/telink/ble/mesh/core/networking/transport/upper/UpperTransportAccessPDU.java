/********************************************************************************************************
 * @file UpperTransportAccessPDU.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date     Sep. 30, 2017
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/
package com.telink.ble.mesh.core.networking.transport.upper;

import android.util.SparseArray;

import com.telink.ble.mesh.core.Encipher;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.networking.AccessType;
import com.telink.ble.mesh.core.networking.NonceGenerator;
import com.telink.ble.mesh.core.networking.transport.lower.SegmentedAccessMessagePDU;
import com.telink.ble.mesh.core.networking.transport.lower.UnsegmentedAccessMessagePDU;
import com.telink.ble.mesh.util.MeshLogger;

import java.nio.ByteOrder;
import java.util.List;

/**
 * Created by kee on 2019/8/9.
 */

public class UpperTransportAccessPDU {
    /**
     * 384 bytes total
     * 4 or 8 bytes transMIC
     * transMIC
     * for unsegmented message: 4 bytes
     * for segmented message, determined by lower transport pdu:
     * 4 bytes if SZMIC == 0, 8 bytes if SZMIC == 1
     */
    private byte[] encryptedPayload;


    private byte[] decryptedPayload;

    private UpperTransportEncryptionSuite mEncryptionSuite;

    public UpperTransportAccessPDU(UpperTransportEncryptionSuite mEncryptionSuite) {
        this.mEncryptionSuite = mEncryptionSuite;
    }

    public byte[] getEncryptedPayload() {
        return encryptedPayload;
    }

    public byte[] getDecryptedPayload() {
        return decryptedPayload;
    }


    public boolean parseAndDecryptSegmentedMessage(SparseArray<SegmentedAccessMessagePDU> messageBuffer, int sequenceNumber, int src, int dst) {
        int len = 0;
        for (int i = 0; i < messageBuffer.size(); i++) {
            len += messageBuffer.get(i).getSegmentM().length;
        }

        byte[] upperTransportPdu = new byte[len];
        int idx = 0;
        int tmpLen;
        for (int i = 0; i < messageBuffer.size(); i++) {
            tmpLen = messageBuffer.get(i).getSegmentM().length;
            System.arraycopy(messageBuffer.get(i).getSegmentM(), 0, upperTransportPdu, idx, tmpLen);
            idx += tmpLen;
        }

//        MeshLogger.log("upper pdu raw: " + Arrays.bytesToHexString(upperTransportPdu, ""));
        this.encryptedPayload = upperTransportPdu;

        SegmentedAccessMessagePDU message0 = messageBuffer.get(0);
        this.decryptedPayload = decrypt(message0.getAkf(), message0.getAid(), message0.getSzmic(), sequenceNumber, src, dst);

        return this.decryptedPayload != null;
    }

    public boolean parseAndDecryptUnsegmentedMessage(UnsegmentedAccessMessagePDU unsegmentedAccessMessagePDU, int sequenceNumber, int src, int dst) {
        this.encryptedPayload = unsegmentedAccessMessagePDU.getUpperTransportPDU();
        this.decryptedPayload = decrypt(unsegmentedAccessMessagePDU.getAkf(), unsegmentedAccessMessagePDU.getAid(), 0, sequenceNumber, src, dst);
        return this.decryptedPayload != null;
    }

    public boolean encrypt(byte[] accessPduData, byte szmic, AccessType accessType, int seqNo, int src, int dst) {
        this.decryptedPayload = accessPduData;
        byte[] seqNoBuffer = MeshUtils.integer2Bytes(seqNo, 3, ByteOrder.BIG_ENDIAN);
        byte[] nonce = NonceGenerator.generateAccessNonce(szmic, seqNoBuffer, src, dst, this.mEncryptionSuite.ivIndex, accessType);
        int mic = MeshUtils.getMicSize(szmic);
        byte[] key;
        if (accessType == AccessType.APPLICATION) {
            key = this.mEncryptionSuite.appKeyList.get(0);
        } else {
            key = this.mEncryptionSuite.deviceKey;
        }
        if (key == null) {
            MeshLogger.e("upper transport encryption err: key null");
            return false;
        }
        this.encryptedPayload = Encipher.ccm(this.decryptedPayload, key, nonce, mic, true);
        return this.encryptedPayload != null;
    }


    private byte[] decrypt(
            int akf,
            byte aid,
            int aszmic,
            int sequenceNumber,
            int src,
            int dst) {
        byte[] key;
        // device key or application key
        byte[] nonce = null;
        byte[] seqNo = MeshUtils.sequenceNumber2Buffer(sequenceNumber);
        if (AccessType.DEVICE.akf == akf) {
            nonce = NonceGenerator.generateAccessNonce((byte) aszmic, seqNo, src, dst, this.mEncryptionSuite.ivIndex, AccessType.DEVICE);
            key = this.mEncryptionSuite.deviceKey;
            if (key == null) {
                MeshLogger.e("decrypt err: device key null");
                return null;
            }
            return decryptPayload(this.encryptedPayload, key, nonce, aszmic);
        } else {

            if (mEncryptionSuite.appKeyList != null) {
                byte decAid;
                byte[] decResult;
                for (byte[] appKey :
                        mEncryptionSuite.appKeyList) {
                    decAid = MeshUtils.generateAid(appKey);
                    if (decAid == aid) {
                        if (nonce == null) {
                            nonce = NonceGenerator.generateAccessNonce((byte) aszmic, seqNo, src, dst, this.mEncryptionSuite.ivIndex, AccessType.APPLICATION);
                        }

                        decResult = decryptPayload(this.encryptedPayload, appKey, nonce, aszmic);
                        if (decResult != null) {
                            return decResult;
                        }
                    }
                }
            }

        }

        return null;
    }

    private byte[] decryptPayload(byte[] payload, byte[] key, byte[] nonce, int aszmic) {
        if (aszmic == 1) {
            return Encipher.ccm(payload, key, nonce, 8, false);
        } else {
            return Encipher.ccm(payload, key, nonce, 4, false);
        }
    }


    public static class UpperTransportEncryptionSuite {
        private List<byte[]> appKeyList;
        private byte[] deviceKey;
        private int ivIndex;

        public UpperTransportEncryptionSuite(byte[] deviceKey, int ivIndex) {
            this.deviceKey = deviceKey;
            this.ivIndex = ivIndex;
        }

        public UpperTransportEncryptionSuite(List<byte[]> appKeyList, int ivIndex) {
            this.appKeyList = appKeyList;
            this.ivIndex = ivIndex;
        }


    }
}
