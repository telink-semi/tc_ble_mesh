package com.telink.sig.mesh.model.storage;

import com.telink.sig.mesh.lib.MeshLib;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Created by kee on 2019/5/29.
 */


/*

typedef struct{
    model_common_t com;             // must first
    u8 rfu1[8];
	u8 sec_nw_beacon;
	u8 ttl_def;
	u8 gatt_proxy;
	u8 frid;
	u8 relay;
	mesh_transmit_t relay_retransmit;	// must after relay
	mesh_transmit_t nw_transmit;
	u8 net_key_list;	// wait for define struct
	u8 app_key_list;	// wait for define struct
	u8 md2app_key_list;	// wait for define struct
	u8 node_identity_def;
	u8 rfu2[1];
	mesh_heartbeat_pub_str hb_pub;
	mesh_heartbeat_sub_str hb_sub;	// wait for define struct
}model_sig_cfg_s_t;


typedef struct{
	u16 dst_adr;
	u16 cnt_val;
	u8 	cnt_log;
	u8  per_log;
	u8  ttl;
	u16 feature;
	u16 netkeyidx;
}mesh_heartbeat_pub_str;
 */
// sizeof(model_sig_cfg_s_t), sizeof(mesh_key_save_t), sizeof(misc_save_t)
// 240  176  28
public class ConfigModelStorage implements MeshStorageBridge {
    public CommonModelStorage commonModel;
    public byte[] rfu1 = new byte[8];
    public byte secNwBeacon = 1;
    public byte ttlDef = 1;
    public byte gattProxy = 1;
    public byte frid = 1;
    public byte relay = 1;
    public byte relayRetransmit = 0;
    public byte nwTransmit = 0;
    public byte netKeyList = 0;
    public byte appKeyList = 0;
    public byte nd2AppKeyList = 0;
    public byte nodeIdentifyDef = 0;
    public byte rfu2 = 0;
    public HeartBeatPubStorage heartBeatPub = null;
    public HeartBeatSubStorage heartBeatSub = null;


    public static ConfigModelStorage getDefault(CommonModelStorage commonModel) {
        ConfigModelStorage configModel = new ConfigModelStorage();
        configModel.commonModel = commonModel;
        return configModel;
    }

    @Override
    public byte[] toBytes() {
        // 48 + common len, 240
        int len = CommonModelStorage.getLen() + 48;
        ByteBuffer byteBuffer = ByteBuffer.allocate(len);
        byteBuffer.put(commonModel.toBytes())
                .put(rfu1)
                .put(secNwBeacon)
                .put(ttlDef)
                .put(gattProxy)
                .put(frid)
                .put(relay)
                .put(relayRetransmit)
                .put(nwTransmit)
                .put(netKeyList)
                .put(appKeyList)
                .put(nd2AppKeyList)
                .put(nodeIdentifyDef)
                .put(rfu2)
                .put(heartBeatPub == null ? new byte[11] : heartBeatPub.toBytes())
                .put(heartBeatSub == null ? new byte[17] : heartBeatSub.toBytes());
        return byteBuffer.array();
    }


    public static class HeartBeatPubStorage implements MeshStorageBridge {


        int destAdr; // 2 bytes
        int cntVal;// 2 bytes
        byte cntLog;
        byte perLog;
        byte ttl;
        int feature;// 2 bytes
        int netKeyIndex;// 2 bytes

        @Override
        public byte[] toBytes() {

            ByteBuffer byteBuffer = ByteBuffer.allocate(11).order(ByteOrder.LITTLE_ENDIAN);
            byteBuffer.putShort((short) destAdr).putShort((short) cntVal)
                    .put(cntLog)
                    .put(perLog)
                    .put(ttl)
                    .putShort((short) feature)
                    .putShort((short) netKeyIndex);
            return byteBuffer.array();
        }
    }

    public static class HeartBeatSubStorage implements MeshStorageBridge {
        int adr;// 2 bytes
        int dest;// 2 bytes
        byte perLog;
        byte cntLog;
        byte minHops;
        byte maxHops;
        int perVal; // 2 bytes
        int cntVal; // 2 bytes
        byte state;
        int tick; // 4 bytes


        @Override
        public byte[] toBytes() {
            final int LEN = 17;
            byte[] re = new byte[LEN];
            int index = 0;
            re[index++] = (byte) adr;
            re[index++] = (byte) (adr >> 8);

            re[index++] = (byte) dest;
            re[index++] = (byte) (dest >> 8);

            re[index++] = perLog;
            re[index++] = cntLog;

            re[index++] = minHops;
            re[index++] = maxHops;

            re[index++] = (byte) perVal;
            re[index++] = (byte) (perVal >> 8);

            re[index++] = (byte) cntVal;
            re[index++] = (byte) (cntVal >> 8);

            re[index++] = state;

            re[index++] = (byte) tick;
            re[index++] = (byte) (tick >> 8);
            re[index++] = (byte) (tick >> 16);
            re[index] = (byte) (tick >> 24);

            return re;

        }
    }

    public static class BindKeyStorage implements MeshStorageBridge {

        /*
        u16 idx		:12;
        u16 rsv		:3;
        u16 bind_ok	:1;
         */

        public int index;
        public int rsv;
        public int bindOk;

        @Override
        public byte[] toBytes() {
            byte[] re = new byte[2];
            re[0] = (byte) index;
            re[1] = (byte) ((index >> 8) | ((rsv & 0b111) << 4) | ((bindOk & 0b1) << 7));
            return re;
        }
    }

    public static class ModelPublishStorage implements MeshStorageBridge {

        /*
        typedef struct{
	u16 appkey_idx      :12;
	u16 credential_flag :1;
	u16 rfu             :3;
	u8 ttl;
	mesh_pub_period_t period;
	mesh_transmit_t transmit;
}mesh_model_pub_par_t;
         */

        // 12 bits
        public int appKeyIndex;
        // 1 bit
        public int credentialFlag;
        // 3 bits
        public int rfu = 0;

        public byte ttl;
        public byte period;
        public byte transmit;


        @Override
        public byte[] toBytes() {
            byte[] re = new byte[5];
            re[0] = (byte) appKeyIndex;
            re[1] = (byte) ((appKeyIndex >> 8) | ((credentialFlag & 0b1) << 4) | ((rfu & 0b111) << 5));
            re[2] = ttl;
            re[3] = period;
            re[4] = transmit;
            return re;
        }
    }

}
