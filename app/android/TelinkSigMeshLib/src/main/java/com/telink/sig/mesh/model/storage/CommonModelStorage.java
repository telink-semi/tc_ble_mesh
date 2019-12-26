package com.telink.sig.mesh.model.storage;

import com.telink.sig.mesh.lib.MeshLib;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Created by kee on 2019/6/20.
 */

public class CommonModelStorage implements MeshStorageBridge {
    // 192 bytes
        /*
        u16 ele_adr;    // use as primary address for model_sig_cfg_s_t
    u8 no_pub   :1;	// means not support publish function
    u8 no_sub   :1; // means not support subscription function; must before pub and sub par
    u8 pub_trans_flag :1; // transition process was ongoing flag.
    u8 pub_2nd_state  :1; // eg: lightness and lightness linear.
    u8 rsv2;
    bind_key_t bind_key[BIND_KEY_MAX];
	u8 pub_uuid[16];
	cb_pub_st_t cb_pub_st;	// no need to save, fix later
	u32 cb_tick_ms;		    // no need to save, fix later
	u16 pub_adr;    // pub_adr and pub_par must existed if sub_list existed //  offset:32
	mesh_model_pub_par_t pub_par;
	u8 rfu3[1];
	u16 sub_list[SUB_LIST_MAX];     // pub_adr, pub_par, sub_list must follow com if existed
	u8 sub_uuid[SUB_LIST_MAX][16];
         */

//    private static final int LEN = 192;

    public int eleAdr; // 2

    public byte noPub = 0; // 1 bit
    public byte noSub = 0; // 1 bit
    public byte pubTransFlag = 0; // 1 bit
    public byte pub2ndState = 0; // 1 bit

    public byte rsv2 = 0; // 1

    public ConfigModelStorage.BindKeyStorage[] bindKeys; // 2 * 4

    public byte[] pubUUID; //  16

    public long cbPubSt; // 8 (callback)

    public int cbTickMs; // 4

    public int pubAdr; // 2

    public ConfigModelStorage.ModelPublishStorage meshPub; // 5

    public byte rfu3; // 1

    public int subList[] = null; // 2 * 8

    public byte[][] subUUIDs; // 8 * 16


    public static CommonModelStorage getDefault(int eleAdr,
                                                int keyIndex) {
        CommonModelStorage commonModel = new CommonModelStorage();
        commonModel.eleAdr = eleAdr;
        ConfigModelStorage.BindKeyStorage bindKey = new ConfigModelStorage.BindKeyStorage();
        bindKey.index = keyIndex;
        bindKey.bindOk = 1;
        bindKey.rsv = 0;
        commonModel.bindKeys = new ConfigModelStorage.BindKeyStorage[]{bindKey};

        return commonModel;
    }

    public static int getLen(){
        return 184 + MeshLib.getCallbackLen();
    }

    @Override
    public byte[] toBytes() {
        // result should be 192 in android
        int callbackLen = MeshLib.getCallbackLen();
        int len = getLen();
        ByteBuffer byteBuffer = ByteBuffer.allocate(len).order(ByteOrder.LITTLE_ENDIAN);
        byteBuffer.putShort((short) eleAdr)
                .put((byte) (noPub | (noSub << 1) | (pubTransFlag << 2) | (pub2ndState << 3)))
                .put(rsv2);
        // 4
        int keySize = 4;
        byte[] key;
        for (int i = 0; i < keySize; i++) {
            if (bindKeys != null && bindKeys.length > i) {
                key = bindKeys[i].toBytes();
                byteBuffer.put(key);
            } else {
                // 2 bytes
                byteBuffer.putShort((short) 0);
            }
        }

        if (pubUUID != null && pubUUID.length == 16) {
            byteBuffer.put(pubUUID);
        } else {
            byteBuffer.position(byteBuffer.position() + 16);
        }

        for (int i = 0; i < callbackLen; i++) {
            byteBuffer.put((byte) 0);
        }

        byteBuffer.putInt(cbTickMs)
                .putShort((short) pubAdr);
        if (meshPub != null) {
            byteBuffer.put(meshPub.toBytes());
        } else {
            byteBuffer.position(byteBuffer.position() + 5);
        }

        byteBuffer.put(rfu3);
        int left = 16;
        if (subList != null) {
            for (int i = 0; i < 8; i++) {
                if (subList.length - 1 > i) {
                    byteBuffer.putShort((short) subList[i]);
                    left -= 2;
                }
            }
        }
        byteBuffer.position(byteBuffer.position() + left);

        left = 8 * 16;
        if (subUUIDs != null) {
            for (int i = 0; i < 8; i++) {
                if (subUUIDs.length - 1 > i) {
                    byteBuffer.put(subUUIDs[i]);
                    left -= 16;
                }
            }
        }
        byteBuffer.position(byteBuffer.position() + left);

        return byteBuffer.array();

    }
}