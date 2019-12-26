/********************************************************************************************************
 * @file MeshCommand.java
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

package com.telink.sig.mesh.model;

/**
 * Created by kee on 2019/2/21.
 */

import com.telink.sig.mesh.util.Arrays;
import com.telink.sig.mesh.util.MeshUtils;

import java.io.Serializable;

/**
 * typedef struct{
 * u16 nk_idx;
 * u16 ak_idx;
 * u8 retry_cnt;   // only for reliable command
 * u8 rsp_max;     // only for reliable command
 * u16 adr_dst;
 * u8 op;
 * u8 par[MESH_CMD_ACCESS_LEN_MAX]; 380
 * }mesh_bulk_cmd_par_t;
 */

/**
 * @see com.telink.sig.mesh.light.MeshService#sendMeshCommand(MeshCommand)
 */
public class MeshCommand implements Serializable {

    private static final int HCI_CMD_BULK_CMD2DEBUG = 0xFFA3;

    private static final int CMD_RETRY_CNT = 2;

    public Object tag = "UNKNOWN-TAG";

    /**
     * network key index
     * 2 bytes
     */
    public int netKeyIndex;

    /**
     * application key index
     * 2 bytes
     */
    public int applicationKeyIndex;

    /**
     * 1 byte
     */
    public byte retryCnt;

    /**
     * 1 byte
     */
    public byte rspMax;

    /**
     * 2 bytes
     */
    public int destAddress;

    /**
     * bytes count:
     * 1: sig1
     *
     * @see OpcodeType#SIG_1
     * <p>
     * 2: sig
     * @see OpcodeType#SIG_2
     * <p>
     * 3: vendor
     * @see OpcodeType#VENDOR
     */
    public int opcode;

    /**
     * if vendor opcode: params[0] is rsp opcode(1 byte), valued 0 means unreliable command; params[1] is tid position (n - 1), in real param;
     * param[0] and param[1] will not be sent
     * then real params
     */
    public byte[] params;
//    public ModelMessage modelMessage;

    /**
     * only used when post command-complete event
     * application can handle this to check if got all devices'status
     */
    public int rspCnt;

    public static MeshCommand newInstance(int netKeyIndex, int appKeyIndex, int rspMax, int adr, int opcode) {
        MeshCommand command = new MeshCommand();
        command.netKeyIndex = netKeyIndex;
        command.applicationKeyIndex = appKeyIndex;
        command.retryCnt = CMD_RETRY_CNT;
        command.rspMax = (byte) rspMax;
        command.destAddress = adr;
        command.opcode = opcode;
        return command;
    }

    public static MeshCommand from(byte[] iniData) {

        int minLen = 10;
        if (iniData.length < minLen) {
            return null;
        }
        int index = 0;
        MeshCommand command = new MeshCommand();
        command.netKeyIndex = (iniData[index++] & 0xFF) | ((iniData[index++] & 0xFF) << 8);
        command.applicationKeyIndex = (iniData[index++] & 0xFF) | ((iniData[index++] & 0xFF) << 8);
        command.retryCnt = (byte) (iniData[index++] & 0xFF);
        command.rspMax = (byte) (iniData[index++] & 0xFF);
        command.destAddress = (iniData[index++] & 0xFF) | ((iniData[index++] & 0xFF) << 8);
        int opPre = iniData[index];
        OpcodeType opType = MeshUtils.getOpType(opPre);
        int opcode = 0;
        for (int i = 0; i < opType.byteSize; i++) {
            opcode |= (iniData[index++] & 0xFF) << (8 * i);
        }
        command.opcode = opcode;

        int paramLen = iniData.length - index;
        if (paramLen > 0) {
            byte[] params = new byte[paramLen];
            System.arraycopy(iniData, index, params, 0, paramLen);
            command.params = params;
        }

        command.rspCnt = 0;
        return command;
    }

    public byte[] toBytes() {
        OpcodeType type = MeshUtils.getOpType(opcode);
//        byte[] params = modelMessage == null ? null : modelMessage.toBytes();
        int reLen = 10 + type.byteSize + (params == null ? 0 : params.length);
        byte[] result = new byte[reLen];
        int index = 0;

        result[index++] = (byte) (HCI_CMD_BULK_CMD2DEBUG & 0xFF);
        result[index++] = (byte) ((HCI_CMD_BULK_CMD2DEBUG >> 8) & 0xFF);

        result[index++] = (byte) (netKeyIndex & 0xFF);
        result[index++] = (byte) ((netKeyIndex >> 8) & 0xFF);


        result[index++] = (byte) (applicationKeyIndex & 0xFF);
        result[index++] = (byte) ((applicationKeyIndex >> 8) & 0xFF);

        result[index++] = retryCnt;
        result[index++] = rspMax;

        result[index++] = (byte) (destAddress & 0xFF);
        result[index++] = (byte) ((destAddress >> 8) & 0xFF);

        result[index++] = (byte) (opcode & 0xFF);
        if (type.byteSize >= 2) {
            result[index++] = (byte) ((opcode >> 8) & 0xFF);
            if (type.byteSize == 3) {
                result[index++] = (byte) ((opcode >> 16) & 0xFF);
            }
        }
        if (params != null) {
            System.arraycopy(params, 0, result, index, params.length);
        }

        return result;
    }


    @Override
    public String toString() {
        return "MeshCommand{" +
                "netKeyIndex=" + String.format("%04X", netKeyIndex) +
                ", applicationKeyIndex=" + String.format("%04X", applicationKeyIndex) +
                ", retryCnt=" + String.format("%02X", retryCnt) +
                ", rspMax=" + String.format("%02X", rspMax) +
                ", destAddress=" + String.format("%04X", destAddress) +
                ", opcode=" + String.format("%06X", opcode) +
                ", params=" + Arrays.bytesToHexString(params, ":") +
                '}';
    }
}
