package com.telink.ble.mesh;

import android.os.Build;
import android.os.Environment;
import android.os.Handler;
import android.os.HandlerThread;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.StatusMessage;
import com.telink.ble.mesh.core.message.generic.LevelStatusMessage;
import com.telink.ble.mesh.core.message.generic.OnOffStatusMessage;
import com.telink.ble.mesh.core.message.lighting.CtlStatusMessage;
import com.telink.ble.mesh.core.message.lighting.LightnessStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.OnlineStatusInfo;
import com.telink.ble.mesh.foundation.MeshApplication;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.event.NetworkInfoUpdateEvent;
import com.telink.ble.mesh.foundation.event.OnlineStatusEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.model.GroupInfo;
import com.telink.ble.mesh.model.LogInfo;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.model.UnitConvert;
import com.telink.ble.mesh.model.json.AddressRange;
import com.telink.ble.mesh.util.FileSystem;
import com.telink.ble.mesh.util.TelinkLog;

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
public class TelinkMeshApplication extends MeshApplication {

    private static TelinkMeshApplication mThis;

    private MeshInfo meshInfo;

    private List<LogInfo> logs = new ArrayList<>();

    private boolean logEnable = false;

    private Handler mOfflineCheckHandler;

    @Override
    public void onCreate() {
        super.onCreate();
        mThis = this;

        HandlerThread offlineCheckThread = new HandlerThread("offline check thread");
        offlineCheckThread.start();
        mOfflineCheckHandler = new Handler(offlineCheckThread.getLooper());
        initMesh();
        TelinkLog.d(meshInfo.toString());
        closePErrorDialog();
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

    public Handler getOfflineCheckHandler() {
        return mOfflineCheckHandler;
    }

    private void initMesh() {
        Object configObj = FileSystem.readAsObject(this, MeshInfo.FILE_NAME);
        if (configObj == null) {
            meshInfo = createNewMesh();
            saveMeshInfo();
        } else {
            meshInfo = (MeshInfo) configObj;
        }
    }

    public void setupMesh(MeshInfo mesh) {
        this.meshInfo = mesh;
        dispatchEvent(new MeshEvent(this, MeshEvent.EVENT_TYPE_MESH_RESET, "mesh reset"));
    }

    public void saveMeshInfo() {
        FileSystem.writeAsObject(this, MeshInfo.FILE_NAME, meshInfo);
    }

    public MeshInfo getMeshInfo() {
        return meshInfo;
    }

    private MeshInfo createNewMesh() {
        // 0x7FFF
        final int DEFAULT_LOCAL_ADDRESS = 0x0001;
        MeshInfo meshInfo = new MeshInfo();

        // for test
//        final byte[] NET_KEY = Arrays.hexToBytes("26E8D2DBD4363AF398FEDE049BAD0086");

        // for test
//        final byte[] APP_KEY = Arrays.hexToBytes("7759F48730A4F1B2259B1B0681BE7C01");

//        final int IV_INDEX = 0x20345678;

//        meshInfo.networkKey = NET_KEY;
        meshInfo.networkKey = MeshUtils.generateRandom(16);
        meshInfo.netKeyIndex = 0x00;
        meshInfo.appKeyList = new ArrayList<>();
//        meshInfo.appKeyList.add(new MeshInfo.AppKey(0x00, APP_KEY));
        meshInfo.appKeyList.add(new MeshInfo.AppKey(0x00, MeshUtils.generateRandom(16)));
        meshInfo.ivIndex = 0;
        meshInfo.sequenceNumber = 0;
        meshInfo.nodes = new ArrayList<>();
        meshInfo.localAddress = DEFAULT_LOCAL_ADDRESS;
        meshInfo.provisionIndex = DEFAULT_LOCAL_ADDRESS + 1; // 0x0002
        meshInfo.provisionerUUID = getLocalUUID();
        meshInfo.groups = new ArrayList<>();
        meshInfo.unicastRange = new AddressRange(0x01, 0xFF);
        String[] groupNames = getResources().getStringArray(R.array.group_name);
        GroupInfo group;
        for (int i = 0; i < 8; i++) {
            group = new GroupInfo();
            group.address = i | 0xC000;
            group.name = groupNames[i];
            meshInfo.groups.add(group);
        }

        return meshInfo;
    }

    public static TelinkMeshApplication getInstance() {
        return mThis;
    }

    public String getLocalUUID() {
        return SharedPreferenceHelper.getLocalUUID(this);
    }

    public void saveLog(String action) {
        if (logEnable) {
            logs.add(new LogInfo(action));
        }
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

    @Override
    protected void onMeshEvent(MeshEvent autoConnectEvent) {
        String eventType = autoConnectEvent.getType();
        if (MeshEvent.EVENT_TYPE_DISCONNECTED.equals(eventType)) {
            for (NodeInfo nodeInfo : meshInfo.nodes) {
                nodeInfo.setOnOff(NodeInfo.ON_OFF_STATE_OFFLINE);
            }
        }
    }

    @Override
    protected void onStatusNotificationEvent(StatusNotificationEvent statusNotificationEvent) {
        NotificationMessage message = statusNotificationEvent.getNotificationMessage();

        StatusMessage statusMessage = message.getStatusMessage();
        if (statusMessage != null) {
            NodeInfo statusChangedNode = null;
            if (message.getStatusMessage() instanceof OnOffStatusMessage) {
                OnOffStatusMessage onOffStatusMessage = (OnOffStatusMessage) statusMessage;
                int onOff = onOffStatusMessage.isComplete() ? onOffStatusMessage.getTargetOnOff() : onOffStatusMessage.getPresentOnOff();
                for (NodeInfo nodeInfo : meshInfo.nodes) {
                    if (nodeInfo.meshAddress == message.getSrc()) {
                        if (nodeInfo.getOnOff() != onOff) {
                            statusChangedNode = nodeInfo;
                            nodeInfo.setOnOff(onOff);
                        }
                        break;
                    }
                }
            } else if (message.getStatusMessage() instanceof LevelStatusMessage) {
                LevelStatusMessage levelStatusMessage = (LevelStatusMessage) statusMessage;
                int srcAdr = message.getSrc();
                int level = levelStatusMessage.isComplete() ? levelStatusMessage.getTargetLevel() : levelStatusMessage.getPresentLevel();
                int tarVal = UnitConvert.level2lum((short) level);
                for (NodeInfo onlineDevice : meshInfo.nodes) {
                    if (onlineDevice.compositionData == null) {
                        continue;
                    }
                    int lightnessEleAdr = onlineDevice.getTargetEleAdr(MeshSigModel.SIG_MD_LIGHTNESS_S.modelId);
                    if (lightnessEleAdr == srcAdr) {
                        if (onLumStatus(onlineDevice, tarVal)) {
                            statusChangedNode = onlineDevice;
                        }

                    } else {
                        int tempEleAdr = onlineDevice.getTargetEleAdr(MeshSigModel.SIG_MD_LIGHT_CTL_TEMP_S.modelId);
                        if (tempEleAdr == srcAdr) {
                            if (onlineDevice.temp != tarVal) {
                                statusChangedNode = onlineDevice;
                                onlineDevice.temp = tarVal;
                            }
                        }
                    }
                }
            } else if (message.getStatusMessage() instanceof CtlStatusMessage) {
                CtlStatusMessage ctlStatusMessage = (CtlStatusMessage) statusMessage;
                int srcAdr = message.getSrc();
                for (NodeInfo onlineDevice : meshInfo.nodes) {
                    if (onlineDevice.meshAddress == srcAdr) {
                        int lum = ctlStatusMessage.isComplete() ? ctlStatusMessage.getTargetLightness() : ctlStatusMessage.getPresentLightness();
                        if (onLumStatus(onlineDevice, UnitConvert.lightness2lum(lum))) {
                            statusChangedNode = onlineDevice;
                        }
//                        onlineDevice.temp = ctlInfo.temp;
                        break;
                    }
                }
            } else if (message.getStatusMessage() instanceof LightnessStatusMessage) {
                LightnessStatusMessage lightnessStatusMessage = (LightnessStatusMessage) statusMessage;
                int srcAdr = message.getSrc();
                for (NodeInfo onlineDevice : meshInfo.nodes) {
                    if (onlineDevice.meshAddress == srcAdr) {
                        int lum = lightnessStatusMessage.isComplete() ? lightnessStatusMessage.getTargetLightness() : lightnessStatusMessage.getPresentLightness();
                        if (onLumStatus(onlineDevice, UnitConvert.lightness2lum(lum))) {
                            statusChangedNode = onlineDevice;
                        }
                        break;
                    }
                }
            }

            if (statusChangedNode != null) {
                onNodeInfoStatusChanged(statusChangedNode);
            }
        }
    }


    private boolean onLumStatus(NodeInfo nodeInfo, int lum) {
        boolean statusChanged = false;
        int tarOnOff = lum > 0 ? 1 : 0;
        if (nodeInfo.getOnOff() != tarOnOff) {
            statusChanged = true;
            nodeInfo.setOnOff(tarOnOff);
        }
        if (nodeInfo.lum != lum) {
            statusChanged = true;
            nodeInfo.lum = lum;
        }
        return statusChanged;
    }


    /**
     * node info status changed for UI refresh
     */
    private void onNodeInfoStatusChanged(NodeInfo nodeInfo) {
        dispatchEvent(new NodeStatusChangedEvent(this, NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, nodeInfo));
    }

    @Override
    protected void onOnlineStatusEvent(OnlineStatusEvent onlineStatusEvent) {
        List<OnlineStatusInfo> infoList = onlineStatusEvent.getOnlineStatusInfoList();
        if (infoList != null && meshInfo != null) {
            NodeInfo statusChangedNode = null;
            for (OnlineStatusInfo onlineStatusInfo : infoList) {
                if (onlineStatusInfo.status == null || onlineStatusInfo.status.length < 3) break;
                NodeInfo deviceInfo = meshInfo.getDeviceByMeshAddress(onlineStatusInfo.address);
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
                if (deviceInfo.getOnOff() != onOff) {
                    statusChangedNode = deviceInfo;
                    deviceInfo.setOnOff(onOff);
                }
                if (deviceInfo.lum != onlineStatusInfo.status[0]) {
                    statusChangedNode = deviceInfo;
                    deviceInfo.lum = onlineStatusInfo.status[0];
                }

                if (deviceInfo.temp != onlineStatusInfo.status[1]) {
                    statusChangedNode = deviceInfo;
                    deviceInfo.temp = onlineStatusInfo.status[1];
                }
            }
            if (statusChangedNode != null) {
                onNodeInfoStatusChanged(statusChangedNode);
            }
        }
    }

    /**
     * save sequence number and iv index when mesh info updated
     */
    protected void onNetworkInfoUpdate(NetworkInfoUpdateEvent networkInfoUpdateEvent) {
        this.meshInfo.ivIndex = networkInfoUpdateEvent.getIvIndex();
        this.meshInfo.sequenceNumber = networkInfoUpdateEvent.getSequenceNumber();
        this.meshInfo.saveOrUpdate(this);
    }


}
