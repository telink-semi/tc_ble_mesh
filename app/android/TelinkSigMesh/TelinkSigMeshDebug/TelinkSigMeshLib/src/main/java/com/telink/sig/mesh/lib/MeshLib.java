/********************************************************************************************************
 * @file MeshLib.java
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
package com.telink.sig.mesh.lib;

import android.content.Context;

import com.telink.sig.mesh.model.NodeInfo;
import com.telink.sig.mesh.util.Arrays;
import com.telink.sig.mesh.util.TelinkLog;

import java.nio.ByteBuffer;

/**
 * Created by kee on 2017/7/24.
 */

public class MeshLib {

    static {
        System.loadLibrary("mesh_lib_share");
    }

//    private ReentrantLock lock = new ReentrantLock();

    /**
     * global context
     */
    private Context context;

    /**
     * helper for interface implement
     */
    private Helper helper;

    /**
     * helper for storage
     */
    private StorageHelper storageHelper;

    /**
     * helper for cloud interface implement
     */

    private VendorHelper vendorHelper;

    private FastProvisionHelper fastProvisionHelper;

    public MeshLib(Context context) {
        this.context = context;

//        setGattProvisionNetInfo(null, null, null, null, (short) 13076);
//        setGattProCloudEn((byte) 1);
//        setKeyBindList(new short[]{0x1000, 0x1002, 0x1004, 0x1300, 0x1301, 0x1303, 0x1304, 0x1306, 0x130f, 0x1310}, (byte) 10);
        /*setGattProvisionNetInfo(new byte[]{0x01, 0x01, 0x01, 0x01,
                        0x01, 0x01, 0x01, 0x01,
                        0x01, 0x01, 0x01, 0x01,
                        0x01, 0x01, 0x01, 0x01},
                null, (byte) 13, new byte[]{14, 15, 16, 17}, (short) 18);*/
    }


    public void setLibHelper(Helper libHelper) {
        helper = libHelper;
    }

    public void setStorageHelper(StorageHelper storageHelper) {
        this.storageHelper = storageHelper;
    }

    public void setVendorHelper(VendorHelper helper) {
        vendorHelper = helper;
    }

    public void setFastProvisionHelper(FastProvisionHelper fastProvisionHelper) {
        this.fastProvisionHelper = fastProvisionHelper;
    }

    public int getLibVersion() {
        return getVersion();
    }

    // java native method
    public void sendPkt(int type, byte b[]) {
        TelinkLog.d("sendPkt type:" + type + " data:" + Arrays.bytesToHexString(b, ":"));
        if (helper != null) {
            helper.sendPkt(type, b);
        }
    }

    public void onReliableCmdStop(int op, int repMax, int repCnt) {

        if (helper != null) {
            helper.onReliableCmdStop(op, repMax, repCnt);
        }
    }

    public void logArray(byte b[]) {
        TelinkLog.d("logArray:" + Arrays.bytesToHexString(b, ":"));
    }

    public void onProvisionBeaconChecked(int result) {
        /*TelinkLog.d("onProvisionBeaconChecked:" + result);
        if (helper != null) {
            helper.onProvisionBeaconChecked(result);
        }*/
    }

    public void flashRead(ByteBuffer byteBuffer, int addr, int len) {
//        FlashOperation.readFlash(context, byteBuffer, addr, len);
    }

    public void flashWrite(byte[] buffer, int addr, int len) {
//        FlashOperation.writeFlash(context, buffer, addr, len);
    }

    public void onIniCmdError(byte[] pu) {
        if (pu == null || pu.length <= 2) {
            TelinkLog.w("onIniCmdError#params#wrong");
            return;
        }
        int len = pu.length - 1;
        byte[] iniData = new byte[len];
        System.arraycopy(pu, 1, iniData, 0, len);
        TelinkLog.w("INI CMD ERR:" + Arrays.bytesToHexString(iniData, ":"));
        if (helper != null) {
            helper.onCommandErr(iniData);
        }
    }

    public void onCmdRsp(byte[] pu) {
        if (pu == null || pu.length == 0) {
            TelinkLog.w("onCmdRsp#NULL");
            return;
        }

        int length = pu.length - 1;
        int index = 2;
        byte[] ret = new byte[length];

        System.arraycopy(pu, 0, ret, 0, index);
        System.arraycopy(pu, index + 1, ret, index, length - index);

        TelinkLog.w("MESH_CMD_RSP:" + Arrays.bytesToHexString(ret, ":"));
        if (helper != null) {
            helper.onCommandRsp(ret);
        }
    }

    public void terminate() {
        if (helper != null) {
            helper.terminate();
        }
    }

    public void onFilterInitComplete() {
        if (helper != null) {
            helper.onFilterInitComplete();
        }
    }

    public void onHeartbeatNotify(int src, int dest, byte[] data) {
        TelinkLog.d("heartbeat notify: " + String.format("src -- %04X  dest -- %04X ", src, dest) + Arrays.bytesToHexString(data, ":"));
    }

    /**
     * provision action complete
     *
     * @param state 0: success；1: timeout
     */
    public void onProvisionComplete(byte state) {
        if (helper != null) {
            helper.onProvisionStateChange(state);
        }
    }

    /**
     * @param event 1： success； 0： timeout
     */
    public void onAppKeyBindEnd(byte event) {
        if (helper != null) {
            helper.onAppKeyBindEnd(event);
        }
    }

    // when iv updated
    public void onIvUpdated(byte[] ivIndex) {
        if (helper != null) {
            int iv = ((ivIndex[0] & 0xFF) << 24) |
                    ((ivIndex[1] & 0xFF) << 16) |
                    ((ivIndex[2] & 0xFF) << 8) |
                    (ivIndex[3] & 0xFF);
            helper.onIvUpdated(iv);
        }
    }

    /**
     * remote provision
     */

    public void onRemoteProvisionCapabilityGet(int maxItem, int activeScan) {
        TelinkLog.d("onRemoteProvisionCapabilityGet: " + maxItem + " -- " + activeScan);
        if (helper != null) {
//            helper.onRemoteCapabilityGet(maxItem, activeScan);
        }
    }

    public void onRemoteProvisionScanReport(byte[] scanRecord) {
        TelinkLog.d("onRemoteProvisionScanReport:" + Arrays.bytesToHexString(scanRecord, ":"));
        if (helper != null) {
//            helper.onRemoteScanReport(scanRecord);
        }
    }

    /**
     * @param address unicast address
     * @param uuid    16 bytes
     */
    public native void initRemoteProvision(int address, byte[] uuid);

    public native void setKeyBindList(short[] keyBindListBuf, byte listCnt);


    /**
     * fast provision
     */

    public native void startFastProv(int pid);

    public int onFastProvAdrRequest() {
        if (fastProvisionHelper != null) {
            return fastProvisionHelper.onFastProvAdrRequest();
        }
        return -1;
    }

    public void onFastProvNodeInfoCallback(byte[] devKey, int nodeAdr, int pid) {
        if (fastProvisionHelper != null) {
            fastProvisionHelper.onFastProvNodeInfoCallback(devKey, nodeAdr, pid);
        }
    }

    public int onFastProvEleCntRequire(int pid) {
        if (fastProvisionHelper != null) {
            return fastProvisionHelper.onFastProvEleCntRequire(pid);
        }
        return 0;
    }

    /**
     * set provision cloud params
     *
     * @param pid 4 bytes
     * @param mac 6 bytes
     * @return 0 err; 1 success
     * @deprecated
     */
    public int setProvCloudParams(byte[] pid, byte[] mac) {
        TelinkLog.d("setProvCloudParams: " + Arrays.bytesToHexString(pid, ":") + " -- " + Arrays.bytesToHexString(mac, ":"));
        if (vendorHelper != null)
            return vendorHelper.setProvCloudParams(pid, mac);
        else return 1;
    }

    /**
     * set provision cloud confirm data
     *
     * @param confirm    16 bytes
     * @param confirmKey 16 bytes
     * @param proRandom  16 bytes
     * @return 0 err; 1 success
     */
    public int setProvCloudConfirm(byte[] confirm, byte[] confirmKey, byte[] proRandom) {
        TelinkLog.d("setProvCloudConfirm");
        if (vendorHelper != null)
            return vendorHelper.setProvCloudConfirm(confirm, confirmKey, proRandom);
        else return 1;
    }

    /**
     * cloud device confirm check callback
     *
     * @param confirmKey 16 bytes
     * @param devRandom  16 bytes
     * @param devConfirm 16 bytes
     * @return 0 err; 1 success
     */
    public int cloudDevConfirmCheck(byte[] confirmKey, byte[] devRandom, byte[] devConfirm) {
        if (vendorHelper != null)
            return vendorHelper.cloudDevConfirmCheck(confirmKey, devRandom, devConfirm);
        else return 1;
    }

    public void gattProvisionNetInfoCallback() {
        // setGattProvisionNetInfo()
        TelinkLog.d("gattProvisionNetInfoCallback");
        /*setGattProvisionNetInfo(new byte[]{0x01, 0x01, 0x01, 0x01,
                        0x01, 0x01, 0x01, 0x01,
                        0x01, 0x01, 0x01, 0x01,
                        0x01, 0x01, 0x01, 0x01},
                null, (byte) 13, new byte[]{14, 15, 16, 17}, (short) 18);*/
        if (vendorHelper != null) {
            vendorHelper.gattProvisionNetInfoCallback();
        }
    }

    // void set_gatt_provision_net_info(u8 p_netkey[16],u16*p_key_index,u8*p_flag,u8 p_ivi[4],u16*p_unicast)

    public native void setGattProvisionNetInfo(byte[] networkKey, Short keyIndex, Byte flag, byte[] ivIndex, Short unicast);

    public native void setGattProCloudEn(byte enable);

    // java & c bridge method


    /**
     * get lib version
     * should invoke when start
     */
    public native int getVersion();

    /**
     * init mesh info
     */
    public native void meshInitAll();


    /**
     * set vendor id
     *
     * @param vendorId 2 bytes
     */
    public native void resetVendorId(short vendorId);

    /**
     * @param opcode
     */
    public native boolean isOpReliable(int opcode);

    /**
     * init clock
     */
    public native void masterClockInit();

    /**
     * main loop for lib processing fifo
     */
    public native void threadMainProcess();


    /**
     * init provisioner random data
     */
    public native void provisionRandomDataInit();


    /**
     * set provision data before process node provision info
     *
     * @param data data
     * @param len  data len
     */
    public native int setProvisionData(byte[] data, int len);

    /**
     * start node provision invite
     */
    public native void startProvisionInvite();

    /**
     * dispatch provision data
     */
    public native void provisionDispatch(byte[] notifyData, int len, ByteBuffer proxyData, int proxyLenP);


    /**
     * push ble notify data to lib fifo
     *
     * @param notifyData notify data
     * @param len        data len
     */
    public native void pushNotifyIntoFifo(byte[] notifyData, int len);

    /**************************SendOpParaDebug**************************
     function : the basic function of the send cmd by the opcode ,use the op(reflect to the mode )
     to operation all the models
     para:
     adr_dst: the dst unicast addr.
     rsp_max: means after sending the parameter,it will how many different packets back
     op: the opcode to operation the models ,this can reflect to the model
     par: the par buf to control the model
     len : the len of the buf
     ret: 0  means OK
     -1 or other value means err
     ****************************************************************************/

    /**
     * the basic function of the send cmd by the opcode ,use the op(reflect to the mode )
     *
     * @param addr     the dst unicast addr.
     * @param rspMax   means after sending the parameter,it will how many different packets back
     * @param opCode   the opcode to operation the models ,this can reflect to the model
     * @param params   the par buf to control the model
     * @param paramLen buf len
     * @return 0  means OK ; -1 or other value means err
     */
    public native int sendOpParaDebug(int addr, byte rspMax, int opCode, byte[] params, int paramLen);


    /**
     * common command
     * typedef struct{
     * u16 nk_idx;
     * u16 ak_idx;
     * u8 retry_cnt;   // only for reliable command
     * u8 rsp_max;     // only for reliable command
     * u16 adr_dst;
     * u8 op;
     * u8 par[MESH_CMD_ACCESS_LEN_MAX]; 380
     * }mesh_bulk_cmd_par_t;
     *
     * @param params @ref mesh_bulk_cmd_par_t
     * @param len    len
     * @return 0 success, -1 err
     */
    public native int sendOpByINI(byte[] params, int len);

    /**************************access_cmd_onoff**************************
     function : access_cmd_onoff,turn on or turn off the lights part
     para:
     adr_dst: the src adr you will send to the cmd .
     rsp_max: means after sending the parameter,it will how many different packets back
     onff: 1 means on ,and 0 means off
     ack: 1 means send reliable cmd ,and the node will send rsp ,0 means unreliable ,will not send
     ret: 0  means OK
     -1 or other value means err
     ****************************************************************************/
    public native int cmdOnOff(int addr, byte rspMax, byte onOff, int ack);


    /**************************cfg_cmd_sub_get**************************
     function : cfg_cmd_sub_get,get the model's subscription list addr part
     para:
     node_adr: the dst adr of the node ,element_primary part
     ele_adr:  model adr of the node
     md_id: the model id part

     ret: 0  means OK
     -1 or other value means err
     ****************************************************************************/
    public native int cfgCmdSubGet(int nodeAdr, int eleAdr, int modelId);


    /**************************cfg_cmd_sub_set**************************
     function : cfg_cmd_sub_set,set the model's subscription list addr part
     para:
     op: the opcode to operate the subscription list addr part ,such as CFG_MODEL_SUB_ADD,CFG_MODEL_SUB_DEL
     node_adr: the node's element addr .
     ele_adr: the model's adr part
     sub_adr: the adr used to add the remove for the addr list
     md_id: the special model id
     sig_model: the sig_model used to operate
     ret: 0  means OK
     -1 or other value means err
     ****************************************************************************/
    public native int cfgCmdSubSet(int opCode, int nodeAdr, int eleAdr, int subAdr, int modelId, byte sig);


    /**************************cfg_cmd_pub_get**************************
     function : cfg_cmd_pub_get,get the model 's pub addr
     para:
     node_adr: the node's element addr .
     ele_adr: the model's adr part
     md_id: the special model id
     sig_model: the sig_model used to operate
     ret: 0  means OK
     -1 or other value means err
     ****************************************************************************/
    public native int cfgCmdPubGet(int nodeAdr, int eleAdr, int modelId, byte sig);


    /**************************cfg_cmd_pub_set**************************
     function : cfg_cmd_pub_set,set the model 's pub addr
     para:
     node_adr: the node's element addr .
     ele_adr: the model's adr part
     pub_adr: the adr used to set for the pub addr
     md_id: the special model id
     sig_model: the sig_model used to operate
     ret: 0  means OK
     -1 or other value means err
     ****************************************************************************/
    /**
     * @param nodeAdr node address
     * @param eleAdr  element address
     * @param pubAdr  publish address
     * @param modelId model id
     * @param param   publish params , len 5
     *                typedef struct{
     *                u16 appkey_idx      :12;
     *                u16 credential_flag :1;
     *                u16 rfu             :3;
     *                u8 ttl;
     *                mesh_pub_period_t period;
     *                mesh_transmit_t transmit;
     *                }mesh_model_pub_par_t;
     * @param sig     is sig model
     * @return 0 OK, other err
     */
    public native int cfgCmdPubSet(int nodeAdr, int eleAdr, int pubAdr, int modelId, byte[] param, byte sig);

    /**
     * set not relay config
     *
     * @param nodeAdr adr
     * @param val     0: no relay; 1: relay
     * @return
     */
    public native int cfgCmdRelaySet(int nodeAdr, int val);

    /************************** access_cmd_set_level **************************
     function : set the lightness of the light
     para:
     adr: set the node's adr
     rsp_max: means after sending the parameter,it will how many different packets back
     lightness: -32768 ~ 32767
     ack: 1 means send reliable cmd ,and the node will send rsp ,0 means unreliable ,will not send
     ret: 0  means OK
     -1 or other value means err
     ****************************************************************************/
    public native int cmdSetLevel(int addr, byte rspMax, int level, int ack);

    /**
     * 设置 lightness delta
     *
     * @param adr        目标地址
     * @param rspMax     最大回复个数
     * @param delta      delta
     * @param ack        ack
     * @param transition 渐变
     * @return
     */
    public native int cmdSetDelta(int adr, byte rspMax, int delta, int ack, byte[] transition);

    /**
     * set the lum of the light
     *
     * @param addr   set the node's adr
     * @param rspMax means after sending the parameter,it will how many different packets back
     * @param lum    0 ~ 100
     * @param ack    1 means send reliable cmd ,and the node will send rsp ,0 means unreliable ,will not send
     * @return 0  means OK ; -1 or other value means err
     */
    public native int cmdSetLum(int addr, byte rspMax, byte lum, int ack);

    public native int cmdSetTemp(int addr, byte rspMax, byte temp, int ack);

    public native int cmdSetHSL100(int addr, byte rspMax, byte hue, byte saturation, byte lightness, int ack, byte[] transition);

    /**
     * access_cmd_get_level,to get the nodes in the net ,online status
     *
     * @param addr   target address
     * @param rspMax means after sending the parameter,it will how many different packets back
     * @return 0: success  ; !0 fail
     */
    public native int cmdGetLevel(int addr, int rspMax);


    /**
     * 设置设备亮度
     *
     * @param addr   target addr
     * @param rspMax response max
     * @return 0: success  ; !0 fail
     */
    public native int cmdGetLum(int addr, int rspMax);

    public native int cmdGetOnOff(int addr, int rspMax);

    public native int cmdGetTemp(int addr, int rspMax);

    /**
     * 获取设备CTL
     *
     * @param addr   target addr
     * @param rspMax response max
     * @return 0: success  ; !0 fail
     */
    public native int cmdGetCtl(int addr, int rspMax);

    /**
     * 设置本地的provision信息
     * set local(app/phone as node) provision info
     *
     * @param data data
     * @param len  len
     */
    public native byte meshProvisionParSetDir(byte[] data, int len);

    /**
     * remove node from mesh
     *
     * @param addr node address
     * @return 0 ok  !0 error
     */
    public native int cmdDeleteNode(int addr);


//    public native int cmdGetScheduler(int addr, int rspMax, byte index);

    public native int cmdSetScheduler(int addr, int rspMax, long schedulerParam, int sceneId, int ack);

    public native int cmdGetScheduler(int addr, int rspMax, byte index);

    public native int cmdSetTime(int addr, int rspMax, long taiTime, int zoneOffset);

    // save scene
    public native int cmdSceneStore(int adr, int rspMax, int id, int ack);

    public native int cmdSceneDelete(int adr, int rspMax, int id, int ack);

    public native int cmdSceneRecall(int adr, int rspMax, int id, int ack, byte[] transition);

    /**
     * set proxy filter
     *
     * @param addr mesh address
     */
    public native void meshProxySetFilterInit(int addr);


    /**
     * set appKey for device
     *
     * @param apkIndex    default 0
     * @param appKey      uniform appKey generated by app
     * @param meshAddress device  address
     * @param netKeyIndex network key index
     */
    public native void meshConfigModeParaSet(int apkIndex, byte[] appKey, int meshAddress, int netKeyIndex, int fastMode);

    /**
     * notify lib for device disconnection
     */
    public native void onDeviceDisconnect();

    /**
     *
     */
    public native void cacheInit();
    /**********************************************************************
     * unit convert util
     **********************************************************************/

    /**
     * lightness to lum
     *
     * @param level -32768 - 32767
     */
    public native byte level2Lum(short level);

    /**
     * lum to lightness
     *
     * @param lum 0-100
     */
    public native int lum2Level(byte lum);


    /**
     * lum to lightness
     */
    public native int lum2Lightness(byte lum);

    /**
     * lightness to lum
     */
    public native byte lightness2Lum(int lightness);

    /**
     * temp100 to temp
     */
    public native int temp100ToTemp(byte temp100);

    /**
     * temp to temp100
     */
    public native byte tempToTemp100(int temp);


    /**
     * get node info in key bind end callback
     */
    public native byte[] getNodeInfo(int adr);

    /**
     * get element cnt in provision success callback
     *
     * @return cnt
     */
    public native int getElementCnt(int adr);


    /**
     * 重新配置 so lib中的vc_node_info
     *
     * @param nodes nodes info data, if array is null, all nodes will be removed
     */
    public native void reattachNodes(byte[][] nodes);

    /**
     * remove node info from VC_NODE_INFO
     *
     * @param address node info data
     * @see com.telink.sig.mesh.model.DeviceInfo#nodeInfo
     * @see NodeInfo#toVCNodeInfo()
     */
    public native void removeNodeInfo(int address);

    /**
     * add node info to VC_NODE_INFO
     *
     * @param nodeInfo node info data
     * @see com.telink.sig.mesh.model.DeviceInfo#nodeInfo
     * @see NodeInfo#toVCNodeInfo()
     */
    public native void addNodeInfo(int address, byte[] nodeInfo);

    /**
     * 重新配置 app key信息
     *
     * @param appKeyIndex app key idx
     * @param netKeyIndex net key idx
     * @param appKey      key value
     */
    public native void resetAppKey(int appKeyIndex, int netKeyIndex, byte[] appKey);

    /**
     * 获取当前 provision 设备的device key
     *
     * @return val
     */
    public native byte[] getCurrentDeviceKey();

    public native byte[] decryptOnlineStatusData(byte[] data);


    public native byte[] getLocalCpsData();


    /*************************************************************************************
     ** mesh OTA **
     **************************************************************************************/

    /**
     * ota progress update
     * 0: prepare
     * 1-99: sending packets
     * 100: ota complete
     *
     * @param currentBlock current block
     * @param totalBlock   total blocks
     * @param progress     0-100
     */
    public void onMeshOTAProgressUpdate(int currentBlock, int totalBlock, int progress) {
        TelinkLog.d("onMeshOTAProgressUpdate:" + progress);
        if (helper != null) {
            helper.onMeshOTAProgressUpdate(currentBlock, totalBlock, progress);
        }
    }

    /**
     * request for firmware data
     *
     * @param maxLen supported max len
     * @return firmware data
     */
    public byte[] onMeshOTAFirmwareRequest(int maxLen) {
        TelinkLog.d("onMeshOTAFirmwareRequest");
        if (helper != null) {
            return helper.onMeshOTAFirmwareRequest(maxLen);
        }
        return null;
    }

    public void onMeshOtaApplyStatusNotify(int srcAdr, byte[] data) {
        TelinkLog.d("onMeshOtaApplyStatusNotify: " + srcAdr + " -- " + Arrays.bytesToHexString(data, ":"));
        if (helper != null) {
            helper.onMeshOtaApplyStatusNotify(srcAdr, data);
        }
    }

    public int onConnectedAdrRequest() {
        TelinkLog.d("onConnectedAdrRequest");
        if (helper != null) {
            return helper.onConnectedAdrRequest();
        }
        return 0;
    }

    /**
     * continue mesh ota when device disconnected previous
     *
     * @return false: continue error
     */
    public native boolean continueMeshOta();

    public native boolean pauseMeshOta();

    public native void meshRetrieveAll();

    /*************************************************************************************
     * mesh storage
     *************************************************************************************/
    /**
     * @param len
     * @param storageType
     * @return
     */
    public byte[] onMeshStorageRetrieve(int len, int storageType) {
        TelinkLog.d("onMeshStorageRetrieve:" + storageType);
        if (storageHelper != null) {
            return storageHelper.onMeshStorageRetrieve(len, storageType);
        }
        return null;
    }

    public void onMeshStorageUpdate(byte[] buffer, int storageType) {
        TelinkLog.d("onMeshStorageUpdate: " + storageType);
        if (storageHelper != null) {
            storageHelper.onMeshStorageUpdate(buffer, storageType);
        }
    }

    /**
     * @return 8 or 4, function pointer
     */
    public native static int getCallbackLen();

    public native byte[] getConfigInfo();

    /******************************************
     * test func
     *******************************************/
//    public native void loopbackTest();
    public native void initMeshKey();

    public native byte[] getMeshKey();

    public native void meshLoopProcess();

    public native void sendCommand(byte[] data, int len);

//    public native int getProvisionState();

    public native void meshSendClProxyBv03(int node_adr);

    public native void meshSendClProxyBv04(int node_adr);

    public native void meshSendClProxyBv05(int node_adr);

    public native void meshSendClProxyBv06(int node_adr);

    public native void meshSendClProxyBv07(int node_adr);

    public native int getClock();

    public native int clockTimeExceed(int ref, int spanUs);

    public native long clockTime();

    public native long ivTest();

    public native long snoTest();

    /**
     * mesh lib helper
     * bridge for ble operations
     */
    public interface Helper {

        void sendPkt(int type, byte data[]);

        void onCommandRsp(byte[] data);

        void onCommandErr(byte[] iniData);

        void onProvisionStateChange(byte state);

        void onReliableCmdStop(int op, int rspMax, int repCnt);

        void terminate();

        void onAppKeyBindEnd(byte event);

        void onIvUpdated(int ivIndex);

        void onFilterInitComplete();

        /**
         * mesh ota progress changed
         *
         * @param progress 0~100
         */
        void onMeshOTAProgressUpdate(int currentBlock, int totalBlock, int progress);

        /**
         * on ota firmware request
         *
         * @param maxLen
         * @return
         */
        byte[] onMeshOTAFirmwareRequest(int maxLen);


        void onMeshOtaApplyStatusNotify(int srcAdr, byte[] data);

        int onConnectedAdrRequest();
//        void onRemoteCapabilityGet(int maxItem, int activeScan);

//        void onRemoteScanReport(byte[] scanRecord);


    }

    public interface StorageHelper {
        byte[] onMeshStorageRetrieve(int len, int storageType);

        void onMeshStorageUpdate(byte[] buffer, int storageType);
    }

    public interface FastProvisionHelper {
        int onFastProvAdrRequest();

        void onFastProvNodeInfoCallback(byte[] devKey, int nodeAdr, int eleCnt);

        int onFastProvEleCntRequire(int pid);
    }

    public interface VendorHelper {

        /**
         * @deprecated
         */
        int setProvCloudParams(byte[] pid, byte[] mac);

        /**
         * 根据confirmKey和random计算本地confirm
         *
         * @param confirm
         * @param confirmKey
         * @param proRandom
         * @return
         */
        int setProvCloudConfirm(byte[] confirm, byte[] confirmKey, byte[] proRandom);

        /**
         * 根据confirmKey和random计算dev confirm， 比对devConfirm值，
         *
         * @param confirmKey
         * @param devRandom
         * @param devConfirm
         * @return 1， success
         */
        int cloudDevConfirmCheck(byte[] confirmKey, byte[] devRandom, byte[] devConfirm);

        void gattProvisionNetInfoCallback();
    }
}
