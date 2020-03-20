package com.telink.sig.mesh.model.storage;

/**
 * Created by kee on 2019/5/29.
 */


import com.telink.sig.mesh.util.Arrays;

/**
 * typedef struct{
 * u8 key[16]; // must at first, because mesh_tx_access_key_get()
 * u8 aid;
 * u16 index;
 * u8 valid;
 * }mesh_app_key_t;
 * <p>
 * typedef struct{
 * u8 valid;
 * u16 index;
 * u8 node_identity;
 * u8 rfu[12];		// for 16 align
 * u8 key[16];
 * mesh_app_key_t app_key[APP_KEY_MAX];
 * }mesh_net_key_save_t;
 * <p>
 * #define KEY_SAVE_ENCODE_ENABLE  1
 * #define KEY_SAVE_ENCODE_FLAG    (0x3A)
 * <p>
 * typedef struct{
 * u8 dev_key[16];	// device key, use as app key
 * mesh_net_key_save_t net_key[NET_KEY_MAX];
 * u8 rfu0[14];
 * u8 mic_val;
 * u8 encode_flag;
 * }mesh_key_save_t;
 */

public class MeshKeyStorage implements MeshStorageBridge<MeshKeyStorage> {
    public static final int LEN = 176;
    public byte[] devKey; // 16 bytes
    public MeshNetKeyStorage[] meshNetKey; //  72 * 2 = 144
    public byte[] rfu = new byte[14]; // 14 bytes
    public byte mic;
    public byte encodeFlag;

    public static MeshKeyStorage getDefault(MeshNetKeyStorage meshNetKey) {
        MeshKeyStorage meshKeyStorage = new MeshKeyStorage();
        meshKeyStorage.devKey = new byte[16];
        meshKeyStorage.meshNetKey = new MeshNetKeyStorage[]{meshNetKey};
        meshKeyStorage.mic = 0;
        meshKeyStorage.encodeFlag = 0;
        return meshKeyStorage;
    }

    public static MeshKeyStorage fromBytes(byte[] data) {
        if (data == null || data.length != LEN) return null;

        MeshKeyStorage meshKeyStorage = new MeshKeyStorage();
        int index = 0;
        int devKeyLen = 16;
        meshKeyStorage.devKey = new byte[devKeyLen];
        System.arraycopy(data, index, meshKeyStorage.devKey, 0, devKeyLen);
        index += devKeyLen;
        byte[] netKey = new byte[MeshNetKeyStorage.LEN];
        System.arraycopy(data, index, netKey, 0, MeshNetKeyStorage.LEN);
        index += MeshNetKeyStorage.LEN;
        MeshNetKeyStorage meshNetKey0 = MeshNetKeyStorage.fromBytes(netKey);

        System.arraycopy(data, index, netKey, 0, MeshNetKeyStorage.LEN);
        index += MeshNetKeyStorage.LEN;
        MeshNetKeyStorage meshNetKey1 = MeshNetKeyStorage.fromBytes(netKey);

        meshKeyStorage.meshNetKey = new MeshNetKeyStorage[]{meshNetKey0, meshNetKey1};

        int rfuLen = 14;
        meshKeyStorage.rfu = new byte[rfuLen];
        System.arraycopy(data, index, meshKeyStorage.rfu, 0, rfuLen);
        index += rfuLen;

        meshKeyStorage.mic = data[index++];
        meshKeyStorage.encodeFlag = data[index];

        return meshKeyStorage;
    }

    @Override
    public byte[] toBytes() {
        if (devKey == null || devKey.length != 16 || meshNetKey == null) return null;

        byte[] re = new byte[LEN];
        int index = 0;
        System.arraycopy(devKey, 0, re, index, devKey.length);
        index += devKey.length;

        byte[] meshNetKeyData;
        for (int i = 0; i < 2; i++) {
            if (meshNetKey.length > i) {
                meshNetKeyData = meshNetKey[i].toBytes();
                System.arraycopy(meshNetKeyData, 0, re, index, meshNetKeyData.length);
            }
            index += MeshNetKeyStorage.LEN;
        }

        System.arraycopy(rfu, 0, re, index, rfu.length);
        index += rfu.length;

        re[index++] = mic;
        re[index] = encodeFlag;

        return re;
    }

    @Override
    public String toString() {
        return "MeshKeyStorage{" +
                "devKey=" + Arrays.bytesToHexString(devKey, ":") +
                ", meshNetKey=" + meshNetKey[0] +
                ", rfu=" + Arrays.bytesToHexString(rfu, "") +
                ", mic=" + mic +
                ", encodeFlag=" + encodeFlag +
                '}';
    }

    public static class MeshNetKeyStorage implements MeshStorageBridge<MeshNetKeyStorage> {
        public static final int LEN = 72;
        public byte valid;
        public int index; // 2 bytes
        public byte nodeId;
        public byte[] rfu = new byte[12]; // 12 bytes , for 16 align
        public byte[] key; // 16 bytes
        public MeshAppKeyStorage[] meshAppKey; // 40

        public static MeshNetKeyStorage getDefault(byte[] netKey, MeshAppKeyStorage meshAppKey) {
            MeshNetKeyStorage meshNetKeyStorage = new MeshNetKeyStorage();
            meshNetKeyStorage.valid = 1;
            meshNetKeyStorage.index = 0;
            meshNetKeyStorage.nodeId = 1;
            meshNetKeyStorage.key = netKey;
            meshNetKeyStorage.meshAppKey = new MeshAppKeyStorage[]{meshAppKey};
            return meshNetKeyStorage;
        }


        @Override
        public byte[] toBytes() {
            if (key == null || key.length != 16 || meshAppKey == null) return null;

            byte[] re = new byte[LEN];
            int index = 0;
            re[index++] = valid;
            re[index++] = (byte) this.index;
            re[index++] = (byte) (this.index >> 8);
            re[index++] = nodeId;
            System.arraycopy(rfu, 0, re, index, rfu.length);
            index += rfu.length;

            System.arraycopy(key, 0, re, index, key.length);
            index += key.length;
            byte[] meshAppKeyData;
            for (int i = 0; i < 2; i++) {
                if (meshAppKey.length > i) {
                    meshAppKeyData = meshAppKey[i].toBytes();
                    System.arraycopy(meshAppKeyData, 0, re, index, meshAppKeyData.length);
                }
                index += MeshAppKeyStorage.LEN;
            }

            return re;
        }

        public static MeshNetKeyStorage fromBytes(byte[] data) {
            MeshNetKeyStorage netKeyStorage = new MeshNetKeyStorage();
            int index = 0;
            netKeyStorage.valid = data[index++];
            netKeyStorage.index = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
            netKeyStorage.nodeId = data[index++];
            int rfuLen = 12;
            netKeyStorage.rfu = new byte[rfuLen];
            System.arraycopy(data, index, netKeyStorage.rfu, 0, rfuLen);
            index += rfuLen;

            int keyLen = 16;
            netKeyStorage.key = new byte[keyLen];
            System.arraycopy(data, index, netKeyStorage.key, 0, keyLen);
            index += keyLen;

            byte[] reData = new byte[MeshAppKeyStorage.LEN];
            System.arraycopy(data, index, reData, 0, MeshAppKeyStorage.LEN);
            index += MeshAppKeyStorage.LEN;
            MeshAppKeyStorage meshAppKey0 = MeshAppKeyStorage.fromBytes(reData);

            System.arraycopy(data, index, reData, 0, MeshAppKeyStorage.LEN);
            MeshAppKeyStorage meshAppKey1 = MeshAppKeyStorage.fromBytes(reData);

            netKeyStorage.meshAppKey = new MeshAppKeyStorage[]{meshAppKey0, meshAppKey1};

            return netKeyStorage;
        }

        @Override
        public String toString() {
            return "MeshNetKeyStorage{" +
                    "valid=" + valid +
                    ", index=" + index +
                    ", nodeId=" + nodeId +
                    ", rfu=" + Arrays.bytesToHexString(rfu, ":") +
                    ", key=" + Arrays.bytesToHexString(key, ":") +
                    ", meshAppKey=" + meshAppKey[0] +
                    '}';
        }
    }

    public static class MeshAppKeyStorage implements MeshStorageBridge {
        public static final int LEN = 20;
        public byte[] key; // 16 bytes
        public byte aid;
        public int index; // 2 bytes
        public byte valid;

        public static MeshAppKeyStorage getDefault(byte[] key) {
            MeshAppKeyStorage meshAppKeyStorage = new MeshAppKeyStorage();
            meshAppKeyStorage.aid = 1;
            meshAppKeyStorage.index = 0;
            meshAppKeyStorage.valid = 1;
            meshAppKeyStorage.key = key;
            return meshAppKeyStorage;
        }


        @Override
        public byte[] toBytes() {
            if (key == null || key.length != 16) return null;
            final int LEN = 20;
            byte[] re = new byte[LEN];
            int index = 0;
            System.arraycopy(key, index, re, 0, key.length);
            index += key.length;
            re[index++] = aid;
            re[index++] = (byte) this.index;
            re[index++] = (byte) (this.index >> 8);
            re[index] = valid;
            return re;
        }

        public static MeshAppKeyStorage fromBytes(byte[] data) {
            MeshAppKeyStorage appKeyStorage = new MeshAppKeyStorage();
            int index = 0;
            appKeyStorage.key = new byte[16];
            System.arraycopy(data, index, appKeyStorage.key, 0, 16);
            index += 16;
            appKeyStorage.aid = data[index++];
            appKeyStorage.index = (data[index++] & 0xFF) | ((data[index++] & 0xFF) << 8);
            appKeyStorage.valid = data[index];
            return appKeyStorage;
        }

        @Override
        public String toString() {
            return "MeshAppKeyStorage{" +
                    "key=" + Arrays.bytesToHexString(key, ":") +
                    ", aid=" + aid +
                    ", index=" + index +
                    ", valid=" + valid +
                    '}';
        }
    }


}
