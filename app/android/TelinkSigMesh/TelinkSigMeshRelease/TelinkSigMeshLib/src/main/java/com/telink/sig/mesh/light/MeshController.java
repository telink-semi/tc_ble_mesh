/********************************************************************************************************
 * @file MeshController.java
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
package com.telink.sig.mesh.light;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGattService;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.ParcelUuid;
import android.os.Process;
import android.support.v4.content.LocalBroadcastManager;
import android.text.TextUtils;

import com.telink.sig.mesh.TelinkApplication;
import com.telink.sig.mesh.ble.AdvertisingDevice;
import com.telink.sig.mesh.ble.Device;
import com.telink.sig.mesh.ble.LeScanFilter;
import com.telink.sig.mesh.ble.MeshScanRecord;
import com.telink.sig.mesh.ble.UnprovisionedDevice;
import com.telink.sig.mesh.event.CommandEvent;
import com.telink.sig.mesh.event.MeshEvent;
import com.telink.sig.mesh.event.MeshOtaEvent;
import com.telink.sig.mesh.event.OtaEvent;
import com.telink.sig.mesh.event.ScanEvent;
import com.telink.sig.mesh.event.SettingEvent;
import com.telink.sig.mesh.lib.MeshLib;
import com.telink.sig.mesh.light.parameter.AutoConnectParameters;
import com.telink.sig.mesh.light.parameter.FastProvisionParameters;
import com.telink.sig.mesh.light.parameter.GattOtaParameters;
import com.telink.sig.mesh.light.parameter.KeyBindParameters;
import com.telink.sig.mesh.light.parameter.Parameters;
import com.telink.sig.mesh.light.parameter.ProvisionParameters;
import com.telink.sig.mesh.light.parameter.RemoteProvisionParameters;
import com.telink.sig.mesh.light.parser.NotificationInfoParser;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.MeshCommand;
import com.telink.sig.mesh.model.NodeInfo;
import com.telink.sig.mesh.model.NotificationInfo;
import com.telink.sig.mesh.model.OpcodeType;
import com.telink.sig.mesh.model.OtaDeviceInfo;
import com.telink.sig.mesh.model.RemoteDevice;
import com.telink.sig.mesh.util.Arrays;
import com.telink.sig.mesh.util.Encipher;
import com.telink.sig.mesh.util.MeshUtils;
import com.telink.sig.mesh.util.TelinkLog;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.UUID;
import java.util.concurrent.CopyOnWriteArraySet;
import java.util.concurrent.atomic.AtomicBoolean;

public class MeshController {

    private static final String TAG = "MeshController";

    /**
     * mesh command return value
     */
    private static final int CMD_RESULT_OK = 0;

    /**
     * set local provision data error
     */
    private static final byte MSG_COMPOSITE_ERR = (byte) 0xfe;


    /**
     * provision process callback type
     * PROV_NORMAL_RET: success
     * PROV_TIMEOUT_RET: timeout
     */
    private static final byte PROV_NORMAL_RET = 0;
    private static final byte PROV_TIMEOUT_RET = 1;

    /**
     * app key bind process callback type
     * MESH_APP_KEY_BIND_EVENT_SUC: success
     * MESH_APP_KEY_BIND_EVENT_TIMEOUT: timeout
     */
    private static final byte MESH_APP_KEY_BIND_EVENT_SUC = 1;
    private static final byte MESH_APP_KEY_BIND_EVENT_TIMEOUT = 0;

    /**
     * command type provision
     */
    private static final int TYPE_PROVISION = 0x10;

    /**
     * command type proxy
     */
    private static final int TYPE_CONTROL = 0x11;

    /**
     * Main Loop <b>thread</b>
     * Looper for MeshLib dealing with package fifo
     */
    private HandlerThread mLooperThread = null;

    /**
     * Main Loop Handler
     */
    private Handler mLoopHandler = null;

    /**
     * Loop interval, unit: ms
     */
    private static final long MESH_MAIN_PROCESS_CYCLE = 2;


    /**
     * Delay handler for custom runnable
     */
    private Handler mDelayHandler = null;

    /**
     * MeshLib created by TelinkApplication {@link TelinkApplication#getMeshLib()}
     */
    private MeshLib meshLib;

    /**
     * App local address
     */
    private int localAddress = 1;

    /**
     * Scan action params
     */
    private long lastScanStartTime = 0;

//    private Handler mScanDelayHandler;

    /*
     * Action modes
     *
     *
     * MODE_IDLE: no scan and connect actions
     */
    private static final int MODE_IDLE = 0x00;


    private static final int MODE_SCAN = 0x01;

    /**
     * MODE_PROVISION: auto scan, connect, provision
     */
    private static final int MODE_PROVISION = 0x02;

    /**
     * MODE_AUTO_CONNECT: auto scan, connect, get device state
     */
    private static final int MODE_AUTO_CONNECT = 0x03;

    /**
     * MODE_OTA: auto scan, connect, start ota
     */
    private static final int MODE_OTA = 0x04;

    /**
     * bind app key
     * connect target device without scan
     */
    private static final int MODE_BIND = 0x05;


    private static final int MODE_REMOTE_PROVISION = 0x06;


    private static final int MODE_REMOTE_BIND = 0x07;

    private static final int MODE_FAST_PROVISION = 0x08;


    private int actionMode = MODE_IDLE;


    // for N version
    private static final long DEFAULT_MIN_SCAN_SPACING = 6 * 1000;

    // for rssi filter
    private static final long DEFAULT_MIN_SCAN_PERIOD = 3 * 1000;

    private DeviceScanCallback mScanCallback;
    // rssi collections
    private Map<BluetoothDevice, List<Integer>> mRssiColl;

    private boolean disconnectWaiting = false;
    // is rssi collecting
    private boolean isRssiCollecting;
    /**
     * current operating device
     */
    private Device mDevice;

    private DeviceCallbackImpl mDeviceCallback;

    /**
     * scanning tag
     */
    private AtomicBoolean isScanning = new AtomicBoolean(false);

    /**
     * connecting tag
     */
//    private AtomicBoolean connecting = new AtomicBoolean(false);

    private Context mContext;

    /**
     * default scan timeout
     */
    private static final long DEFAULT_SCAN_TIMEOUT = 10 * 1000;

    /**
     * default scan rest
     */
    private static final long DEFAULT_SCAN_REST = 3 * 1000;

    /**
     * is provision processing
     */
    private boolean isPvProcessing = false;

    private boolean isPvWaiting = false;

    /**
     * key bind running...
     */
    private boolean isKeyBindProcessing = false;

    /**
     * events
     */
    public static final String ACTION_SCAN_EVENT = "com.telink.sig.mesh.ACTION_SCAN_EVENT";

    public static final String ACTION_MESH_EVENT = "com.telink.sig.mesh.ACTION_MESH_EVENT";

    public static final String ACTION_COMMAND_EVENT = "com.telink.sig.mesh.ACTION_COMMAND_EVENT";

    public static final String ACTION_SETTING_EVENT = "com.telink.sig.mesh.ACTION_SETTING_EVENT";

    public static final String ACTION_STATUS_RESPONSE = "com.telink.sig.mesh.ACTION_STATUS_RESPONSE";

    public static final String ACTION_ONLINE_STATUS = "com.telink.sig.mesh.ACTION_ONLINE_STATUS";

    public static final String ACTION_OTA = "com.telink.sig.mesh.ACTION_OTA";

    public static final String ACTION_MESH_OTA = "com.telink.sig.mesh.ACTION_MESH_OTA";

    public static final String EXTRA_EVENT_TYPE = "com.telink.sig.mesh.EXTRA_EVENT_TYPE";

    public static final String EXTRA_DEVICE_INFO = "com.telink.sig.mesh.EXTRA_DEVICE_INFO";

    public static final String EXTRA_ADV_DEVICE = "com.telink.sig.mesh.EXTRA_ADV_DEVICE";

    public static final String EXTRA_EVENT_DESC = "com.telink.sig.mesh.EXTRA_EVENT_DESC";

    public static final String EXTRA_NOTIFY_INFO = "com.telink.sig.mesh.EXTRA_NOTIFY_INFO";

    public static final String EXTRA_NOTIFY_RAW = "com.telink.sig.mesh.EXTRA_NOTIFY_RAW";

    public static final String EXTRA_MESH_COMMAND = "com.telink.sig.mesh.EXTRA_MESH_COMMAND";

    public static final String EXTRA_SETTING_INFO = "com.telink.sig.mesh.EXTRA_SETTING_INFO";

    public static final String EXTRA_MESH_OTA_PROGRESS = "com.telink.sig.mesh.EXTRA_MESH_OTA_PROGRESS";

    public static final String EXTRA_MESH_OTA_APPLY_SRC = "com.telink.sig.mesh.EXTRA_MESH_OTA_APPLY_SRC";

    public static final String EVENT_TYPE_SERVICE_CREATE = "com.telink.sig.mesh.SERVICE_CREATE";

    public static final String EVENT_TYPE_SERVICE_DESTROY = "com.telink.sig.mesh.SERVICE_DESTROY";


    /**
     * connect timeout
     */
    private static final long CONNECTION_TIMEOUT = 10 * 1000;

    /**
     * connect retry
     */
    private static final int MAX_CONNECT_RETRY = 3;

    /**
     * scanned device list
     */
    private Set<AdvertisingDevice> advDevices;

    //    private Map<String, AtomicInteger> connectRetryDevices;
    private int connectRetry = 0;

    /**
     * 自动连接下的初始化状态， 分步 1 get ctl, 2 set time
     * deprecated
     */
//    private int initStep = 0;


    /**
     * 需要provision的设备列表
     */
    private UnprovisionedDevice mProvisioningDevice;
//    private List<UnprovisionedDevice> provisionTargets;

//    private AtomicInteger provisioningIndex = new AtomicInteger(0);


    /**
     * auto connect targets
     */
//    private Set<String> autoConnectTargets;

    private Parameters mParams;

    private byte[] meshOtaFirmware;

    /**
     * processing reliable mesh command
     */
    private MeshCommand processingCmd;

    private final Object CMD_LOCK = new Object();

    private int connectedMeshAdr;


    private final static int PROXY_ADV_TYPE_NETWORK_ID = 0x00;

    private final static int PROXY_ADV_TYPE_NODE_IDENTITY = 0x01;

    /**
     * network id , generated by k3(network key)
     */
    private byte[] networkId = null;

    private byte[] identityKey = null;


    private int fastProvisionIndex = 1;
    private int fastPvElementCount = 0;

    private boolean isLogin = false;

    protected void start(Context context) {

        saveLog("#start");
        this.mContext = context;

        meshLib = ((TelinkApplication) this.mContext.getApplicationContext()).getMeshLib();

//        mesh = TelinkApplication.getInstance().getMesh();
//        TelinkLog.w("NetWorkKey -- " + Arrays.bytesToHexString(mesh.networkKey, ":"));
        meshLib.setLibHelper(meshLibHelper);
        meshLib.setFastProvisionHelper(fastProvisionHelper);
        mLooperThread = new HandlerThread("mesh_main_loop_thread", Process.THREAD_PRIORITY_LOWEST);
        mLooperThread.start();
        mLoopHandler = new Handler(mLooperThread.getLooper());
        mLoopHandler.post(meshMainProcess);

        mDelayHandler = new Handler();
//        mScanDelayHandler = new Handler();
        mScanCallback = new DeviceScanCallback();
        LeBluetooth.getInstance().setLeScanCallback(mScanCallback);
        mProvisioningDevice = null;
//        provisionTargets = new ArrayList<>();
        mRssiColl = new LinkedHashMap<>();
        connectRetry = 0;
        advDevices = new CopyOnWriteArraySet<>();

        mDevice = new Device(context);
        mDeviceCallback = new DeviceCallbackImpl();
        mDevice.setCallback(mDeviceCallback);
    }

    public void stop() {
        saveLog("#stop");
        this.actionMode = MODE_IDLE;
        stopScan();
        if (mLoopHandler != null) {
            mLoopHandler.removeCallbacksAndMessages(null);
            mLoopHandler = null;
        }

        if (mLooperThread != null) {
            mLooperThread.quitSafely();
            mLooperThread = null;
        }

        meshLib = null;

        mScanCallback = null;
        LeBluetooth.getInstance().setLeScanCallback(null);

        if (mDeviceCallback != null) {
            mDeviceCallback.clear();
        }

        if (mDevice != null) {
            mDevice.disconnect();
            mDevice.setCallback(null);
            mDevice = null;
        }

//        if (provisionTargets != null) {
//            provisionTargets.clear();
//        }


        if (mRssiColl != null) {
            mRssiColl.clear();
        }

        if (advDevices != null) {
            advDevices.clear();
        }

        if (mDelayHandler != null) {
            mDelayHandler.removeCallbacksAndMessages(null);
        }
    }

    public void setMeshOtaFirmware(byte[] meshOtaFirmware) {
        this.meshOtaFirmware = meshOtaFirmware;
    }

    public void setConnectedMeshAdr(int connectedMeshAdr) {
        this.connectedMeshAdr = connectedMeshAdr;
    }

    //action single scan
    public void startScan(ScanParameters parameters) {
        if (this.actionMode == MODE_SCAN) return;

        this.actionMode = MODE_SCAN;
        advDevices.clear();
        this.mParams = parameters;

        if (mDevice == null || !mDevice.disconnect()) {
            startScan();
        } else {
            saveLog("waiting for disconnect...");
        }
    }


    public void autoConnect(AutoConnectParameters parameters) {
        if (this.actionMode == MODE_AUTO_CONNECT) {
            this.updateAutoConnectParams(parameters);
            return;
        }
        mDelayHandler.removeCallbacksAndMessages(null);
        saveLog("start auto connect action");
        this.actionMode = MODE_AUTO_CONNECT;
        this.mParams = parameters;
        if (checkConnectTargets()) {

            if (mDevice != null && mDevice.isProxyNodeConnected()) {

                if (isLogin) {
                    onAutoConnectSuccess();
                } else {
                    filterInit(localAddress);
                }
                return;
            }
        } else {
            return;
        }

        if (mDevice == null || !mDevice.disconnect()) {
            startScan();
        } else {
            saveLog("waiting for disconnect...");
        }
    }

    private boolean checkConnectTargets() {
        if (!validateMeshConnectable()) {
            mDelayHandler.removeCallbacksAndMessages(null);
            onMeshEvent(MeshEvent.EVENT_TYPE_MESH_EMPTY, "mesh empty");
            idle(true);
            return false;
        }
        return true;
    }

    public boolean validateMeshConnectable() {
        Object targets = mParams.get(Parameters.ACTION_AUTO_CONNECT_TARGET_LIST);
        if (targets != null && ((Set<String>) targets).size() != 0) {
            return true;
        }

        Object nodes = mParams.get(Parameters.ACTION_AUTO_CONNECT_NODE_ADDRESS_LIST);
        if (nodes != null && ((int[]) nodes).length != 0) {
            Object networkKey = mParams.get(Parameters.ACTION_AUTO_CONNECT_NETWORK_KEY);
            if (networkKey != null) {

                byte[] netKey = (byte[]) networkKey;
                this.identityKey = Encipher.generateIdentityKey(netKey);
                this.networkId = Encipher.k3(netKey);
                return true;
            }
        }
        return false;
    }


    public boolean updateAutoConnectParams(AutoConnectParameters parameters) {
        if (this.actionMode != MODE_AUTO_CONNECT) return false;
        this.mParams = parameters;
        checkConnectTargets();
        return true;
    }

    // action startProvision
    public void startProvision(ProvisionParameters parameters) {
        if (this.actionMode == MODE_PROVISION) return;
        saveLog("start provision action");

        this.actionMode = MODE_PROVISION;
        this.isPvProcessing = false;
        this.mParams = parameters;

//        pvIgnoreList.clear();

        if (mParams.get(Parameters.ACTION_PROVISION_TARGET) != null) {
            mProvisioningDevice = (UnprovisionedDevice) mParams.get(Parameters.ACTION_PROVISION_TARGET);
        } else {
            saveLog("provision params err!");
            this.actionMode = MODE_IDLE;
            return;
        }

        mDelayHandler.removeCallbacksAndMessages(null);
        connectRetry = 0;

        if (mDevice == null || !mDevice.disconnect()) {
            connectProvisioningDevice();
        } else {
            isPvWaiting = true;
            disconnectWaiting = true;
        }
    }

    public void startKeyBind(KeyBindParameters bindParameters) {
//        if (this.actionMode == )
        saveLog("start key bind action");
        this.mParams = bindParameters;
        mDelayHandler.removeCallbacksAndMessages(null);
        isKeyBindProcessing = false;
        actionMode = MODE_BIND;
        connectRetry = 0;
        DeviceInfo deviceInfo = (DeviceInfo) mParams.get(Parameters.ACTION_BIND_DEVICE_INFO);
        // validPrivate(deviceInfo)
        if (mDevice != null && mDevice.isProxyNodeConnected() && deviceInfo.macAddress.equals(mDevice.getMacAddress())) {
            keyBindPrepare();
        } else {
            if (mDevice == null || !mDevice.disconnect()) {
                startScan();
            } else {
                disconnectWaiting = true;
                saveLog("waiting for disconnect...");
            }
        }

    }

    public void startRemoteProvision(RemoteProvisionParameters parameters) {
        saveLog("start remote provision");
        this.actionMode = MODE_REMOTE_PROVISION;
        this.mParams = parameters;

        if (mDevice == null || !mDevice.isConnected()) {
            onRemoteProvisionFail("device not connected");
            return;
        }
        mDelayHandler.removeCallbacksAndMessages(null);
        final RemoteDevice remoteDevice = (RemoteDevice) mParams.get(Parameters.ACTION_REMOTE_PROVISION_DEVICE);
        saveLog("remote provision target: " + remoteDevice.toString());

        meshLib.initRemoteProvision(remoteDevice.src, remoteDevice.uuid);
//        mDelayHandler.postDelayed(linkOpenTimeoutTask, 3 * 1000);
    }

    protected void startRemoteBind(KeyBindParameters parameters) {
        saveLog("startRemoteBind");

        if (mDevice == null || !mDevice.isConnected()) {
            onRemoteBindFail("device not connected");
            return;
        }
        mDelayHandler.removeCallbacksAndMessages(null);
        this.actionMode = MODE_REMOTE_BIND;
        this.mParams = parameters;
        bindAppKey();
    }

    /**
     *
     */
    void startFastProvision(FastProvisionParameters provisionParameters) {
        if (this.actionMode == MODE_FAST_PROVISION) {
            return;
        }

        this.actionMode = MODE_FAST_PROVISION;
        this.fastProvisionIndex = provisionParameters.getInt(Parameters.ACTION_FAST_PROVISION_ADDRESS, 0);
        this.fastPvElementCount = provisionParameters.getInt(Parameters.ACTION_FAST_PROVISION_ELE_CNT, 0);

        if (this.fastProvisionIndex == 0) {
            this.actionMode = MODE_IDLE;
            onFastProvisionFail();
            return;
        }
        connectRetry = 0;
        this.mParams = provisionParameters;
        if (mDevice == null || !mDevice.disconnect()) {
            startScan();
        } else {
            disconnectWaiting = true;
            saveLog("waiting for disconnect...");
        }
    }

    private void onRemoteProvisionSuccess() {
        sendRemoteProvisionEvent(MeshEvent.EVENT_TYPE_REMOTE_PROVISION_SUCCESS);
    }

    private void onRemoteProvisionFail(String desc) {
        saveLog("remote provision fail: " + desc);
        sendRemoteProvisionEvent(MeshEvent.EVENT_TYPE_REMOTE_PROVISION_FAIL);
    }

    private void onRemoteBindSuccess() {
        sendRemoteBindEvent(MeshEvent.EVENT_TYPE_REMOTE_BIND_SUCCESS);
    }

    private void onRemoteBindFail(String desc) {
        saveLog("remote bind fail: " + desc);
        sendRemoteBindEvent(MeshEvent.EVENT_TYPE_REMOTE_BIND_FAIL);
    }

    private void sendRemoteProvisionEvent(String type) {
        Intent intent = new Intent();
        DeviceInfo deviceInfo = new DeviceInfo();

        RemoteDevice remoteDevice = (RemoteDevice) mParams.get(Parameters.ACTION_REMOTE_PROVISION_DEVICE);
        deviceInfo.meshAddress = remoteDevice.pvAddress;
        deviceInfo.macAddress = remoteDevice.mac;
        saveLog("remote provision event: " + deviceInfo.macAddress + " -- " + type);
        if (type.equals(MeshEvent.EVENT_TYPE_REMOTE_PROVISION_SUCCESS)) {
            deviceInfo.elementCnt = meshLib.getElementCnt(remoteDevice.pvAddress);
            deviceInfo.deviceKey = meshLib.getCurrentDeviceKey();
        }
        intent.setAction(ACTION_MESH_EVENT);
        intent.putExtra(EXTRA_DEVICE_INFO, deviceInfo);
        intent.putExtra(EXTRA_EVENT_TYPE, type);

        LocalBroadcastManager.getInstance(mContext)
                .sendBroadcast(intent);
    }

    private void sendRemoteBindEvent(String type) {
        DeviceInfo deviceInfo = (DeviceInfo) mParams.get(Parameters.ACTION_BIND_DEVICE_INFO);
        if (type.equals(MeshEvent.EVENT_TYPE_REMOTE_BIND_SUCCESS)) {
            if (!validPrivate(deviceInfo)) {
                byte[] nodeInfoData = meshLib.getNodeInfo(deviceInfo.meshAddress);
                deviceInfo.nodeInfo = NodeInfo.from(nodeInfoData);
            }
        }
        Intent intent = new Intent();
        intent.setAction(ACTION_MESH_EVENT);
        intent.putExtra(EXTRA_DEVICE_INFO, deviceInfo);
        intent.putExtra(EXTRA_EVENT_TYPE, type);

        LocalBroadcastManager.getInstance(mContext)
                .sendBroadcast(intent);
    }

    private void connectProvisioningDevice() {
        connectDevice(mProvisioningDevice.device, mProvisioningDevice.scanRecord, mProvisioningDevice.rssi);
    }


    private boolean validPrivate(DeviceInfo deviceInfo) {
        boolean fast = mParams.getBool(Parameters.ACTION_BIND_FAST_BIND, false);
        return fast && deviceInfo.nodeInfo != null;
    }

    public void startOta(GattOtaParameters gattOtaParameters) {

        if (this.actionMode == MODE_OTA) return;

        mDelayHandler.removeCallbacksAndMessages(null);
//        connectRetryDevices.clear();
        connectRetry = 0;
        this.mParams = gattOtaParameters;
        String otaMac = gattOtaParameters.getString(Parameters.ACTION_OTA_MAC);
        byte[] otaFirm = (byte[]) gattOtaParameters.get(Parameters.ACTION_OTA_FIRMWARE);
        this.actionMode = MODE_OTA;
        if (mDevice != null && mDevice.isConnected()) {
            if (mDevice.getMacAddress().equals(otaMac)) {
                mDevice.startOta(otaFirm);
            } else {
                mDevice.disconnect();
            }
        } else {
            startScan();
        }
    }

    public boolean getOnlineStatus() {
        return mDevice != null && mDevice.enableOnlineStatus();
    }

    /**
     * @param deviceInfo
     * @return command if sent
     */
    public boolean kickOut(DeviceInfo deviceInfo) {
        TelinkLog.d("Kick out:" + deviceInfo.meshAddress + " -- " + deviceInfo.macAddress);
        boolean result = this.cmdDeleteNode(deviceInfo.meshAddress);
        return result;
    }


    public void idle(boolean disconnect) {
        saveLog("---idle---");
        this.actionMode = MODE_IDLE;
        if (disconnect && mDevice != null) {
            mDevice.disconnect();
        }
        stopScan();
    }

    public String getCurDeviceMac() {
        if (mDevice != null && mDevice.isConnected()) {
            return mDevice.getMacAddress();
        }
        return null;
    }

    private void stopScan() {
        saveLog("StopScan");
        isScanning.set(false);
        mDelayHandler.removeCallbacks(scanTimeoutTask);
        LeBluetooth.getInstance().stopScan();
    }

    private void startScan() {
        saveLog("start scan check");
        stopScan();
        mDelayHandler.removeCallbacks(scanTimeoutTask);

        mDelayHandler.removeCallbacks(rssiCheckTask);
        long scanDelay = 0;
        long minScanSpacing = mParams.getLong(Parameters.COMMON_SCAN_MIN_SPACING, DEFAULT_MIN_SCAN_SPACING);
        if (minScanSpacing > 0 && System.currentTimeMillis() - lastScanStartTime < minScanSpacing) {
            scanDelay = minScanSpacing - (System.currentTimeMillis() - lastScanStartTime);
            if (scanDelay > minScanSpacing) scanDelay = minScanSpacing;
        }

        mDelayHandler.removeCallbacks(startScanTask);
        mDelayHandler.postDelayed(startScanTask, scanDelay);
    }


    private Runnable startScanTask = new Runnable() {
        @Override
        public void run() {
            if (actionMode != MODE_IDLE) {
                saveLog("start scan run");
                LeScanFilter filter = (LeScanFilter) mParams.get(Parameters.SCAN_FILTERS);
                if (filter == null) {
                    LeBluetooth.getInstance().startScan(null);
                } else {
                    LeBluetooth.getInstance().startScan(filter.uuidInclude);
                }


                long timeout = mParams.getLong(Parameters.COMMON_SCAN_TIMEOUT, DEFAULT_SCAN_TIMEOUT);

                mDelayHandler.removeCallbacks(scanTimeoutTask);
                mDelayHandler.postDelayed(scanTimeoutTask, timeout);
            }
        }
    };

    private Runnable scanTimeoutTask = new Runnable() {
        @Override
        public void run() {
            saveLog("on scan timeout");
            stopScan();
            if (actionMode == MODE_AUTO_CONNECT) {
                if (isRssiCollecting) {
                    mDelayHandler.removeCallbacks(rssiCheckTask);
                    checkRssi();
                } else {
                    long scanRest = mParams.getLong(Parameters.ACTION_AUTO_CONNECT_SCAN_REST, DEFAULT_SCAN_REST);
                    mDelayHandler.removeCallbacks(restartScanTask);
                    mDelayHandler.postDelayed(restartScanTask, scanRest);
                }

            } else if (actionMode == MODE_BIND) {
                onKeyBindFail("scan timeout");
            } else if (actionMode == MODE_FAST_PROVISION) {
                saveLog("fast prov fail : scan timeout");
                onFastProvisionFail();
            } else {
                onScanTimeout();
            }
                /*else if (actionMode == MODE_PROVISION || actionMode == MODE_OTA) {
                onMeshEvent(EVENT_TYPE_SCAN_TIMEOUT, "scan timeout");
            } else if (actionMode == MODE_BIND) {
                onMeshEvent(MeshEvent.EVENT_TYPE_KEY_BIND_FAIL, "scan timeout");
            }*/

        }
    };

    private void onScanFail() {
        this.actionMode = MODE_IDLE;
        Intent intent = new Intent();
        intent.setAction(ACTION_SCAN_EVENT);
        intent.putExtra(EXTRA_EVENT_TYPE, ScanEvent.SCAN_FAIL);
        LocalBroadcastManager.getInstance(mContext)
                .sendBroadcast(intent);
    }

    private void onScanTimeout() {
        this.actionMode = MODE_IDLE;
        Intent intent = new Intent();
        intent.setAction(ACTION_SCAN_EVENT);
        intent.putExtra(EXTRA_EVENT_TYPE, ScanEvent.SCAN_TIMEOUT);
        LocalBroadcastManager.getInstance(mContext)
                .sendBroadcast(intent);
    }

    private Runnable restartScanTask = new Runnable() {
        @Override
        public void run() {
            saveLog("restart scan");
            startScan();
        }
    };

    private Runnable rssiCheckTask = new Runnable() {
        @Override
        public void run() {
            checkRssi();
        }
    };

    private void checkRssi() {
        saveLog("check rssi");
        isRssiCollecting = false;
        BluetoothDevice targetDevice = null;
        int averageRssi = 0;
        for (BluetoothDevice device : mRssiColl.keySet()) {
            List<Integer> rssiList = mRssiColl.get(device);
            int total = 0;
            for (int rssi : rssiList) {
                total += rssi;
            }
            int average = total / rssiList.size();
//            TelinkLog.d("average: " + average + " -- " + device.getAddress() + " current: " + averageRssi);
            if (averageRssi == 0 || averageRssi < average) {
                averageRssi = total / rssiList.size();
                targetDevice = device;
            }
        }

        connectDevice(targetDevice, null, averageRssi);
    }

    private Runnable connectTimeoutTask = new Runnable() {
        @Override
        public void run() {
            if (mDevice != null) {
//                saveLog("connect timeout:" + connectRetry);
                if (actionMode == MODE_AUTO_CONNECT) {
                    mDevice.disconnect();
                    startScan();
                } else if (actionMode == MODE_PROVISION) {
//                    int retry = getAndIncrementRetryCount(mDevice.getMacAddress());
                    mDevice.disconnect();
                    connectRetry++;
                    if (connectRetry >= MAX_CONNECT_RETRY) {
                        onProvisionFail("connect retry max");
                        /*if (pvIgnoreList.add(mDevice.getMacAddress())) {
                            saveLog("provision fail: connect retry max");
                            onProvisionFail("connect retry max");
                        }*/
                    } else {
                        connectProvisioningDevice();
                    }
//                    startScan();
                } else if (actionMode == MODE_OTA) {
//                    int retry = getAndIncrementRetryCount(mDevice.getMacAddress());

                    connectRetry++;
                    mDevice.disconnect();
                    if (connectRetry >= MAX_CONNECT_RETRY) {
                        saveLog("ota fail: connect retry max");
                        onOtaEvent(OtaEvent.EVENT_TYPE_OTA_FAIL);
                    } else {
                        startScan();
                    }
                } else if (actionMode == MODE_BIND) {
//                    int retry = getAndIncrementRetryCount(mDevice.getMacAddress());
                    connectRetry++;
                    mDevice.disconnect();
                    if (connectRetry >= MAX_CONNECT_RETRY) {
                        saveLog("bind fail: connect retry max");
                        onMeshEvent(MeshEvent.EVENT_TYPE_KEY_BIND_FAIL, "bind fail: connect retry max");
                    } else {
                        startScan();
                    }
                }
            }
        }
    };

    private Runnable provisionTask = new Runnable() {
        @Override
        public void run() {
            byte[] pvData = mParams.getByteArray(Parameters.ACTION_PROVISION_DATA);

            if (pvData != null) {
                int newAdr = mProvisioningDevice.unicastAddress;
                mDevice.setMeshAddress(newAdr);
                meshLib.provisionRandomDataInit();
                meshLib.setProvisionData(pvData, pvData.length);
                meshLib.startProvisionInvite();
                isPvProcessing = true;
                saveLog("provision: set provision data");
//                mDelayHandler.postDelayed(provisionStateCheckTask, 3000);
            } else {
                onProvisionFail("provision data invalid");
            }
        }
    };


    private void connectDevice(BluetoothDevice device, byte[] scanRecord, int rssi) {
        saveLog("connect device:" + device.getAddress() + " -- " + rssi);
        mDelayHandler.removeCallbacks(scanTimeoutTask);
        stopScan();

        mDevice.connect(device, scanRecord, rssi);
        mDelayHandler.removeCallbacks(connectTimeoutTask);
        mDelayHandler.postDelayed(connectTimeoutTask, CONNECTION_TIMEOUT);
    }

    private void onProvisionSuccess() {
        if (mDevice != null) {
            saveLog("onProvisionSuccess: " + mDevice.getMeshAddress() + " -- " + mDevice.getMacAddress());
            actionMode = MODE_IDLE;
            onMeshEvent(MeshEvent.EVENT_TYPE_PROVISION_SUCCESS, "provision success");
        }
    }

    private void onProvisionFail(String desc) {
        if (mDevice != null) {
            saveLog("onProvisionFail: " + mDevice.getMeshAddress() + " -- " + mDevice.getMacAddress() + " -- " + desc);
            actionMode = MODE_IDLE;
            onMeshEvent(MeshEvent.EVENT_TYPE_PROVISION_FAIL, desc);
        }
    }


    /**
     * start key bind action
     */
    private void bindAppKey() {
        if (mDevice != null && mDevice.isConnected() && meshLib != null) {
            isKeyBindProcessing = true;
            final byte[] appKey = mParams.getByteArray(Parameters.ACTION_BIND_APP_KEY);
            final int appKeyIndex = mParams.getInt(Parameters.ACTION_BIND_APP_KEY_INDEX, 0);
            final int netKeyIndex = mParams.getInt(Parameters.ACTION_BIND_NET_KEY_INDEX, 0);
            DeviceInfo deviceInfo = (DeviceInfo) mParams.get(Parameters.ACTION_BIND_DEVICE_INFO);
            mDevice.setMeshAddress(deviceInfo.meshAddress);
            saveLog("bind app key: " + deviceInfo.meshAddress + " -- " + deviceInfo.macAddress +
                    " -- " + Arrays.bytesToHexString(appKey, ":") + " netKeyIndex: " + netKeyIndex
                    + " appKeyIndex: " + appKeyIndex);
            int fastMode = validPrivate(deviceInfo) ? 1 : 0;
            meshLib.meshConfigModeParaSet(appKeyIndex, appKey, deviceInfo.meshAddress, netKeyIndex, fastMode);
        }
    }

    /**
     * bind key success
     */
    private void onKeyBindSuccess() {
        if (mDevice != null) {
            saveLog("onKeyBindSuccess: " + mDevice.getMeshAddress() + " -- " + mDevice.getMacAddress());
            this.actionMode = MODE_IDLE;
            onMeshEvent(MeshEvent.EVENT_TYPE_KEY_BIND_SUCCESS, "key bind success");
        }
    }

    /**
     * bind app key fail
     *
     * @param desc fail reason
     */
    private void onKeyBindFail(String desc) {
        if (mDevice != null) {
            saveLog("onKeyBindFail: " + mDevice.getMeshAddress() + " -- " + mDevice.getMacAddress() + " -- " + desc);
            this.actionMode = MODE_IDLE;
            onMeshEvent(MeshEvent.EVENT_TYPE_KEY_BIND_FAIL, desc);
        }
    }


    private void keyBindPrepare() {
        mDelayHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                mDevice.writeCCCForPx();
                mDelayHandler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        filterInit(localAddress);
                    }
                }, 500);
            }
        }, 500);

    }

    private class DeviceCallbackImpl implements Device.DeviceCallback {

        public void clear() {
        }

        public DeviceCallbackImpl() {
        }


        @Override
        public void onConnected() {
            saveLog("onConnected:" + mDevice.getMacAddress());
        }

        @Override
        public void onDisconnected() {
            saveLog("onDisconnected:" + mDevice.getMacAddress() + " -- mode: " + actionMode);
            mDelayHandler.removeCallbacksAndMessages(null);
            isLogin = false;
            onMeshEvent(MeshEvent.EVENT_TYPE_DISCONNECTED, "device disconnected");
            if (disconnectWaiting) {
                disconnectWaiting = false;
                connectRetry = -1;
            }
            if (meshLib != null) {
                meshLib.onDeviceDisconnect();
            }
            //  && autoConnectTargets.size() != 0
            if (actionMode != MODE_IDLE) {

                mDelayHandler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        saveLog("disconnect delayed task run");
                        if (actionMode == MODE_AUTO_CONNECT) {
                            startScan();
                            /*if (checkConnectTargets()) {
                                startScan();
                            }*/
                        } else if (actionMode == MODE_PROVISION) {
                            if (isPvProcessing) {
                                onProvisionFail("provision fail: disconnect during provision processing");
                            } else {
                                if (isPvWaiting) {
                                    isPvWaiting = false;
                                    connectProvisioningDevice();
                                } else {
                                    connectRetry++;
                                    if (connectRetry >= MAX_CONNECT_RETRY) {
                                        onProvisionFail("provision fail: onDisconnect max time");
                                    } else {
                                        connectProvisioningDevice();
                                    }
                                }
                            }

                        } else if (actionMode == MODE_OTA || actionMode == MODE_FAST_PROVISION) {

                            connectRetry++;
                            if (connectRetry >= MAX_CONNECT_RETRY) {
                                if (actionMode == MODE_OTA) {
                                    saveLog("ota fail: onDisconnect max time");
                                    onOtaEvent(OtaEvent.EVENT_TYPE_OTA_FAIL);
                                    actionMode = MODE_IDLE;
                                } else {
                                    saveLog("fast provision fail: onDisconnect max time");
                                    onFastProvisionFail();
                                }

                            } else {
                                startScan();
                            }
                        } else if (actionMode == MODE_BIND) {
                            if (isKeyBindProcessing) {
                                isKeyBindProcessing = false;
                                onKeyBindFail("key bind fail: disconnected during bind processing");
                            } else {
                                connectRetry++;
                                if (connectRetry >= MAX_CONNECT_RETRY) {
                                    onKeyBindFail("bind fail: onDisconnect max time");
                                } else {
                                    startScan();
                                }
                            }
                        } else if (actionMode == MODE_SCAN) {
                            startScan();
                        } else if (actionMode == MODE_REMOTE_PROVISION) {
                            onRemoteProvisionFail("disconnected during remote provision processing");
                        } else if (actionMode == MODE_REMOTE_BIND) {
                            onRemoteBindFail("disconnected during remote provision processing");
                        }

                    }
                }, 1000);
            }
        }

        @Override
        public void onServicesDiscovered(List<BluetoothGattService> services) {
            saveLog("onServicesDiscovered:" + mDevice.getMacAddress());
            // 有可能回调多次
            mDelayHandler.removeCallbacksAndMessages(null);
            if (actionMode == MODE_AUTO_CONNECT) {
                mDelayHandler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        if (mDevice != null) {
                            mDevice.writeCCCForPx();
                        }
                        mDelayHandler.postDelayed(new Runnable() {
                            @Override
                            public void run() {
                                filterInit(localAddress);
                            }
                        }, 100);
                    }
                }, 500);
            } else if (actionMode == MODE_PROVISION) {
                mDelayHandler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        mDevice.writeCCCForPv();
                        mDelayHandler.postDelayed(provisionTask, 100);
                    }
                }, 500);
            } else if (actionMode == MODE_OTA) {
                mDelayHandler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        // todo read firmware version
                        filterInit(localAddress);
                    }
                }, 500);
            } else if (actionMode == MODE_BIND) {
                keyBindPrepare();
            } else if (actionMode == MODE_FAST_PROVISION) {
                mDelayHandler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        mDevice.writeCCCForPv();
                        mDelayHandler.postDelayed(new Runnable() {
                            @Override
                            public void run() {
                                int pid = mParams.getInt(Parameters.ACTION_FAST_PROVISION_PID, 0);
                                if (pid == 0) {
                                    saveLog("invalid pid");
                                    onFastProvisionFail();
                                } else {
                                    meshLib.startFastProv(pid);
                                }
                            }
                        }, 100);
                    }
                }, 500);
            }
        }

        @Override
        public void onNotifyEnable() {

        }

        @Override
        public void onNotify(UUID charUUID, byte[] data) {
            if (charUUID.equals(UuidInfo.CHARACTERISTIC_UUID_ONLINE_STATUS)) {
                // 62:06:6E:24:F6:E1:22:87:FA:D7:F5:8A:38:51:9D:54:3D:92
                if (data != null) {
                    byte[] decrypted = meshLib.decryptOnlineStatusData(data);
                    if (decrypted != null) {
                        saveLog("online status decrypted data: " + Arrays.bytesToHexString(decrypted, ":"));
                        onOnlineStatusNotify(decrypted);
//                        mQueue.add(decrypted);
//                        threadHandler.post(notifyTask);
                    } else {
                        saveLog("online status decrypt err");
                    }
                }

            } else {
                meshLib.pushNotifyIntoFifo(data, data.length);
            }

        }

        @Override
        public void onCommandError(String errorMsg) {

        }

        @Override
        public void onOtaStateChanged(int state) {

            String type = null;
            switch (state) {
                case Device.STATE_SUCCESS:
                    type = OtaEvent.EVENT_TYPE_OTA_SUCCESS;
                    actionMode = MODE_IDLE;
                    break;

                case Device.STATE_FAILURE:
                    type = OtaEvent.EVENT_TYPE_OTA_FAIL;
                    actionMode = MODE_IDLE;
                    break;

                case Device.STATE_PROGRESS:
                    type = OtaEvent.EVENT_TYPE_OTA_PROGRESS;
                    break;
            }

            onOtaEvent(type);
        }
    }


    private void onOnlineStatusNotify(byte[] data) {

        Intent intent = new Intent();
        intent.setAction(ACTION_ONLINE_STATUS);
        intent.putExtra(EXTRA_NOTIFY_RAW, data);
        LocalBroadcastManager.getInstance(mContext)
                .sendBroadcast(intent);

    }

    private void onOtaEvent(String type) {

        Intent intent = new Intent();
        OtaDeviceInfo deviceInfo = null;
        if (mDevice != null) {
            deviceInfo = new OtaDeviceInfo();
            deviceInfo.macAddress = mDevice.getMacAddress().toUpperCase();
            deviceInfo.meshAddress = mDevice.getMeshAddress();
            if (type.equals(OtaEvent.EVENT_TYPE_OTA_PROGRESS)) {
                deviceInfo.progress = mDevice.getOtaProgress();
            }
        }
        intent.setAction(ACTION_OTA);
        intent.putExtra(EXTRA_DEVICE_INFO, deviceInfo);
        intent.putExtra(EXTRA_EVENT_TYPE, type);

        LocalBroadcastManager.getInstance(mContext)
                .sendBroadcast(intent);
    }


    private void onFastProvisionComplete() {
        this.actionMode = MODE_IDLE;
        Intent intent = new Intent();
        intent.setAction(ACTION_MESH_EVENT);
        intent.putExtra(EXTRA_EVENT_TYPE, MeshEvent.EVENT_TYPE_FAST_PROVISION_COMPLETE);
        LocalBroadcastManager.getInstance(mContext).sendBroadcast(intent);
    }

    private void onFastProvisionFail() {
        this.actionMode = MODE_IDLE;

        Intent intent = new Intent();
        intent.setAction(ACTION_MESH_EVENT);
        intent.putExtra(EXTRA_EVENT_TYPE, MeshEvent.EVENT_TYPE_FAST_PROVISION_FAIL);
        LocalBroadcastManager.getInstance(mContext).sendBroadcast(intent);
    }

    private void onFastProvisionNodeInfoUpdate(byte[] deviceKey, int address, int eleCnt) {
        DeviceInfo deviceInfo = new DeviceInfo();

        byte[] mac = new byte[6];
        System.arraycopy(deviceKey, 0, mac, 0, 6);
        deviceInfo.macAddress = Arrays.bytesToHexString(Arrays.reverse(mac), ":").toUpperCase();

        TelinkLog.d("onFastProvisionNodeInfoUpdate: deviceKey " + Arrays.bytesToHexString(deviceKey, ":"));
        TelinkLog.d("onFastProvisionNodeInfoUpdate: mac " + deviceInfo.macAddress);
        deviceInfo.meshAddress = address;
        deviceInfo.deviceKey = deviceKey;
        deviceInfo.elementCnt = eleCnt;

        Intent intent = new Intent();
        intent.setAction(ACTION_MESH_EVENT);
        intent.putExtra(EXTRA_DEVICE_INFO, deviceInfo);
        intent.putExtra(EXTRA_EVENT_TYPE, MeshEvent.EVENT_TYPE_FAST_PROVISION_DEVICE_ADDRESS_SET);
        LocalBroadcastManager.getInstance(mContext)
                .sendBroadcast(intent);
    }

    private void onDeviceFound(AdvertisingDevice device) {
        saveLog("onDeviceFound: " + device.device.getAddress());
        if (advDevices.add(device)) {
            Intent intent = new Intent();
            intent.setAction(ACTION_SCAN_EVENT);
            intent.putExtra(EXTRA_ADV_DEVICE, device);
            intent.putExtra(EXTRA_EVENT_TYPE, ScanEvent.DEVICE_FOUND);
            LocalBroadcastManager.getInstance(mContext)
                    .sendBroadcast(intent);
        } else {
            saveLog("Device already exist");
        }
    }


    private final Object SCAN_LOCK = new Object();

    private void onScanFilter(final BluetoothDevice device, final int rssi, final byte[] scanRecord) {

//        if (!device.getAddress().toUpperCase().contains("A6:A5:A4:A3")) return;
//        if (!device.getAddress().toUpperCase().contains("FF:FF:BB:CC:DD")) return;

        synchronized (SCAN_LOCK) {
            if (!isScanning.get()) return;
            boolean connectIntent = false;
            if (actionMode == MODE_AUTO_CONNECT) {
                if (validateProxyAdv(device.getAddress(), scanRecord)) {

                    long minPeriod = mParams.getLong(Parameters.ACTION_AUTO_CONNECT_SCAN_MIN_PERIOD, DEFAULT_MIN_SCAN_PERIOD);
                    if (minPeriod > 0) {
                        if (!isRssiCollecting) {
                            saveLog("rssi collect start");
                            mDelayHandler.removeCallbacks(rssiCheckTask);
                            mDelayHandler.postDelayed(rssiCheckTask, minPeriod);
                            isRssiCollecting = true;
                            mRssiColl.clear();
                        }

                        List<Integer> rssiList = mRssiColl.get(device);
                        if (rssiList == null) {
                            rssiList = new ArrayList<>();
                            rssiList.add(rssi);
                            mRssiColl.put(device, rssiList);
                        } else {
                            rssiList.add(rssi);
                        }
                        saveLog("rssi collect:" + device.getAddress() + " -- " + rssi);
                    } else {
                        connectIntent = true;
                    }
                }

            } else if (actionMode == MODE_OTA) {
                String otaMac = mParams.getString(Parameters.ACTION_OTA_MAC);
                if (!TextUtils.isEmpty(otaMac) && device.getAddress().equals(otaMac)) {
                    connectIntent = true;
                }
            } else if (actionMode == MODE_BIND) {
                DeviceInfo deviceInfo = (DeviceInfo) mParams.get(Parameters.ACTION_BIND_DEVICE_INFO);
                if (device.getAddress().equals(deviceInfo.macAddress)) {
                    connectIntent = true;
                }
            } else if (actionMode == MODE_SCAN) {
                connectIntent = false;
                LeScanFilter filter = (LeScanFilter) mParams.get(Parameters.SCAN_FILTERS);
                if (filter.macExclude != null && filter.macExclude.length != 0) {
                    for (String mac : filter.macExclude) {
                        if (mac.toUpperCase().equals(device.getAddress().toUpperCase())) {
                            return;
                        }
                    }
                }

                boolean isTarget = true;
                if (filter.macInclude != null && filter.macInclude.length != 0) {
                    isTarget = false;
                    for (String mac : filter.macInclude) {
                        if (mac.toUpperCase().equals(device.getAddress().toUpperCase())) {
                            isTarget = true;
                            break;
                        }
                    }
                }

                if (isTarget) {
                    boolean single = mParams.getBool(Parameters.SCAN_SINGLE_MODE, false);
                    if (single) {
                        stopScan();
                    }
                    onDeviceFound(new AdvertisingDevice(device, rssi, scanRecord));
                }
            } else if (actionMode == MODE_FAST_PROVISION) {
                connectIntent = true;
            }

            if (connectIntent) {
                isScanning.set(false);
                mDelayHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        connectDevice(device, scanRecord, rssi);
                    }
                });
            }
        }
    }


    /**
     * validate proxy service advertising data when [auto connect]
     * <p>
     * {@link #MODE_AUTO_CONNECT}
     *
     * @return matches networkId id or node identity
     */
    private boolean validateProxyAdv(String mac, byte[] scanRecord) {
        Object targets = mParams.get(Parameters.ACTION_AUTO_CONNECT_TARGET_LIST);
        if (targets != null) {
            Set<String> macs = (Set<String>) targets;
            return macs.contains(mac);
        }

        MeshScanRecord sr = MeshScanRecord.parseFromBytes(scanRecord);
        byte[] serviceData = sr.getServiceData(ParcelUuid.fromString(UuidInfo.PROXY_SERVICE_UUID.toString()));
        if (serviceData != null && serviceData.length >= 9) {
            int type = serviceData[0];
            if (type == PROXY_ADV_TYPE_NETWORK_ID) {
                // validate network id matches
                final int networkIdLen = 8;
                byte[] advertisingNetworkId = new byte[networkIdLen];
                System.arraycopy(serviceData, 1, advertisingNetworkId, 0, networkIdLen);
                boolean networkIdCheck = Arrays.equals(networkId, advertisingNetworkId);
                TelinkLog.d("check network id: " + networkIdCheck);
                return networkIdCheck;
            } else if (type == PROXY_ADV_TYPE_NODE_IDENTITY) {
                Object nodeList = mParams.get(Parameters.ACTION_AUTO_CONNECT_NODE_ADDRESS_LIST);
                if (nodeList == null) return false;
                boolean nodeIdentityCheck = validateNodeIdentity((int[]) nodeList, serviceData);
                TelinkLog.d("check node identity: " + nodeIdentityCheck);
                return nodeIdentityCheck;
            }
        }
        return false;
    }

    private boolean validateNodeIdentity(int[] nodeList, byte[] serviceData) {
        if (this.identityKey == null || nodeList.length == 0 || serviceData.length < 17)
            return false;

        // hash and random data length
        final int dataLen = 8;
        byte[] advHash;
        byte[] random;
        for (int nodeAddress : nodeList) {
            int index = 1;
            advHash = new byte[dataLen];
            System.arraycopy(serviceData, index, advHash, 0, dataLen);
            index += dataLen;

            random = new byte[dataLen];
            System.arraycopy(serviceData, index, random, 0, dataLen);
            byte[] hash = Encipher.generateNodeIdentityHash(identityKey, random, nodeAddress);
            if (Arrays.equals(advHash, hash)) {
                return true;
            }
        }

        return false;
    }


    private class DeviceScanCallback implements LeBluetooth.LeScanCallback {

        @Override
        public void onLeScan(BluetoothDevice device, int rssi, byte[] scanRecord) {
            TelinkLog.w("scan:" + device.getName() + " --mac: " + device.getAddress() + " --record: " + Arrays.bytesToHexString(scanRecord, ":"));
            onScanFilter(device, rssi, scanRecord);
        }

        @Override
        public void onScanFail(int errorCode) {
            saveLog("scan fail:" + errorCode);
            MeshController.this.onScanFail();
        }

        @Override
        public void onStartedScan() {
            isScanning.set(true);
            isRssiCollecting = false;
            lastScanStartTime = System.currentTimeMillis();
        }

        @Override
        public void onStoppedScan() {
//            isScanning = false;
        }
    }


    private MeshLib.FastProvisionHelper fastProvisionHelper = new MeshLib.FastProvisionHelper() {
        @Override
        public int onFastProvAdrRequest() {
            if (actionMode == MODE_FAST_PROVISION) {
                return fastProvisionIndex;
            }
            return -1;
        }

        @Override
        public void onFastProvNodeInfoCallback(byte[] devKey, int nodeAdr, int pid) {
            fastProvisionIndex += fastPvElementCount;
            MeshController.this.onFastProvisionNodeInfoUpdate(devKey, nodeAdr, fastPvElementCount);
        }

        @Override
        public int onFastProvEleCntRequire(int pid) {
            return fastPvElementCount;
        }
    };


    private MeshLib.Helper meshLibHelper = new MeshLib.Helper() {
        @Override
        public void sendPkt(int type, byte[] data) {

            if (mDevice != null && mDevice.isConnected()) {
                if (type == TYPE_PROVISION) {
                    mDevice.sendProvisionCmd(data);
                } else if (type == TYPE_CONTROL) {
                    mDevice.sendControlCmd(data);
                }
            }
        }

        @Override
        public void onCommandRsp(byte[] data) {
            if (mDevice == null || !mDevice.isConnected()) return;
            saveLog("command rsp " + Arrays.bytesToHexString(data, ":"));
            postNotificationEvent(data);
        }

        @Override
        public void onCommandErr(byte[] iniData) {
            MeshCommand meshCommand = MeshCommand.from(iniData);
            saveLog("on command err: " + (meshCommand == null ? "null" : meshCommand.toString()));
            if (meshCommand != null) {
                onCmdComplete(meshCommand.opcode, meshCommand.rspMax, 0);
            }

        }

        @Override
        public void onProvisionStateChange(byte state) {
            if (mDevice == null)
                return;

            if (actionMode == MODE_REMOTE_PROVISION) {
                if (state == PROV_NORMAL_RET) {
                    onRemoteProvisionSuccess();
                } else {
                    onRemoteProvisionFail("provision callback fail");
                }
            } else if (actionMode == MODE_PROVISION) {

                if (isPvProcessing)
                    isPvProcessing = false;

                if (state == PROV_NORMAL_RET) {
                    onProvisionSuccess();
                } else if (state == PROV_TIMEOUT_RET) {
                    onProvisionFail("provision timeout");
                } else {
                    onProvisionFail("callback state unknown");
                }
            }
        }

        @Override
        public void onReliableCmdStop(int op, int rspMax, int rspCnt) {
            saveLog("onReliableCmdStop: " + ", op=" + String.format("%04X", op) + " rspMax=" + rspMax + " rspCnt=" + rspCnt);
            onCmdComplete(op, rspMax, rspCnt);
        }

        @Override
        public void terminate() {
            if (mDevice != null) {
                saveLog("terminate from lib");
//                mDevice.disconnect();
            }
        }

        @Override
        public void onAppKeyBindEnd(byte event) {
            if (actionMode == MODE_BIND) {
                if (isKeyBindProcessing) {
                    isKeyBindProcessing = false;

                    if (event == MESH_APP_KEY_BIND_EVENT_SUC) {
                        onKeyBindSuccess();
                    } else if (event == MESH_APP_KEY_BIND_EVENT_TIMEOUT) {
                        onKeyBindFail("app key bind timeout");
                    } else {
                        onKeyBindFail("app key bind err");
                    }

                }
            } else if (actionMode == MODE_REMOTE_BIND) {
                if (event == MESH_APP_KEY_BIND_EVENT_SUC) {
                    onRemoteBindSuccess();
                } else {
                    onRemoteBindFail("bind callback fail");
                }
            } else if (actionMode == MODE_FAST_PROVISION) {
                if (event == MESH_APP_KEY_BIND_EVENT_SUC) {
                    saveLog("fast provision complete");
                    onFastProvisionComplete();
                } else {
                    saveLog("fast provision end err");
                    onFastProvisionFail();
                }
            }


        }

        @Override
        public void onIvUpdated(int ivIndex) {
            onIvUpdate(ivIndex);
        }

        @Override
        public void onFilterInitComplete() {
            mDelayHandler.removeCallbacks(filterTimeoutTask);
            isLogin = true;
            if (mDevice != null && mDevice.isConnected()) {
                if (actionMode == MODE_AUTO_CONNECT) {
                    onAutoConnectSuccess();
                } else if (actionMode == MODE_BIND) {
                    bindAppKey();
//                    mDelayHandler.removeCallbacks(bindTask);
//                    mDelayHandler.postDelayed(bindTask, 1500);
                } else if (actionMode == MODE_OTA) {
                    byte[] otaFirm = (byte[]) mParams.get(Parameters.ACTION_OTA_FIRMWARE);
                    if (otaFirm != null) {
                        mDevice.startOta(otaFirm);
                    }
                }
            }
        }

        @Override
        public void onMeshOTAProgressUpdate(int currentBlock, int totalBlock, int progress) {
            onMeshOTAProgressChange(progress);
        }

        @Override
        public byte[] onMeshOTAFirmwareRequest(int maxLen) {
            return meshOtaFirmware;
        }

        @Override
        public void onMeshOtaApplyStatusNotify(int srcAdr, byte[] data) {
            MeshController.this.onMeshOtaApplyStatusNotify(srcAdr, data);
        }

        @Override
        public int onConnectedAdrRequest() {
            return connectedMeshAdr;
        }

    };

    private Runnable meshMainProcess = new Runnable() {
        @Override
        public void run() {
            if (meshLib != null) {

//                onOnlineStatusNotify(new byte[]{0x62,0x06, (byte) 0xE6,0x2B,0x5A,0x00,0x00
//                ,0x00,0x64, (byte) 0xFF, 0x00, 0x00,0x00, 0x00, 0x00, 0x00});
                meshLib.threadMainProcess();
                if (mLoopHandler != null) {
                    mLoopHandler.postDelayed(this, MESH_MAIN_PROCESS_CYCLE);
                }
            }
        }
    };

    /**
     * device connect success at auto connect mode
     */
    private void onAutoConnectSuccess() {
        onMeshEvent(MeshEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN, "service discover complete");
    }

    private void onMeshOtaApplyStatusNotify(int src, byte[] data) {
        Intent intent = new Intent();
        intent.setAction(ACTION_MESH_OTA);
        intent.putExtra(EXTRA_MESH_OTA_APPLY_SRC, src);
        intent.putExtra(EXTRA_NOTIFY_RAW, data);
        intent.putExtra(EXTRA_EVENT_TYPE, MeshOtaEvent.EVENT_TYPE_APPLY_STATUS);
        LocalBroadcastManager.getInstance(mContext)
                .sendBroadcast(intent);
    }

    private void onMeshOTAProgressChange(int progress) {
        Intent intent = new Intent();
        intent.setAction(ACTION_MESH_OTA);
        intent.putExtra(EXTRA_MESH_OTA_PROGRESS, progress);
        intent.putExtra(EXTRA_EVENT_TYPE, MeshOtaEvent.EVENT_TYPE_PROGRESS_UPDATE);
        LocalBroadcastManager.getInstance(mContext)
                .sendBroadcast(intent);
    }

    private void onMeshEvent(String type, String desc) {

        saveLog("mesh event: type -- " + type + " desc -- " + desc);
        Intent intent = new Intent();
        DeviceInfo deviceInfo = null;
        if (mDevice != null) {
            deviceInfo = new DeviceInfo();
            deviceInfo.macAddress = mDevice.getMacAddress().toUpperCase();
            deviceInfo.meshAddress = mDevice.getMeshAddress();

            if (type.equals(MeshEvent.EVENT_TYPE_KEY_BIND_SUCCESS)) {
//                if (validPrivate())
                deviceInfo.boundModels = (short[]) mParams.get(Parameters.ACTION_BIND_MODEL_LIST);
                deviceInfo = (DeviceInfo) mParams.get(Parameters.ACTION_BIND_DEVICE_INFO);
                if (!validPrivate(deviceInfo)) {
                    byte[] nodeInfoData = meshLib.getNodeInfo(mDevice.getMeshAddress());
                    deviceInfo.nodeInfo = NodeInfo.from(nodeInfoData);
                }
            } else if (type.equals(MeshEvent.EVENT_TYPE_PROVISION_SUCCESS)) {
                deviceInfo.elementCnt = meshLib.getElementCnt(mDevice.getMeshAddress());
                deviceInfo.deviceKey = meshLib.getCurrentDeviceKey();
                /*if (privateMode) {
                    PrivateDevice device = getPrivateDevice(mDevice.getScanRecord());
                    if (device != null) {
                        deviceInfo.nodeInfo = NodeInfo.getByPrivateDevice(device, deviceInfo.meshAddress,
                                deviceInfo.elementCnt, deviceInfo.deviceKey);
                        TelinkLog.d("device nodeInfo: " + deviceInfo.nodeInfo);
                    }
                }*/

            }
        }
        intent.setAction(ACTION_MESH_EVENT);
        intent.putExtra(EXTRA_DEVICE_INFO, deviceInfo);
        intent.putExtra(EXTRA_EVENT_TYPE, type);

        LocalBroadcastManager.getInstance(mContext)
                .sendBroadcast(intent);
    }


    private void onIvUpdate(int ivIndex) {
        Intent intent = new Intent();
        intent.setAction(ACTION_SETTING_EVENT);
        intent.putExtra(EXTRA_SETTING_INFO, ivIndex);
        intent.putExtra(EXTRA_EVENT_TYPE, SettingEvent.EVENT_TYPE_IV_UPDATE);
        LocalBroadcastManager.getInstance(mContext)
                .sendBroadcast(intent);
    }

    private void postNotificationEvent(byte[] data) {
//        saveLog("onNotify:" + type);

        NotificationInfo notifyInfo = NotificationInfoParser.create().parseInfo(data);

        if (checkLinkStatus(notifyInfo)) return;

        Intent intent = new Intent();
        intent.setAction(ACTION_STATUS_RESPONSE);
        intent.putExtra(EXTRA_NOTIFY_RAW, data);
        intent.putExtra(EXTRA_NOTIFY_INFO, notifyInfo);

        LocalBroadcastManager.getInstance(mContext)
                .sendBroadcast(intent);
    }

    private Runnable linkOpenTimeoutTask = new Runnable() {
        @Override
        public void run() {
            onProvisionFail("device link open timeout");
        }
    };

    private boolean checkLinkStatus(NotificationInfo notificationInfo) {
        if (notificationInfo == null) return false;
        if (actionMode == MODE_REMOTE_PROVISION) {
            if (notificationInfo.opcode == Opcode.REMOTE_PROV_LINK_STS.getValue()) {
                //                mDelayHandler.removeCallbacks(linkOpenTimeoutTask);

                final byte[] pvData = mParams.getByteArray(Parameters.ACTION_PROVISION_DATA);
                if (pvData == null) {
                    onRemoteProvisionFail("provision data null !!");
                    return false;
                }

                mDelayHandler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        meshLib.provisionRandomDataInit();
                        meshLib.setProvisionData(pvData, pvData.length);
                        meshLib.startProvisionInvite();
                    }
                }, 100);

                return true;
            }
        }
        return false;
    }


    /*******************************
     cmd  start */


    /**
     * @param addr   目标地址
     * @param rspMax means after sending the parameter,it will how many different packets back
     *               示例为2：列表中有2个设备
     * @param onOff  1 means on ,and 0 means off
     * @param ack    1 means send reliable cmd ,and the node will send rsp ,0 means unreliable ,will not send
     *               1 有回复 0 无回复
     */
    void cmdOnOff(int addr, byte rspMax, byte onOff, int ack) {
        if (meshLib == null) return;
        if (mDevice != null && mDevice.isConnected()) {
            saveLog("CMD_ON_OFF: addr-" + Integer.toHexString(addr)
                    + " rspMax-" + Integer.toHexString(rspMax)
                    + " onOff-" + Integer.toHexString(onOff)
                    + " ack-" + Integer.toHexString(ack));
            int result = meshLib.cmdOnOff(addr, rspMax, onOff, ack);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
            } else {

                onCmdProcessing();
            }

        }
    }

    /*void cmdOnOff(int adr, byte rspMax, byte onOff, int ack) {
        MeshCommand command = new MeshCommand();
        command.netKeyIndex = 0;
        command.applicationKeyIndex = 0;
        command.retryCnt = 2;
        command.rspMax = rspMax;
        command.destAddress = adr;

        command.opcode = ack == 0 ? Opcode.G_ON_OFF_SET_NO_ACK.getValue() : Opcode.G_ON_OFF_SET.getValue(); // G_ONOFF_SET_NOACK
        OnOffMessage onOffMessage = OnOffMessage.createInstance(onOff);
//        onOffMessage.transTime = TransitionTime.fromTime(3 * 1000).getValue();
        onOffMessage.delay = (byte) 200;
        command.params = onOffMessage.toBytes();
        byte[] re = command.toBytes();
        meshLib.sendOpByINI(re, re.length);
//        meshLib.sendOpParaDebug(adr, rspMax, 1, 1);
    }*/


    /**
     * 获取订阅/分组信息
     *
     * @param nodeAdr 节点地址
     * @param eleAdr  暂同节点地址
     * @param modelId SIG_MD_G_ONOFF_S || SIG_MD_G_ONOFF_C
     */
    void cfgCmdSubGet(int nodeAdr, int eleAdr, int modelId) {
        if (meshLib == null) return;
        if (mDevice != null && mDevice.isConnected()) {
            saveLog("CMD_SUB_GET: nodeAdr-" + Integer.toHexString(nodeAdr)
                    + " eleAdr-" + Integer.toHexString(eleAdr)
                    + " modelId-" + Integer.toHexString(modelId));
            int result = meshLib.cfgCmdSubGet(nodeAdr, eleAdr, modelId);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
            } else {
                onCmdProcessing();
            }

        }
    }

    /**
     * 设置订阅/分组信息
     *
     * @param opCode  添加／删除 CFG_MODEL_SUB_ADD || CFG_MODEL_SUB_DEL
     * @param nodeAdr 节点地址
     * @param eleAdr  元素地址
     * @param subAdr  订阅地址
     * @param modelId SIG_MD_G_ONOFF_S || SIG_MD_G_ONOFF_C
     */
    boolean cfgCmdSubSet(int opCode, int nodeAdr, int eleAdr, int subAdr, int modelId, boolean sig) {
        if (meshLib == null) return false;
        if (mDevice != null && mDevice.isConnected()) {
            saveLog("CMD_SUB_SET: opCode-" + Integer.toHexString(opCode)
                    + " nodeAdr-" + Integer.toHexString(nodeAdr)
                    + " eleAdr-" + Integer.toHexString(eleAdr)
                    + " subAdr-" + Integer.toHexString(subAdr)
                    + " modelId-" + Integer.toHexString(modelId));
            int result = meshLib.cfgCmdSubSet(opCode, nodeAdr, eleAdr, subAdr, modelId, (byte) (sig ? 1 : 0));
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
            } else {
                onCmdProcessing();
                return true;
            }

        }
        return false;
    }

    /**
     * 获取public address
     *
     * @param nodeAdr
     * @param eleAdr
     * @param modelId
     */
    void cfgCmdPubGet(int nodeAdr, int eleAdr, int modelId, boolean sig) {
        if (meshLib == null) return;
        if (mDevice != null && mDevice.isConnected()) {
            saveLog("CMD_Pub_Get: nodeAdr-" + Integer.toHexString(nodeAdr)
                    + " eleAdr-" + Integer.toHexString(eleAdr)
                    + " modelId-" + Integer.toHexString(modelId));
            int result = meshLib.cfgCmdPubGet(nodeAdr, eleAdr, modelId, (byte) (sig ? 1 : 0));
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
            } else {
                onCmdProcessing();
            }

        }
    }

    /**
     * 设置public address
     *
     * @param nodeAdr
     * @param eleAdr
     * @param pubAdr
     * @param modelId
     */
    boolean cfgCmdPubSet(int nodeAdr, int eleAdr, int pubAdr, int modelId, byte[] params, boolean sig) {
        if (meshLib == null) return false;
        if (mDevice != null && mDevice.isConnected()) {
            saveLog("CMD_PUB_SET: nodeAdr-" + Integer.toHexString(nodeAdr)
                    + " eleAdr-" + Integer.toHexString(eleAdr)
                    + " pubAdr-" + Integer.toHexString(pubAdr)
                    + " modelId-" + Integer.toHexString(modelId));
            int result = meshLib.cfgCmdPubSet(nodeAdr, eleAdr, pubAdr, modelId, params, (byte) (sig ? 1 : 0));
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
            } else {
                onCmdProcessing();
                return true;
            }

        }
        return false;
    }

    boolean cfgCmdRelaySet(int nodeAdr, int value) {
        if (meshLib == null) return false;
        if (mDevice != null && mDevice.isConnected()) {
            int result = meshLib.cfgCmdRelaySet(nodeAdr, value);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
            } else {
                onCmdProcessing();
                return true;
            }

        }
        return false;
    }

    /**
     * 设置本地的provision信息
     *
     * @param data data
     * @param len  len
     */
    void meshProvisionParSetDir(byte[] data, int len) {
        if (meshLib == null) return;
        saveLog("meshProvisionParSetDir");
        byte result = meshLib.meshProvisionParSetDir(data, len);
        if (result == MSG_COMPOSITE_ERR) {
            // todo
            saveLog("set local pv data err!");
        }
    }

    protected void setLocalAddress(int address) {
        this.localAddress = address;

    }

    /**
     * 重置所有设备信息
     *
     * @param nodesData all nodes data
     */
    void reattachNodes(byte[][] nodesData) {
        if (meshLib == null) return;
        meshLib.reattachNodes(nodesData);
    }

    public void removeNodeInfo(int address) {
        if (meshLib != null) {
            meshLib.removeNodeInfo(address);
        }
    }

    public void addNodeInfo(int address, byte[] nodeInfo) {
        if (meshLib != null) {
            meshLib.addNodeInfo(address, nodeInfo);
        }
    }

    void resetAppKey(int appKeyIndex, int netKeyIndex, byte[] appKey) {
        if (meshLib == null) return;
        meshLib.resetAppKey(appKeyIndex, netKeyIndex, appKey);
    }

    /**
     * 设置level信息 ，addr可以是单灯，group，0xffff
     *
     * @param addr  地址
     * @param level
     */
    void cmdSetLevel(int addr, byte level) {
        if (meshLib == null) return;
        if (mDevice != null && mDevice.isConnected()) {
            if (level <= 0) {
                level = 0;
            }
            // todo rspMax 值暂定为1
            int result = meshLib.cmdSetLevel(addr, (byte) 1, level, 0);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
            } else {
                onCmdProcessing();
            }

        }
    }


    /**
     * 设置亮度信息 ，addr可以是单灯，group，0xffff
     *
     * @param addr 地址
     * @param lum  亮度值 1-100
     */
    void cmdSetLum(int addr, byte lum) {
        if (meshLib == null) return;
        if (mDevice != null && mDevice.isConnected()) {
            if (lum <= 1) {
                lum = 1;
            } else if (lum > 100) {
                lum = 100;
            }
            // todo rspMax 值暂定为1
            int result = meshLib.cmdSetLum(addr, (byte) 1, lum, 0);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
            } else {
                onCmdProcessing();
            }

        }
    }

    /**
     * 设置亮度信息 ，addr可以是单灯，group，0xffff
     *
     * @param addr 地址
     * @param temp 温度值 0-100
     */
    void cmdSetTemp(int addr, byte temp) {
        if (meshLib == null) return;
        if (mDevice != null && mDevice.isConnected()) {
//            int result = meshLib.accessCmdSetLevel(addr, percent);
            if (temp <= 0) {
                temp = 0;
            } else if (temp > 100) {
                temp = 100;
            }
            // todo rspMax 值暂定为1
            int result = meshLib.cmdSetTemp(addr, (byte) 1, temp, 0);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
            } else {
                onCmdProcessing();
            }

        }
    }

    /**
     * @param addr       unicast
     * @param rspMax     回复
     * @param hue        h
     * @param saturation s
     * @param lightness  l
     * @param ack        是否有回复
     * @param transition 渐变参数 ， 传NULL则启用设备默认， 传{0, 0}则关闭渐变
     */
    void cmdSetHSL100(int addr, byte rspMax, byte hue, byte saturation, byte lightness, int ack, byte[] transition) {
        if (meshLib == null) return;
        if (mDevice != null && mDevice.isConnected()) {

            int result = meshLib.cmdSetHSL100(addr, rspMax, hue, saturation, lightness, ack, transition);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
            } else {
                onCmdProcessing();
            }

        }
    }


    void cmdSetDelta(int adr, byte rspMax, int delta, int ack, byte[] transition) {
        if (meshLib == null) return;
        if (mDevice != null && mDevice.isConnected()) {
            int result = meshLib.cmdSetDelta(adr, rspMax, delta, ack, transition);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
            } else {
                onCmdProcessing();
            }

        }
    }


    /**
     * 获取状态信息
     *
     * @param addr   地址
     * @param rspMax 回复次数
     * @return 指令是否发送成功
     */
    boolean cmdGetLevel(int addr, int rspMax) {
        if (meshLib == null) return false;
        if (mDevice != null && mDevice.isConnected()) {
            int result = meshLib.cmdGetLevel(addr, rspMax);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
                return false;
            } else {
                onCmdProcessing();
                return true;
            }
        }

        return false;
    }

    /**
     * 获取状态信息
     *
     * @param addr   地址
     * @param rspMax 回复次数
     * @return 指令是否发送成功
     */
    boolean cmdGetLum(int addr, int rspMax) {
        if (meshLib == null) return false;
        if (mDevice != null && mDevice.isConnected()) {
            int result = meshLib.cmdGetLum(addr, rspMax);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
                return false;
            } else {
                onCmdProcessing();
                return true;
            }
        }
        return false;
    }

    /**
     * 获取开关状态
     *
     * @param addr   地址
     * @param rspMax 回复次数
     * @return 指令是否发送成功
     */
    boolean cmdGetOnOff(int addr, int rspMax) {
        if (meshLib == null) return false;
        if (mDevice != null && mDevice.isConnected()) {
            int result = meshLib.cmdGetOnOff(addr, rspMax);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
                return false;
            } else {
                onCmdProcessing();
                return true;
            }
        }
        return false;
    }

    /**
     * 获取状态信息
     *
     * @param addr   地址
     * @param rspMax 回复次数
     * @return 指令是否发送成功
     */
    boolean cmdGetTemp(int addr, int rspMax) {
        if (meshLib == null) return false;
        if (mDevice != null && mDevice.isConnected()) {
            int result = meshLib.cmdGetTemp(addr, rspMax);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
                return false;
            } else {
                onCmdProcessing();
                return true;
            }
        }

        return false;

    }

    boolean cmdGetCtl(int addr, int rspMax) {
        if (meshLib == null) return false;
        if (mDevice != null && mDevice.isConnected()) {
            int result = meshLib.cmdGetCtl(addr, rspMax);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
                return false;
            } else {
                onCmdProcessing();
                return true;
            }
        }

        return false;

    }


    /**
     * 删除节点信息
     *
     * @param addr
     * @return
     */
    boolean cmdDeleteNode(int addr) {
        if (meshLib == null) return false;
        return meshLib.cmdDeleteNode(addr) == CMD_RESULT_OK;
        /*if (mDevice != null && mDevice.isConnected()) {
            return meshLib.cmdDeleteNode(addr) == CMD_RESULT_OK;
        }*/
//        return false;
    }

    void sendMeshTestCmd(int addr, int mode) {
        if (meshLib == null) return;
        switch (mode) {
            case 3:
                meshLib.meshSendClProxyBv03(addr);
                break;

            case 4:
                meshLib.meshSendClProxyBv04(addr);
                break;

            case 5:
                meshLib.meshSendClProxyBv05(addr);
                break;

            case 6:
                meshLib.meshSendClProxyBv06(addr);
                break;

            case 7:
                meshLib.meshSendClProxyBv07(addr);
                break;

        }
    }

    protected void filterInit(int adr) {
        meshLib.meshProxySetFilterInit(adr);
        mDelayHandler.removeCallbacks(filterTimeoutTask);
        mDelayHandler.postDelayed(filterTimeoutTask, 1500);
    }

    private Runnable filterTimeoutTask = new Runnable() {
        @Override
        public void run() {
            saveLog("filter init timeout");
            mDevice.disconnect();
        }
    };


    boolean sendMeshCommand(MeshCommand command) {
        saveLog("Send Mesh Command : " + command.toString());
        if (mDevice == null || !mDevice.isConnected()) {
            return false;
        }
        byte[] params = command.toBytes();
        boolean reliable;
        final OpcodeType type = MeshUtils.getOpType(command.opcode);
        // for vendor command , params[0] is rsp opcode, so check it to make sure if command reliable
        if (type == OpcodeType.VENDOR) {
            reliable = command.params != null && command.params.length >= 1 && command.params[0] != 0x00;
        } else {
            reliable = meshLib.isOpReliable(command.opcode);
        }

        /*
         * result: 0 success, -1 busy, -2 fifo err
         */
        int result = meshLib.sendOpByINI(params, params.length);
        if (result == 0) {
            // if dest address == local address, reliable stop callback will not be invoked, such as mesh ota distribute start
            if (reliable && command.destAddress != localAddress) {
                // reliable command processing
                onCmdProcessing(command);
            } else {
                // unreliable command processing
                saveLog("process unreliable command");
            }
            return true;
        } else if (result == -1) {
            if (reliable) {
                onMeshCommandBusy(command);
            } else {
                saveLog("error !! unreliable cmd BUSY !!!");
            }
        } else if (result == -2) {
            saveLog("warning !! command error !!!  reliable -- " + reliable + " result -- " + result);
            onMeshCommandError(command);
        } else {
            saveLog("warning !! command error !!!  reliable -- " + reliable + " result -- " + result);
            onMeshCommandError(command);
        }
        return false;
    }


    /**
     * @return 0 success, -1 busy, -2 push fifo err
     */
    boolean sendOpByINI(byte[] params) {
        if (meshLib == null || params == null || params.length == 0) return false;
        if (mDevice != null && mDevice.isConnected()) {

            int result = meshLib.sendOpByINI(params, params.length);
            saveLog("sendOpByINI result: " + result);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
                return false;
            } else {
                onCmdProcessing();
                return true;
            }

        }
        return false;
    }

    void cmdSetTime(int addr, int rspMax, long taiTime, int zoneOffset) {
        if (meshLib == null) return;
        if (mDevice != null && mDevice.isConnected()) {

            int result = meshLib.cmdSetTime(addr, rspMax, taiTime, zoneOffset);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
            } else {
                onCmdProcessing();
            }

        }
    }

    void cmdSetScheduler(int addr, int rspMax, long schedulerParam, int sceneId, int ack) {
        if (meshLib == null) return;
        if (mDevice != null && mDevice.isConnected()) {

            int result = meshLib.cmdSetScheduler(addr, rspMax, schedulerParam, sceneId, ack);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
            } else {
                onCmdProcessing();
            }

        }
    }

    void cmdGetScheduler(int addr, int rspMax, byte index) {
        if (meshLib == null) return;
        if (mDevice != null && mDevice.isConnected()) {

            int result = meshLib.cmdGetScheduler(addr, rspMax, index);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
            } else {
                onCmdProcessing();
            }

        }
    }

    void cmdSceneStore(int adr, int rspMax, int id, int ack) {
        if (meshLib == null) return;
        if (mDevice != null && mDevice.isConnected()) {
            int result = meshLib.cmdSceneStore(adr, rspMax, id, ack);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
            } else {
                onCmdProcessing();
            }
        }
    }

    void cmdSceneDelete(int adr, int rspMax, int id, int ack) {
        if (meshLib == null) return;
        if (mDevice != null && mDevice.isConnected()) {
            int result = meshLib.cmdSceneDelete(adr, rspMax, id, ack);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
            } else {
                onCmdProcessing();
            }

        }
    }


    void cmdSceneRecall(int adr, int rspMax, int id, int ack, byte[] transition) {
        if (meshLib == null) return;
        if (mDevice != null && mDevice.isConnected()) {
            int result = meshLib.cmdSceneRecall(adr, rspMax, id, ack, transition);
            if (result != CMD_RESULT_OK) {
                onMeshCommandBusy();
            } else {
                onCmdProcessing();
            }

        }
    }

    private void onCommandEvent(String eventType, MeshCommand command) {
        Intent intent = new Intent();
        intent.setAction(ACTION_COMMAND_EVENT);
        intent.putExtra(EXTRA_EVENT_TYPE, eventType);
        intent.putExtra(EXTRA_MESH_COMMAND, command);
        LocalBroadcastManager.getInstance(mContext).sendBroadcast(intent);
    }

    private void onMeshCommandError(MeshCommand meshCommand) {
        onCommandEvent(CommandEvent.EVENT_TYPE_CMD_ERROR_UNKNOWN, meshCommand);
    }

    private void onMeshCommandBusy(MeshCommand command) {
        saveLog("Error: CMD busy!");
        onCommandEvent(CommandEvent.EVENT_TYPE_CMD_ERROR_BUSY, command);
    }

    private void onMeshCommandBusy() {
        onMeshCommandBusy(null);
    }

    private void onCmdProcessing(MeshCommand command) {
        synchronized (CMD_LOCK) {
            processingCmd = command;
            onCommandEvent(CommandEvent.EVENT_TYPE_CMD_PROCESSING, command);
        }
    }

    private void onCmdProcessing() {
        onCmdProcessing(null);
    }

    private void onCmdComplete(int opCode, int rspMax, int rspCnt) {
//        checkLinkStatus()
        saveLog("cmd complete opCode -- " + String.format("%06X", opCode) + " rspMax -- " + rspMax + " rspCnt -- " + rspCnt);
        synchronized (CMD_LOCK) {
            if (processingCmd != null) {
                final OpcodeType type = MeshUtils.getOpType(opCode);
                int processingOpcode = processingCmd.opcode;
                if (type == OpcodeType.VENDOR){
                    processingOpcode &= 0xFF;
                }
                if (processingOpcode == opCode) {
                    processingCmd.rspCnt = rspCnt;
                    onCommandEvent(CommandEvent.EVENT_TYPE_CMD_COMPLETE, processingCmd);
                    processingCmd = null;
                } else {
                    saveLog("cmd complete error !!! target cmd err");
                }
            } else {
                MeshCommand meshCommand = new MeshCommand();
                meshCommand.opcode = opCode;
                meshCommand.rspMax = (byte) rspMax;
                meshCommand.rspCnt = rspCnt;
                onCommandEvent(CommandEvent.EVENT_TYPE_CMD_COMPLETE, meshCommand);
            }
        }
    }

    public void ivTest() {
        if (meshLib != null) {
            meshLib.ivTest();
        }
    }

    public void snoTest() {
        if (meshLib != null) {
            meshLib.snoTest();
        }
    }


    /*
     cmd  end
     *******************************/

    private void saveLog(String log) {
        TelinkLog.d(log);
        TelinkApplication.getInstance().saveLog("Controller -> " + log);
    }

    private void storeMeshParams(byte params, int address, int flag) {

    }
}
