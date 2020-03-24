/********************************************************************************************************
 * @file TelinkMeshApplication.java
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
package com.telink.sig.mesh.demo;

import android.content.Intent;
import android.os.Build;
import android.os.Environment;

import com.telink.sig.mesh.TelinkApplication;
import com.telink.sig.mesh.demo.model.AddressRange;
import com.telink.sig.mesh.demo.model.LogInfo;
import com.telink.sig.mesh.demo.model.Mesh;
import com.telink.sig.mesh.event.MeshEvent;
import com.telink.sig.mesh.event.NotificationEvent;
import com.telink.sig.mesh.event.SettingEvent;
import com.telink.sig.mesh.light.CtlStatusNotificationParser;
import com.telink.sig.mesh.light.MeshController;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.light.ProvisionDataGenerator;
import com.telink.sig.mesh.light.StorageType;
import com.telink.sig.mesh.light.parser.OnlineStatusInfoParser;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.Group;
import com.telink.sig.mesh.model.NodeInfo;
import com.telink.sig.mesh.model.NotificationInfo;
import com.telink.sig.mesh.model.SigMeshModel;
import com.telink.sig.mesh.model.storage.CommonModelStorage;
import com.telink.sig.mesh.model.storage.ConfigModelStorage;
import com.telink.sig.mesh.model.storage.MeshKeyStorage;
import com.telink.sig.mesh.model.storage.MeshOTAModelStorage;
import com.telink.sig.mesh.model.storage.MiscStorage;
import com.telink.sig.mesh.util.Arrays;
import com.telink.sig.mesh.util.MeshUtils;
import com.telink.sig.mesh.util.TelinkLog;

import java.io.File;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

/**
 * Telinkg Mesh App
 * Created by kee on 2017/8/17.
 */
public class TelinkMeshApplication extends TelinkApplication {

    private static final String TAG = "TelinkMeshApplication";
    private static TelinkMeshApplication meshApplication;

    private Mesh mMesh;

    private List<LogInfo> logs;

    private boolean logEnable = false;

    private String localUUID;

    @Override
    public void onCreate() {
        super.onCreate();
        CrashHandler.init(this);
        meshApplication = this;
        TelinkLog.d("app create");
        initMesh();
        initMeshLib();
//        TelinkLog.d("Mesh network key:" + Arrays.bytesToHexString(mMesh.networkKey, ":"));
//        TelinkLog.d("Mesh app key:" + Arrays.bytesToHexString(mMesh.appKey, ":"));
//        TelinkLog.d("Mesh key:" + Arrays.bytesToHexString(getMeshLib().getMeshKey(), ":"));
        logs = new ArrayList<>();
        this.setLogEnable(SharedPreferenceHelper.isLogEnable(this));
        closePErrorDialog();

//        TelinkLog.d("netkey: " + Arrays.bytesToHexString(getMesh().networkKey, ""));
    }


    private void closePErrorDialog() {
        if (Build.VERSION.SDK_INT <= 27) {
            return;
        }
        try {
            Class aClass = Class.forName("android.content.pm.PackageParser$Package");
            Constructor declaredConstructor = aClass.getDeclaredConstructor(String.class);
            declaredConstructor.setAccessible(true);
        } catch (Exception e) {
            e.printStackTrace();
        }
        try {
            Class cls = Class.forName("android.app.ActivityThread");
            Method declaredMethod = cls.getDeclaredMethod("currentActivityThread");
            declaredMethod.setAccessible(true);
            Object activityThread = declaredMethod.invoke(null);
            Field mHiddenApiWarningShown = cls.getDeclaredField("mHiddenApiWarningShown");
            mHiddenApiWarningShown.setAccessible(true);
            mHiddenApiWarningShown.setBoolean(activityThread, true);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void initMesh() {
        Object object = FileSystem.readAsObject(this, Mesh.STORAGE_NAME);
        if (object == null) {
            mMesh = new Mesh();

            mMesh.networkKey = MeshUtils.generateRandom(16);
            mMesh.appKey = MeshUtils.generateRandom(16);
            List<Group> groups = new ArrayList<>();
            String[] groupNames = getResources().getStringArray(R.array.group_name);
            for (int i = 0; i < 8; i++) {
                Group group = new Group();
                group.address = i | 0xC000;
                group.name = groupNames[i];
                groups.add(group);
            }
            mMesh.groups = groups;

            mMesh.devices = new ArrayList<>();

            // init local address an provision index
            mMesh.localAddress = 1;
            mMesh.pvIndex = 1;
//            mMesh.selectedModels = new ArrayList<>();
            mMesh.selectedModels.addAll(java.util.Arrays.asList(SigMeshModel.getDefaultSelectList()));
            mMesh.uuid = Arrays.bytesToHexString(MeshUtils.generateRandom(16), "");
            mMesh.provisionerUUID = getLocalUUID();
            mMesh.unicastRange = new AddressRange(0x01, 0x03FF);
            mMesh.saveOrUpdate(this);
        } else {
            mMesh = (Mesh) object;
        }
    }


    public String getLocalUUID() {
        if (localUUID == null) {
            localUUID = SharedPreferenceHelper.getLocalUUID(this);
        }
        return localUUID;
    }

    public static TelinkMeshApplication getInstance() {
        return meshApplication;
    }

    public Mesh getMesh() {
        return mMesh;
    }

    public void setupMesh(Mesh mesh) {
//        if (!mesh.equals(mMesh)) {
        this.mMesh = mesh;
        dispatchEvent(new MeshEvent(this, MeshEvent.EVENT_TYPE_MESH_RESET, null));
//        }
    }

    @Override
    protected void onServiceCreated() {

        byte[] pvData = ProvisionDataGenerator.getProvisionData(mMesh.networkKey, mMesh.netKeyIndex, mMesh.ivUpdateFlag, mMesh.ivIndex, mMesh.localAddress);
        MeshService.getInstance().meshProvisionParSetDir(pvData, pvData.length);
//        mMesh.saveOrUpdate(getApplicationContext());
        MeshService.getInstance().setLocalAddress(mMesh.localAddress);
        MeshService.getInstance().resetAppKey(mMesh.appKeyIndex, mMesh.netKeyIndex, mMesh.appKey);
        /*List<byte[]> nodeList = new ArrayList<>();
        for (DeviceInfo node : mMesh.devices) {
            nodeList.add(node.nodeInfo.toVCNodeInfo());
        }
        byte[][] nodeData = nodeList.toArray(new byte[][]{});
        MeshService.getInstance().reattachNodes(nodeData);*/
//        MeshService.getInstance().resetAppKey(mMesh.appKeyIndex, mMesh.netKeyIndex, mMesh.appKey);
//            TelinkMeshApplication.getInstance().getMesh().localAddress = addr;
//            SharedPreferenceHelper.setFirst(this, false);
//        }

        super.onServiceCreated();
    }

    @Override
    protected void onMeshEvent(Intent intent) {
        switch (intent.getStringExtra(MeshController.EXTRA_EVENT_TYPE)) {
            case MeshEvent.EVENT_TYPE_DISCONNECTED:
                for (DeviceInfo tmpDev : mMesh.devices) {
                    tmpDev.setOnOff(-1);
                    tmpDev.lum = 0;
                    tmpDev.temp = 0;
                }
                break;
        }
        super.onMeshEvent(intent);
    }

    @Override
    protected void onSettingEvent(Intent intent) {
//        super.onSettingEvent(intent);
        final String type = intent.getStringExtra(MeshController.EXTRA_EVENT_TYPE);
        final int iv = intent.getIntExtra(SettingEvent.EVENT_TYPE_IV_UPDATE, -1);
        if (SettingEvent.EVENT_TYPE_IV_UPDATE.equals(type) && iv != -1) {
            if (mMesh != null) {
                mMesh.ivIndex = iv;
                mMesh.saveOrUpdate(this);
            }
        }
    }

    @Override
    protected void onOnlineStatusNotify(Intent intent) {
        byte[] data = intent.getByteArrayExtra(MeshController.EXTRA_NOTIFY_RAW);
        List<OnlineStatusInfoParser.OnlineStatusInfo> infoList = OnlineStatusInfoParser.create().parseInfo(data);
        if (infoList != null && mMesh != null) {
            for (OnlineStatusInfoParser.OnlineStatusInfo onlineStatusInfo : infoList) {
                if (onlineStatusInfo.status == null || onlineStatusInfo.status.length < 3) break;
                DeviceInfo deviceInfo = mMesh.getDeviceByMeshAddress(onlineStatusInfo.address);
                if (deviceInfo == null) continue;
                int onOff;
                if (onlineStatusInfo.sn == 0) {
                    onOff = -1;
                } else {
                    if (onlineStatusInfo.status[0] == 0) {
                        onOff = 0;
                    } else {
                        onOff = 1;
                    }


                }
                /*if (deviceInfo.getOnOff() != onOff){

                }*/
                deviceInfo.setOnOff(onOff);
                deviceInfo.lum = onlineStatusInfo.status[0];
                deviceInfo.temp = onlineStatusInfo.status[1];
            }
        }
        super.onOnlineStatusNotify(intent);
    }

    @Override
    protected void onNotificationRsp(Intent intent) {

        byte[] data = intent.getByteArrayExtra(MeshController.EXTRA_NOTIFY_RAW);
        NotificationInfo notificationInfo = (NotificationInfo) intent.getSerializableExtra(MeshController.EXTRA_NOTIFY_INFO);
        NotificationEvent event = new NotificationEvent(this, data, notificationInfo);
        boolean statusChanged = false;
        if (mMesh != null && mMesh.devices != null) {
//            NotificationInfo info = event.getNotificationInfo();
            if (notificationInfo == null || notificationInfo.params == null) return;
            byte[] params = notificationInfo.params;
            switch (event.getType()) {
                case NotificationEvent.EVENT_TYPE_DEVICE_ON_OFF_STATUS:
                    int onOff;
                    if (params.length == 1) {
                        onOff = params[0];
                    } else {
                        onOff = params[1];
                    }

                    for (DeviceInfo onlineDevice : mMesh.devices) {
                        if (onlineDevice.meshAddress == event.getNotificationInfo().srcAdr) {
                            statusChanged = true;
                            if (onlineDevice.getOnOff() != onOff) {
                                onlineDevice.setOnOff(onOff);
                            }
                            break;
                        }
                    }

                    break;


                case NotificationEvent.EVENT_TYPE_DEVICE_LEVEL_STATUS:
                    // 查找 该 element 下 的 亮度/色温 model

                    int srcAdr = event.getNotificationInfo().srcAdr;
                    short status;

                    // 是否带有目标值
                    if (params.length >= 4) {
                        status = (short) ((params[2] & 0xFF) | ((params[3] & 0xFF) << 8));
                    } else {
                        status = (short) ((params[0] & 0xFF) | ((params[1] & 0xFF) << 8));
                    }

                    byte tarVal = getMeshLib().level2Lum(status);
                    TelinkLog.d("lightness status val: " + tarVal + " -- " + status);
                    for (DeviceInfo onlineDevice : mMesh.devices) {
                        int deviceAdr = onlineDevice.meshAddress;
                        int updateType = -1;
                        if (onlineDevice.nodeInfo == null) {
                            continue;
                        }
                        for (NodeInfo.Element element : onlineDevice.nodeInfo.cpsData.elements) {
                            if (deviceAdr == srcAdr) {
                                if (element.containModel(SigMeshModel.SIG_MD_LIGHTNESS_S.modelId)) {
                                    updateType = 0;
                                    break;
                                } else if (element.containModel(SigMeshModel.SIG_MD_LIGHT_CTL_TEMP_S.modelId)) {
                                    updateType = 1;
                                    break;
                                }
                            }
                            deviceAdr++;
                        }

                        if (updateType == 0) {
                            // update lum
                            statusChanged = true;
                            TelinkLog.d("update type lum");
                            if (tarVal <= 0) {
                                onlineDevice.setOnOff(0);
                            } else {
                                onlineDevice.setOnOff(1);
                                onlineDevice.lum = tarVal;
                            }
                        } else if (updateType == 1) {
                            statusChanged = true;
                            // update temp
                            TelinkLog.d("update type temp");
                            onlineDevice.temp = tarVal;
                        }
                        /*if (onlineDevice.meshAddress == event.getNotificationInfo().srcAdr) {
                            if (lum <= 0) {
                                onlineDevice.onOff = 0;
                            } else {
                                onlineDevice.onOff = 1;
                                onlineDevice.lum = lum;
                            }

                            break;
                        }*/
                    }
                    break;

                case NotificationEvent.EVENT_TYPE_LIGHTNESS_STATUS_NOTIFY:
                    int liStatus;
                    if (params.length >= 4) {
                        liStatus = (short) ((params[2] & 0xFF) | ((params[3] & 0xFF) << 8));
                    } else {
                        liStatus = (short) ((params[0] & 0xFF) | ((params[1] & 0xFF) << 8));
                    }

                    int lightness = TelinkApplication.getInstance().getMeshLib().lightness2Lum(liStatus);
                    for (DeviceInfo onlineDevice : mMesh.devices) {
                        if (onlineDevice.meshAddress == event.getNotificationInfo().srcAdr) {
                            if (onlineDevice.getOnOff() == -1 || onlineDevice.lum != lightness) {
                                if (lightness <= 0) {
                                    onlineDevice.setOnOff(0);
                                } else {
                                    onlineDevice.setOnOff(1);
                                    onlineDevice.lum = lightness;
                                }
                                statusChanged = true;
                            }

                            break;
                        }
                    }
                    break;
                case NotificationEvent.EVENT_TYPE_CTL_STATUS_NOTIFY:
                    CtlStatusNotificationParser ctlParser = CtlStatusNotificationParser.create();
                    CtlStatusNotificationParser.CtlInfo ctlInfo = ctlParser.parse(params);
                    if (ctlInfo == null) return;
                    for (DeviceInfo onlineDevice : mMesh.devices) {
                        if (onlineDevice.meshAddress == event.getNotificationInfo().srcAdr) {
                            if (onlineDevice.getOnOff() == -1 || ctlInfo.lum != onlineDevice.lum || ctlInfo.temp != onlineDevice.temp) {
                                statusChanged = true;
                            }
                            if (ctlInfo.lum > 0) {
                                onlineDevice.setOnOff(1);
                                onlineDevice.lum = ctlInfo.lum;
                            } else {
                                onlineDevice.setOnOff(0);
                            }
                            onlineDevice.temp = ctlInfo.temp;
                            break;
                        }
                    }

                    break;


            }

        }
        event.setStatusChanged(statusChanged);
        dispatchEvent(event);

    }

    public void setLogEnable(boolean enable) {
        if (this.logEnable != enable) {
            this.logEnable = enable;
        }
    }


    public void saveLog(String action) {
        if (logEnable) {
            logs.add(new LogInfo(action));
        }
    }

    @Override
    public byte[] onMeshStorageRetrieve(int len, int storageType) {
        StorageType type = StorageType.valueOf(storageType);
        if (type == null) return null;
        return this.onMeshInfoRequired(len, type);
    }

    @Override
    public void onMeshStorageUpdate(byte[] buffer, int storageType) {
        StorageType type = StorageType.valueOf(storageType);
        if (type == null) return;
        this.onMeshInfoUpdate(buffer, type);
    }


    public byte[] onMeshInfoRequired(int len, StorageType type) {
        TelinkLog.d("onMeshInfoRequired: " + type + " len: " + len);
        switch (type) {
            case MESH_KEY:
                byte[] meshKeyData = getMeshKey().toBytes();
                TelinkLog.d("mesh key: " + Arrays.bytesToHexString(meshKeyData, ":"));
                return meshKeyData;

            case MISC:
                byte[] miscData = getMisc().toBytes();
                TelinkLog.d("misc: " + Arrays.bytesToHexString(miscData, ":"));
                return miscData;

            case NODE_INFO:
                byte[] nodeInfoData = getVCNodeInfo();
                TelinkLog.d("node info: " + Arrays.bytesToHexString(nodeInfoData, ":"));
                return nodeInfoData;

            case CONFIG_MODEL:
                byte[] configData = getConfigModel().toBytes();
                TelinkLog.d("config: " + Arrays.bytesToHexString(configData, ":"));
                return configData;

            case MESH_OTA:
                byte[] meshOtaModelData = getMeshOTAModelInfo().toBytes();
                TelinkLog.d("mesh ota model data: " + Arrays.bytesToHexString(meshOtaModelData, ":"));
                return meshOtaModelData;
        }
        return null;
    }

    public void onMeshInfoUpdate(byte[] data, StorageType type) {
        TelinkLog.d("onMeshInfoUpdate: " + type + " data: " + Arrays.bytesToHexString(data, ":"));
        switch (type) {
            case MESH_KEY:
                saveMeshKey(data);
                break;

            case MISC:
                saveMisc(data);
                break;
        }
    }

    public MeshKeyStorage getMeshKey() {
        MeshKeyStorage.MeshAppKeyStorage meshAppKeyStorage
                = MeshKeyStorage.MeshAppKeyStorage.getDefault(mMesh.appKey);

        MeshKeyStorage.MeshNetKeyStorage meshNetKeyStorage
                = MeshKeyStorage.MeshNetKeyStorage.getDefault(mMesh.networkKey, meshAppKeyStorage);

        return MeshKeyStorage.getDefault(meshNetKeyStorage);
    }

    public void saveMeshKey(byte[] data) {
        MeshKeyStorage meshKeyStorage = MeshKeyStorage.fromBytes(data);
        TelinkLog.d("save mesh key: " + meshKeyStorage.toString());

//        mMesh.appKey = appKey;
//        mMesh.networkKey = netKey;

//        mMesh.saveOrUpdate(this);
    }

    public MiscStorage getMisc() {
        return MiscStorage.getDefault(mMesh.sno, mMesh.ivIndex);
    }


    public void saveMisc(byte[] data) {

        MiscStorage miscStorage = MiscStorage.fromBytes(data);

        TelinkLog.d("save misc: " + miscStorage);
        if (miscStorage == null) return;
        // iv index saved as big endian in misc
        mMesh.sno = miscStorage.sno;
        mMesh.ivIndex = ((miscStorage.ivIndex[0] & 0xFF) << 24)
                | ((miscStorage.ivIndex[1] & 0xFF) << 16)
                | ((miscStorage.ivIndex[2] & 0xFF) << 8)
                | (miscStorage.ivIndex[3] & 0xFF);
        mMesh.saveOrUpdate(this);
    }

    public ConfigModelStorage getConfigModel() {
//        int modelId = SigMeshModel.SIG_MD_CFG_SERVER.modelId;
        CommonModelStorage commonModelStorage =
                CommonModelStorage.getDefault(mMesh.localAddress, mMesh.appKeyIndex);
        return ConfigModelStorage.getDefault(commonModelStorage);
    }


    public MeshOTAModelStorage getMeshOTAModelInfo() {
        return MeshOTAModelStorage.getDefault(mMesh.localAddress, mMesh.appKeyIndex);
    }

    public byte[] getVCNodeInfo() {
        if (mMesh.devices == null || mMesh.devices.size() == 0) {
            return null;
        }
        final int sLen = 404;
        byte[] re = new byte[sLen * mMesh.devices.size()];
        int index = 0;
        for (DeviceInfo node : mMesh.devices) {
            if (node.nodeInfo != null) {
                System.arraycopy(node.nodeInfo.toVCNodeInfo(), 0, re, index, sLen);
            } else {
                System.arraycopy(createSimpleVCNodeInfo(node.meshAddress, node.elementCnt, node.deviceKey), 0, re, index, sLen);
            }
            index += sLen;
        }
        return re;
    }

    public void saveLogInFile(String fileName, String logInfo) {
        File root = Environment.getExternalStorageDirectory();
        File dir = new File(root.getAbsolutePath() + File.separator + "TelinkSigMeshSetting");
        if (FileSystem.writeString(dir, fileName + ".txt", logInfo) != null) {
            TelinkLog.d("log saved in: " + fileName);
        }
    }

    public List<LogInfo> getLogInfo() {
        return logs;
    }

    public void clearLogInfo() {
        logs.clear();
    }

    public byte[] createSimpleVCNodeInfo(int nodeAdr, int elementCnt, byte[] deviceKey) {
        byte[] result = new byte[404];
        for (int i = 0; i < result.length; i++) {
            result[i] = (byte) 0xFF;
        }
        int index = 0;
        result[index++] = (byte) (nodeAdr & 0xFF);
        result[index++] = (byte) ((nodeAdr >> 8) & 0xFF);

        result[index++] = (byte) (elementCnt & 0xFF);

        result[index++] = 0;

        int keyLen = 16;
        System.arraycopy(deviceKey, 0, result, index, keyLen);
        return result;
    }
}
