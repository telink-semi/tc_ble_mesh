package com.telink.sig.mesh.model.storage;

/**
 * Created by kee on 2019/5/29.
 */

import java.util.Arrays;

/**
 * typedef struct{
 * u32 sno;
 * u8 ct_flag;
 * u8 rfu1[2];
 * u8 iv_update_trigger_flag;     // trigger by self or other
 * u8 iv_index[4];
 * u8 rfu2[8];
 * u8 user[8];
 * }misc_save_t;
 */

/**
 * misc info in storage
 * currently sno and ivIndex are in used
 * sno: sequence number
 */
public class MiscStorage implements MeshStorageBridge {
    public static final int LEN = 28;
    public int sno;
    public byte ctFlag = 0;
    public byte[] rfu1 = new byte[2];
    public byte ivUpdateTriggerFlag = 0;
    public byte[] ivIndex; // 4 bytes
    public byte[] rfu2 = new byte[8];
    public byte[] user = new byte[8];


    public static MiscStorage getDefault(int sno, int ivIndex) {
        MiscStorage miscStorage = new MiscStorage();
        miscStorage.sno = sno;
        miscStorage.ivIndex = new byte[4];
        // big endian
        miscStorage.ivIndex[0] = (byte) (ivIndex >> 24);
        miscStorage.ivIndex[1] = (byte) (ivIndex >> 16);
        miscStorage.ivIndex[2] = (byte) (ivIndex >> 8);
        miscStorage.ivIndex[3] = (byte) (ivIndex);
        return miscStorage;
    }

    public static MiscStorage fromBytes(byte[] data) {
        if (data == null || data.length != 28) return null;
        MiscStorage miscStorage = new MiscStorage();
        int index = 0;
        miscStorage.sno = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8) | ((data[index++] & 0xFF) << 16)
                | ((data[index++] & 0xFF) << 24);
        miscStorage.ctFlag = data[index++];
        int rfuLen = 2;
        miscStorage.rfu1 = new byte[rfuLen];
        System.arraycopy(data, index, miscStorage.rfu1, 0, rfuLen);
        index += rfuLen;
        miscStorage.ivUpdateTriggerFlag = data[index++];

        int ivIndexLen = 4;
        miscStorage.ivIndex = new byte[ivIndexLen];
        System.arraycopy(data, index, miscStorage.ivIndex, 0, ivIndexLen);
        index += ivIndexLen;

        rfuLen = 8;
        miscStorage.rfu2 = new byte[rfuLen];
        System.arraycopy(data, index, miscStorage.rfu2, 0, rfuLen);
        index += rfuLen;

        int userLen = 8;
        miscStorage.user = new byte[userLen];
        System.arraycopy(data, index, miscStorage.user, 0, userLen);
        return miscStorage;
    }

    @Override
    public String toString() {
        return "MiscStorage{" +
                "sno=" + sno +
                ", ctFlag=" + ctFlag +
                ", rfu1=" + Arrays.toString(rfu1) +
                ", ivUpdateTriggerFlag=" + ivUpdateTriggerFlag +
                ", ivIndex=" + Arrays.toString(ivIndex) +
                ", rfu2=" + Arrays.toString(rfu2) +
                ", user=" + Arrays.toString(user) +
                '}';
    }

    @Override
    public byte[] toBytes() {
        byte[] re = new byte[LEN];
        int index = 0;
        re[index++] = (byte) sno;
        re[index++] = (byte) (sno >> 8);
        re[index++] = (byte) (sno >> 16);
        re[index++] = (byte) (sno >> 24);

        re[index++] = ctFlag;

        System.arraycopy(rfu1, 0, re, index, rfu1.length);
        index += rfu1.length;

        re[index++] = ivUpdateTriggerFlag;

        System.arraycopy(ivIndex, 0, re, index, ivIndex.length);
        index += ivIndex.length;

        System.arraycopy(rfu2, 0, re, index, rfu2.length);
        index += rfu2.length;

        System.arraycopy(user, 0, re, index, user.length);

        return re;
    }


}
