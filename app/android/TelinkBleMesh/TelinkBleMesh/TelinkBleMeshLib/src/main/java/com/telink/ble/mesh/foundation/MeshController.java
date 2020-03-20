package com.telink.ble.mesh.foundation;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGattService;
import android.content.Context;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.ParcelUuid;
import android.util.SparseArray;

import com.telink.ble.mesh.core.Encipher;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.access.AccessBridge;
import com.telink.ble.mesh.core.access.BindingBearer;
import com.telink.ble.mesh.core.access.BindingController;
import com.telink.ble.mesh.core.access.MeshUpdatingController;
import com.telink.ble.mesh.core.access.RemoteProvisioningController;
import com.telink.ble.mesh.core.ble.GattController;
import com.telink.ble.mesh.core.ble.LeScanFilter;
import com.telink.ble.mesh.core.ble.LeScanSetting;
import com.telink.ble.mesh.core.ble.MeshScanRecord;
import com.telink.ble.mesh.core.ble.ScanningController;
import com.telink.ble.mesh.core.ble.UuidInfo;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.StatusMessage;
import com.telink.ble.mesh.core.networking.NetworkingBridge;
import com.telink.ble.mesh.core.networking.NetworkingController;
import com.telink.ble.mesh.core.provisioning.ProvisioningBridge;
import com.telink.ble.mesh.core.provisioning.ProvisioningController;
import com.telink.ble.mesh.core.proxy.ProxyPDU;
import com.telink.ble.mesh.entity.AdvertisingDevice;
import com.telink.ble.mesh.entity.BindingDevice;
import com.telink.ble.mesh.entity.MeshUpdatingConfiguration;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.entity.ProvisioningDevice;
import com.telink.ble.mesh.entity.RemoteProvisioningDevice;
import com.telink.ble.mesh.foundation.event.AutoConnectEvent;
import com.telink.ble.mesh.foundation.event.BindingEvent;
import com.telink.ble.mesh.foundation.event.GattOtaEvent;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.event.MeshUpdatingEvent;
import com.telink.ble.mesh.foundation.event.NetworkInfoUpdateEvent;
import com.telink.ble.mesh.foundation.event.OnlineStatusEvent;
import com.telink.ble.mesh.foundation.event.ProvisioningEvent;
import com.telink.ble.mesh.foundation.event.RemoteProvisioningEvent;
import com.telink.ble.mesh.foundation.event.ScanEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.foundation.parameter.AutoConnectFilterType;
import com.telink.ble.mesh.foundation.parameter.AutoConnectParameters;
import com.telink.ble.mesh.foundation.parameter.BindingParameters;
import com.telink.ble.mesh.foundation.parameter.GattOtaParameters;
import com.telink.ble.mesh.foundation.parameter.MeshOtaParameters;
import com.telink.ble.mesh.foundation.parameter.Parameters;
import com.telink.ble.mesh.foundation.parameter.ProvisioningParameters;
import com.telink.ble.mesh.foundation.parameter.ScanParameters;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.TelinkLog;

import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;
import java.util.UUID;

/**
 * Created by kee on 2018/12/11.
 */

public final class MeshController implements ProvisioningBridge, NetworkingBridge, AccessBridge {
    private final String TAG = "MeshController -- ";

    /**
     * proxy node advertising networkId
     */
    private final static int PROXY_ADV_TYPE_NETWORK_ID = 0x00;

    /**
     * proxy node advertising nodeIdentity
     */
    private final static int PROXY_ADV_TYPE_NODE_IDENTITY = 0x01;

    /**
     * controllers
     */
    private ScanningController mScanningController;

    /**
     * gatt controller for gatt connection
     */
    private GattController mGattController;

    private Context mContext;

    private HandlerThread handlerThread;

    private Handler mDelayHandler;

    private ProvisioningController mProvisioningController;

    private NetworkingController mNetworkingController;

    private BindingController mBindingController;

    private MeshUpdatingController mMeshUpdatingController;

    /**
     * rely on ProvisioningController
     */
    private RemoteProvisioningController mRemoteProvisioningController;


    private Mode actionMode = Mode.MODE_IDLE;

    /**
     * parameters for different action
     */
    private Parameters mActionParams;

    /**
     * is disconnecting when reconnect
     */
    private boolean isDisconnectWaiting = false;
    /**
     * lock for scanning
     */
    private final Object SCAN_LOCK = new Object();

    private boolean isScanning = false;

    private Set<AdvertisingDevice> advDevices = new LinkedHashSet<>();

    private MeshConfiguration meshConfiguration;

    private byte[] networkId = null;

    private byte[] networkIdentityKey = null;

    private byte[] networkBeaconKey = null;

    private boolean isLogin = false;

    /**
     * action started flag when provisioning, binding, ota,  mesh-updating
     */
    private boolean isActionStarted = false;

//    private boolean isProvisionProcessing = false;

//    private boolean isKeyBindProcessing = false;

    private static final int MAX_CONNECT_RETRY = 3;

    private int connectRetry = 0;


    /**
     * direct connected device address when auto connect mesh
     */
    private int directDeviceAddress = 0;


    void start(Context context) {

        handlerThread = new HandlerThread("Mesh-Controller");
        handlerThread.start();
        mDelayHandler = new Handler(handlerThread.getLooper());

        mContext = context.getApplicationContext();
        initScanningController(handlerThread);
        initGattController(handlerThread);
        initProvisioningController(handlerThread);
        initNetworkingController(handlerThread);
        initAccessController(handlerThread);
    }

    void stop() {
        this.actionMode = Mode.MODE_IDLE;
        this.directDeviceAddress = 0;
        this.isLogin = false;
        stopScan();
        if (advDevices != null) {
            advDevices.clear();
        }

        if (mGattController != null) {
            mGattController.disconnect();
            mGattController.setCallback(null);
            mGattController = null;
        }

        if (mProvisioningController != null) {
            mProvisioningController.clear();
            mProvisioningController = null;
        }

        if (mNetworkingController != null) {
            mNetworkingController.clear();
            mNetworkingController = null;
        }

        if (mBindingController != null) {
            mBindingController.clear();
            mBindingController = null;
        }

        if (mMeshUpdatingController != null) {
            mMeshUpdatingController.clear();
            mMeshUpdatingController = null;
        }

        if (mDelayHandler != null) {
            mDelayHandler.removeCallbacksAndMessages(null);
            mDelayHandler = null;
        }

        if (handlerThread != null) {
            handlerThread.quitSafely();
            handlerThread = null;
        }
    }


    void setupMeshNetwork(MeshConfiguration configuration) {
        this.meshConfiguration = configuration;
        this.networkId = Encipher.k3(configuration.networkKey);
        this.networkIdentityKey = Encipher.generateIdentityKey(configuration.networkKey);
        this.networkBeaconKey = Encipher.generateBeaconKey(configuration.networkKey);
        mNetworkingController.setup(configuration);
    }

    void removeDevice(int meshAddress) {
        this.meshConfiguration.deviceKeyMap.remove(meshAddress);
    }

    private void initScanningController(HandlerThread handlerThread) {
        mScanningController = new ScanningController(mContext, handlerThread);
        mScanningController.setScanCallback(this.scanCallback);
    }

    private void initGattController(HandlerThread handlerThread) {
        mGattController = new GattController(mContext, handlerThread);
        mGattController.setCallback(deviceCallback);

    }

    private void initProvisioningController(HandlerThread handlerThread) {
        mProvisioningController = new ProvisioningController(handlerThread);
        mProvisioningController.setProvisioningBridge(this);
    }

    private void initNetworkingController(HandlerThread handlerThread) {
        mNetworkingController = new NetworkingController(handlerThread);
        mNetworkingController.setNetworkingBridge(this);
    }

    private void initAccessController(HandlerThread handlerThread) {
        mBindingController = new BindingController(handlerThread);
        mBindingController.register(this);

        mMeshUpdatingController = new MeshUpdatingController(handlerThread);
        mMeshUpdatingController.register(this);

        mRemoteProvisioningController = new RemoteProvisioningController(handlerThread);
        mRemoteProvisioningController.register(this);
    }


    boolean isProxyLogin() {
        return isLogin;
    }

    int getDirectNodeAddress() {
        return directDeviceAddress;
    }

    public void idle(boolean disconnect) {
        TelinkLog.d(TAG + "---idle--- " + disconnect);
        this.actionMode = Mode.MODE_IDLE;
        mDelayHandler.removeCallbacksAndMessages(null);
        if (disconnect) {
            mGattController.disconnect();
        }
        stopScan();
    }

    public boolean isBluetoothEnabled() {
        return mScanningController.isEnabled();
    }

    public void enableBluetooth() {
        mScanningController.enableBluetooth();
    }

    public String getCurDeviceMac() {
        if (mGattController.isConnected()) {
            return mGattController.getMacAddress();
        }
        return null;
    }

    private void stopScan() {
        isScanning = false;
        mScanningController.stopScan();
    }

    public void connect(BluetoothDevice device) {
        stopScan();
        mGattController.connect(device);
    }

    public void disconnect() {
        mGattController.disconnect();
    }

    public void startScan(ScanParameters scanParameters) {
        if (!validateActionMode(Mode.MODE_SCAN)) return;
        mDelayHandler.removeCallbacksAndMessages(null);
        this.actionMode = Mode.MODE_SCAN;
        advDevices.clear();
        this.mActionParams = scanParameters;
        resetAction();
        if (!mGattController.disconnect()) {
            startScan();
        } else {
            TelinkLog.d(TAG + "waiting for disconnect...");
        }
    }


    private void startScan() {
        TelinkLog.d(TAG + "start scan");
        LeScanFilter filter = (LeScanFilter) mActionParams.get(Parameters.SCAN_FILTERS);
        LeScanSetting scanSetting = new LeScanSetting(500, 10 * 1000);
//        LeScanSetting scanSetting = LeScanSetting.getDefault();
        mScanningController.startScan(filter, scanSetting);
    }


    void startProvisioning(ProvisioningParameters parameters) {
        TelinkLog.d(TAG + "start provision");
        if (!validateActionMode(Mode.MODE_PROVISION)) return;
        mDelayHandler.removeCallbacksAndMessages(null);
        this.actionMode = Mode.MODE_PROVISION;
        this.mProvisioningController.setProvisioningBridge(this);
        this.mActionParams = parameters;
        resetAction();
        ProvisioningDevice provisioningDevice = (ProvisioningDevice) parameters.get(Parameters.ACTION_PROVISIONING_TARGET);
        rebuildProvisioningDevice(provisioningDevice);
        if (mGattController.disconnect()) {
            this.isDisconnectWaiting = false;
        } else {
            connect(provisioningDevice.getAdvertisingDevice().device);
        }
    }

    private void rebuildProvisioningDevice(ProvisioningDevice provisioningDevice) {
        provisioningDevice.setIvIndex(this.meshConfiguration.ivIndex);
        provisioningDevice.setNetworkKey(this.meshConfiguration.networkKey);
        provisioningDevice.setNetworkKeyIndex(this.meshConfiguration.netKeyIndex);
        provisioningDevice.setIvUpdateFlag((byte) 0);
        provisioningDevice.setKeyRefreshFlag((byte) 0);
    }


    void startBinding(BindingParameters bindingParameters) {
        if (!validateActionMode(Mode.MODE_BIND)) {
            TelinkLog.d(TAG + "binding currently");
            return;
        }
        mDelayHandler.removeCallbacksAndMessages(null);
        BindingDevice bindingDevice = (BindingDevice) bindingParameters.get(Parameters.ACTION_BINDING_TARGET);

        if (!MeshUtils.validUnicastAddress(bindingDevice.getMeshAddress())) {
            TelinkLog.w(TAG + "start bind fail: node address invalid");
            return;
        }

        this.mActionParams = bindingParameters;
        this.actionMode = Mode.MODE_BIND;
        resetAction();

        //
        int bindingTarget = bindingDevice.getMeshAddress();
        BindingBearer bindingBearer = bindingDevice.getBearer();
        if (mGattController.isProxyNodeConnected() && (bindingDevice.getMacAddress().equals(mGattController.getMacAddress()) || bindingBearer == BindingBearer.Any)) {
            onConnectSuccess();
        } else {
            if (mGattController.disconnect()) {
                // waiting for callback
                isDisconnectWaiting = true;
            } else {
                startScan();
            }
        }
    }

    void startRemoteProvision(RemoteProvisioningDevice remoteProvisioningDevice) {
        if (!validateActionMode(Mode.MODE_REMOTE_PROVISION)) {
            TelinkLog.d(TAG + "remote provisioning currently");
            return;
        }
        mDelayHandler.removeCallbacksAndMessages(null);
        if (!mGattController.isProxyNodeConnected()) {
            onRemoteProvisionFail("proxy node not connected");
            return;
        }
        this.actionMode = Mode.MODE_REMOTE_PROVISION;
        rebuildProvisioningDevice(remoteProvisioningDevice);
        mRemoteProvisioningController.begin(this.mProvisioningController, meshConfiguration.getDefaultAppKeyIndex(), remoteProvisioningDevice);
    }


    private void onRemoteProvisionFail(String desc) {

    }

    void autoConnect(AutoConnectParameters parameters) {
        if (!validateActionMode(Mode.MODE_AUTO_CONNECT)) {
            TelinkLog.d(TAG + "auto connect currently");
            return;
        }
        mDelayHandler.removeCallbacksAndMessages(null);
        this.mActionParams = parameters;
        TelinkLog.d(TAG + "auto connect");
        this.actionMode = Mode.MODE_AUTO_CONNECT;
        if (!validateAutoConnectTargets()) {
            return;
        }
        if (mGattController.isProxyNodeConnected()) {
            if (isLogin) {
                onAutoConnectSuccess();
            } else {
                proxyFilterInit();
            }
        } else {
            if (mGattController.disconnect()) {
                // waiting for disconnected
                isDisconnectWaiting = true;
            } else {
                startScan();
            }
        }
    }


    void startGattOta(GattOtaParameters otaParameters) {
        if (!validateActionMode(Mode.MODE_OTA)) {
            TelinkLog.d(TAG + "ota running currently");
            return;
        }
        mDelayHandler.removeCallbacksAndMessages(null);
        this.mActionParams = otaParameters;
        this.actionMode = Mode.MODE_OTA;
        resetAction();
        int meshAddress = otaParameters.getInt(Parameters.ACTION_OTA_MESH_ADDRESS, -1);
        if (mGattController.isProxyNodeConnected() && directDeviceAddress == meshAddress) {
            startGattOta();
        } else {
            if (mGattController.disconnect()) {
                // waiting for disconnected
                isDisconnectWaiting = true;
            } else {
                startScan();
            }
        }
    }

    void startMeshOTA(MeshOtaParameters meshOtaParameters) {
        if (!validateActionMode(Mode.MODE_MESH_OTA)) {
            TelinkLog.d(TAG + "mesh updating running currently");
            return;
        }
        mDelayHandler.removeCallbacksAndMessages(null);
        this.mActionParams = meshOtaParameters;
        this.actionMode = Mode.MODE_MESH_OTA;
        resetAction();
        if (mGattController.isProxyNodeConnected()) {
            onActionStart();
            mMeshUpdatingController.begin((MeshUpdatingConfiguration) mActionParams.get(Parameters.ACTION_MESH_OTA_CONFIG));
        } else {
            if (mGattController.disconnect()) {
                // waiting for disconnected
                isDisconnectWaiting = true;
            } else {
                startScan();
            }
        }
    }

    void stopMeshOta() {
        if (actionMode != Mode.MODE_MESH_OTA) {
            TelinkLog.d(TAG + "mesh updating stop: not running...");
            return;
        }
        mMeshUpdatingController.stop();
    }

    private boolean validateActionMode(Mode targetMode) {
        if (actionMode == targetMode) {
            return false;
        } else {
            /*if (actionMode == MODE_AUTO_CONNECT) {
                onMeshEvent(MeshEvent.EVENT_TYPE_DISCONNECTED);
            }*/
            if (actionMode == Mode.MODE_REMOTE_PROVISION) {
                mRemoteProvisioningController.clear();
            } else if (actionMode == Mode.MODE_PROVISION) {
                mProvisioningController.clear();
            } else if (actionMode == Mode.MODE_BIND) {
                mBindingController.clear();
            } else if (actionMode == Mode.MODE_MESH_OTA) {
                mMeshUpdatingController.clear();
            }

            actionMode = targetMode;
            return true;
        }
    }

    private void startGattOta() {
        byte[] otaFirmware = (byte[]) mActionParams.get(Parameters.ACTION_OTA_FIRMWARE);
        if (otaFirmware == null) {
            onOtaComplete(false, "firmware not found");
        } else {
            onActionStart();
            mGattController.startOta(otaFirmware);
        }
    }

    private boolean validateAutoConnectTargets() {

        if (meshConfiguration.deviceKeyMap.size() == 0) {
            idle(false);
            onMeshEvent(MeshEvent.EVENT_TYPE_MESH_EMPTY, "mesh empty");
            return false;
        }
//        AutoConnectFilterType filterType = (AutoConnectFilterType) mActionParams.get(Parameters.ACTION_AUTO_CONNECT_FILTER_TYPE);
        /*if (!filterType.isNetworkIdSupport) {
            if (meshConfiguration.deviceKeyMap.size() == 0) {
                idle(true);
                onMeshEvent(MeshEvent.EVENT_TYPE_MESH_EMPTY, "mesh empty");
                return false;
            }
        }*/
        return true;
    }

    private void onMeshEvent(String eventType, String desc) {
        MeshEvent meshEvent = new MeshEvent(this, eventType, desc);
        onEventPrepared(meshEvent);
    }

    private void onEventPrepared(Event event) {
        if (callback != null) {
            callback.onEventPrepared(event);
        }
    }

    private void onActionStart() {
        isActionStarted = true;
    }

    private void resetAction() {
        isActionStarted = false;
        connectRetry = 0;
    }

    /**
     * connected and filter set success
     */
    private void onAutoConnectSuccess() {
        onAutoConnectEvent(AutoConnectEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN);
    }


    private void onAutoConnectEvent(String eventType) {
        AutoConnectEvent autoConnectEvent = new AutoConnectEvent(this, eventType, this.directDeviceAddress);
        onEventPrepared(autoConnectEvent);
    }

    public boolean getOnlineStatus() {
        return isLogin && mGattController.enableOnlineStatus();
    }

    /**
     * send mesh message
     * include config message and common message
     */
    public boolean sendMeshMessage(MeshMessage meshMessage) {
        if (!isLogin) {
            TelinkLog.d(TAG + "not login when sending message");
            return false;
        }
        TelinkLog.d(TAG + "send mesh message: " + meshMessage.getClass().getSimpleName()
                + String.format(" opcode: 0x%04X -- dst: 0x%04X", meshMessage.getOpcode(), meshMessage.getDestinationAddress())
                + " isReliable: " + meshMessage.isReliable()
                + " retryCnt: " + meshMessage.getRetryCnt());
        return mNetworkingController.sendMeshMessage(meshMessage);
    }

    private void proxyFilterInit() {
        mNetworkingController.proxyFilterInit();
    }

    /**
     * post online_status event
     */
    private void onOnlineStatusNotify(byte[] data) {
        OnlineStatusEvent onlineStatusEvent = new OnlineStatusEvent(this, data);
        onEventPrepared(onlineStatusEvent);
    }

    /**
     * gatt notification
     *
     * @param completePacket online_status raw data or mesh complete data
     */
    private void onGattNotification(byte[] completePacket) {
        if (completePacket.length > 1) {
            byte proxyPduType = (byte) (completePacket[0] & ProxyPDU.BITS_TYPE);

            byte[] payloadData = new byte[completePacket.length - 1];
            System.arraycopy(completePacket, 1, payloadData, 0, payloadData.length);

            switch (proxyPduType) {
                case ProxyPDU.TYPE_PROVISIONING_PDU:
                    // provision packet
                    if (mProvisioningController != null && isActionStarted) {
                        mProvisioningController.pushNotification(payloadData);
                    }
                    break;

                case ProxyPDU.TYPE_NETWORK_PDU:

                    // proxy packet
                    if (mNetworkingController != null) {
                        mNetworkingController.parseNetworkPdu(payloadData);
                    }
                    break;

                case ProxyPDU.TYPE_PROXY_CONFIGURATION:
                    if (mNetworkingController != null) {
                        mNetworkingController.parseProxyConfigurationPdu(payloadData);
                    }
                    break;


                case ProxyPDU.TYPE_MESH_BEACON:
                    if (mNetworkingController != null) {
                        mNetworkingController.parseMeshBeacon(payloadData, this.networkId, this.networkBeaconKey);
                    }
                    break;
            }

        }

    }


    private void onConnectSuccess() {

        if (actionMode == Mode.MODE_PROVISION) {
            ProvisioningDevice provisioningDevice = (ProvisioningDevice) mActionParams.get(Parameters.ACTION_PROVISIONING_TARGET);
            onActionStart();
            mProvisioningController.begin(provisioningDevice);
        } else {
            boolean isFilterInitNeeded = mActionParams.getBool(Parameters.COMMON_PROXY_FILTER_INIT_NEEDED, false);
            if (!isLogin && isFilterInitNeeded) {
                proxyFilterInit();
            } else {
                // no need to init proxy filter
                onProxyLoginSuccess();
            }
        }
    }

    private void onProxyLoginSuccess() {
        isLogin = true;
        switch (actionMode) {
            case MODE_AUTO_CONNECT:
                this.onAutoConnectSuccess();
                break;

            case MODE_BIND:
                mDelayHandler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        onActionStart();
                        BindingDevice bindingDevice = (BindingDevice) mActionParams.get(Parameters.ACTION_BINDING_TARGET);
                        mBindingController.begin(meshConfiguration.netKeyIndex,
                                meshConfiguration.appKeyMap.get(bindingDevice.getAppKeyIndex()), bindingDevice);
                    }
                }, 500);
                break;

            case MODE_OTA:
                startGattOta();
                break;

            case MODE_MESH_OTA:
                onActionStart();
                mMeshUpdatingController.begin((MeshUpdatingConfiguration) mActionParams.get(Parameters.ACTION_MESH_OTA_CONFIG));
                break;
        }
    }

    private void onConnectionInterrupt() {
        String desc = "connection interrupt";
        switch (actionMode) {
            case MODE_PROVISION:
                ProvisioningDevice device = mProvisioningController.getProvisioningDevice();
                onProvisionFailed(device, desc);
                break;
            case MODE_BIND:
                onBindingFail(desc);
                break;

            case MODE_OTA:
                onOtaComplete(false, desc);
                break;

            case MODE_MESH_OTA:
                onMeshUpdatingComplete();
                onMeshUpdatingEvent(MeshUpdatingEvent.EVENT_TYPE_UPDATING_FAIL,
                        null,
                        desc, -1);
                break;
        }
    }

    private void onConnectionFail() {
        String desc = "connect fail";
        switch (actionMode) {
            case MODE_PROVISION:
                ProvisioningDevice provisioningDevice = (ProvisioningDevice) mActionParams.get(Parameters.ACTION_PROVISIONING_TARGET);
                onProvisionFailed(provisioningDevice, "connect fail");
                break;
            case MODE_BIND:
                onBindingFail(desc);
                break;

            case MODE_OTA:
                onOtaComplete(false, desc);
                break;

            case MODE_MESH_OTA:
                onMeshUpdatingComplete();
                onMeshUpdatingEvent(MeshUpdatingEvent.EVENT_TYPE_UPDATING_FAIL,
                        null,
                        desc, -1);
                break;
        }
    }


    private void onGattDisconnected() {
        mDelayHandler.removeCallbacksAndMessages(null);
        onMeshEvent(MeshEvent.EVENT_TYPE_DISCONNECTED, "disconnected when: " + actionMode);
        mNetworkingController.onGattDisconnected();
        if (isDisconnectWaiting) {
            isDisconnectWaiting = false;
            connectRetry = -1;
        }
        isLogin = false;
        directDeviceAddress = 0;
        if (actionMode != Mode.MODE_IDLE) {
            mDelayHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    switch (actionMode) {
                        case MODE_SCAN:
                        case MODE_AUTO_CONNECT:
                            startScan();
                            break;

                        case MODE_PROVISION:
                        case MODE_BIND:
                        case MODE_OTA:
                        case MODE_MESH_OTA:
                            if (isActionStarted) {
                                onConnectionInterrupt();
                            } else {
                                connectRetry++;
                                if (connectRetry >= MAX_CONNECT_RETRY) {
                                    // connect retry max
                                    onConnectionFail();
                                } else {
                                    // reconnect when provision and re-scan when other mode
                                    if (actionMode == Mode.MODE_PROVISION) {
                                        ProvisioningDevice provisioningDevice = (ProvisioningDevice) mActionParams.get(Parameters.ACTION_PROVISIONING_TARGET);
                                        TelinkLog.d(TAG + "provisioning connect retry: " + connectRetry);
                                        connect(provisioningDevice.getAdvertisingDevice().device);
                                    } else {
                                        startScan();
                                    }
                                }
                            }
                            break;
                    }
                }
            }, 1000);
        }

    }

    private void onOtaComplete(boolean success, String desc) {
        resetAction();
        this.actionMode = Mode.MODE_IDLE;
        String evenType = success ?
                GattOtaEvent.EVENT_TYPE_OTA_SUCCESS : GattOtaEvent.EVENT_TYPE_OTA_FAIL;
        onOtaEvent(evenType, 0, desc);
    }

    private void onOtaProgress(int progress) {
        onOtaEvent(GattOtaEvent.EVENT_TYPE_OTA_PROGRESS, progress, "ota progress update");
    }

    private void onOtaEvent(String eventType, int progress, String desc) {
        int meshAddress = mActionParams.getInt(Parameters.ACTION_OTA_MESH_ADDRESS, -1);
        GattOtaEvent otaEvent = new GattOtaEvent(this, eventType, meshAddress, progress, desc);
        onEventPrepared(otaEvent);
    }

    private GattController.DeviceCallback deviceCallback = new GattController.DeviceCallback() {
        @Override
        public void onConnected() {

        }

        @Override
        public void onDisconnected() {
            MeshController.this.onGattDisconnected();
        }

        @Override
        public void onServicesDiscovered(List<BluetoothGattService> services) {
            mDelayHandler.removeCallbacksAndMessages(null);

            mDelayHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    if (actionMode == Mode.MODE_PROVISION) {
                        mGattController.provisionInit();
                    } else {
                        mGattController.proxyInit();
                        mNetworkingController.checkSequenceNumber(networkId, networkBeaconKey);
                    }

                    mDelayHandler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            onConnectSuccess();
                        }
                    }, 100);
                }
            }, 500);

        }

        @Override
        public void onNotifyEnable() {

        }

        @Override
        public void onNotify(UUID charUUID, byte[] data) {
            if (charUUID.equals(UuidInfo.CHARACTERISTIC_UUID_ONLINE_STATUS)) {
                TelinkLog.d(TAG + "online status encrypted data: " + Arrays.bytesToHexString(data, ":"));
                byte[] decrypted = Encipher.decryptOnlineStatus(data, networkBeaconKey);
                if (decrypted != null) {
                    TelinkLog.d(TAG + "online status decrypted data: " + Arrays.bytesToHexString(decrypted, ":"));
                    onOnlineStatusNotify(decrypted);
                } else {
                    TelinkLog.d(TAG + "online status decrypt err");
                }
            } else {
                onGattNotification(data);
            }
        }

        @Override
        public void onCommandError(String errorMsg) {

        }

        @Override
        public void onCommandSuccess(UUID charUUID, byte[] data) {

        }

        @Override
        public void onOtaStateChanged(int state) {
            switch (state) {
                case GattController.OTA_STATE_FAILURE:
                    onOtaComplete(false, "gatt command fail");
                    break;

                case GattController.OTA_STATE_SUCCESS:
                    onOtaComplete(true, "ota complete");
                    break;
                case GattController.OTA_STATE_PROGRESS:
                    onOtaProgress(mGattController.getOtaProgress());
                    break;
            }
        }
    };


    private void onDeviceFound(AdvertisingDevice device) {
        TelinkLog.d(TAG + "on device found: " + device.device.getAddress());
        if (advDevices.add(device)) {
            ScanEvent scanEvent = new ScanEvent(this, ScanEvent.EVENT_TYPE_DEVICE_FOUND, device);
            onEventPrepared(scanEvent);
        } else {
            TelinkLog.d(TAG + "Device already exist");
        }
    }

    private void onScanFail() {
        this.actionMode = Mode.MODE_IDLE;
        ScanEvent scanEvent = new ScanEvent(this, ScanEvent.EVENT_TYPE_SCAN_FAIL, null);
        onEventPrepared(scanEvent);
    }

    private void onScanTimeout() {
        TelinkLog.d(TAG + "scanning timeout: " + actionMode);
        switch (actionMode) {
            case MODE_SCAN:
                this.actionMode = Mode.MODE_IDLE;
                onScanTimeoutEvent();
                break;

            case MODE_BIND:
                this.actionMode = Mode.MODE_IDLE;
                onBindingFail("device not found when scanning");
                break;

            case MODE_AUTO_CONNECT:
                mDelayHandler.postDelayed(restartScanTask, 3 * 1000);
                break;

            case MODE_OTA:
                this.actionMode = Mode.MODE_IDLE;
                onOtaComplete(false, "ota fail: scan timeout");
                break;
        }
    }

    private Runnable restartScanTask = new Runnable() {
        @Override
        public void run() {
            if (actionMode == Mode.MODE_AUTO_CONNECT)
                startScan();
        }
    };

    private void onScanTimeoutEvent() {
        this.actionMode = Mode.MODE_IDLE;
        ScanEvent scanEvent = new ScanEvent(this, ScanEvent.EVENT_TYPE_SCAN_TIMEOUT, null);
        onEventPrepared(scanEvent);
    }

    /**
     * validate advertising device when auto connect {@link Mode#MODE_AUTO_CONNECT}
     *
     * @param scanRecord
     * @return
     */
    private boolean validateProxyAdv(byte[] scanRecord) {

        MeshScanRecord sr = MeshScanRecord.parseFromBytes(scanRecord);
        byte[] serviceData = sr.getServiceData(ParcelUuid.fromString(UuidInfo.PROXY_SERVICE_UUID.toString()));
        AutoConnectFilterType filterType = (AutoConnectFilterType) mActionParams.get(Parameters.ACTION_AUTO_CONNECT_FILTER_TYPE);
        if (serviceData != null && serviceData.length >= 9) {
            int type = serviceData[0];
            if (type == PROXY_ADV_TYPE_NETWORK_ID && filterType.isNetworkIdSupport) {
                // validate network id matches
                final int networkIdLen = 8;
                byte[] advertisingNetworkId = new byte[networkIdLen];
                System.arraycopy(serviceData, 1, advertisingNetworkId, 0, networkIdLen);
                boolean networkIdCheck = Arrays.equals(networkId, advertisingNetworkId);
                TelinkLog.d(TAG + "check network id: " + networkIdCheck);
                return networkIdCheck;
            } else if (type == PROXY_ADV_TYPE_NODE_IDENTITY && filterType.isNodeIdentitySupport) {
                boolean nodeIdentityCheck = validateNodeIdentity(serviceData);
                TelinkLog.d(TAG + "check node identity: " + nodeIdentityCheck);
                return nodeIdentityCheck;
            }
        }
        return false;
    }


    private boolean validateNodeIdentity(byte[] serviceData) {
        if (this.networkIdentityKey == null || meshConfiguration.deviceKeyMap.size() == 0 || serviceData.length < 17)
            return false;

        // hash and random data length
        final int dataLen = 8;
        byte[] advHash;
        byte[] random;
        SparseArray<byte[]> deviceKeyMap = meshConfiguration.deviceKeyMap;
        int nodeAddress;
        for (int i = 0; i < deviceKeyMap.size(); i++) {
            nodeAddress = deviceKeyMap.keyAt(i);

            int index = 1;
            advHash = new byte[dataLen];
            System.arraycopy(serviceData, index, advHash, 0, dataLen);
            index += dataLen;

            random = new byte[dataLen];
            System.arraycopy(serviceData, index, random, 0, dataLen);
            byte[] hash = Encipher.generateNodeIdentityHash(networkIdentityKey, random, nodeAddress);
            if (Arrays.equals(advHash, hash)) {
                directDeviceAddress = nodeAddress;
                TelinkLog.d(TAG + "reset direct device address");
                return true;
            }
        }

        return false;
    }

    private boolean validateTargetNodeIdentity(byte[] scanRecord, int nodeAddress) {


        MeshScanRecord sr = MeshScanRecord.parseFromBytes(scanRecord);
        byte[] serviceData = sr.getServiceData(ParcelUuid.fromString(UuidInfo.PROXY_SERVICE_UUID.toString()));
//        AutoConnectFilterType filterType = (AutoConnectFilterType) mActionParams.get(Parameters.ACTION_AUTO_CONNECT_FILTER_TYPE);
        if (serviceData != null && serviceData.length >= 9) {
            int type = serviceData[0];
            if (type == PROXY_ADV_TYPE_NODE_IDENTITY) {
                final int dataLen = 8;
                int index = 1;
                byte[] advHash = new byte[dataLen];
                System.arraycopy(serviceData, index, advHash, 0, dataLen);
                index += dataLen;

                byte[] random = new byte[dataLen];
                System.arraycopy(serviceData, index, random, 0, dataLen);
                byte[] hash = Encipher.generateNodeIdentityHash(networkIdentityKey, random, nodeAddress);
                return Arrays.equals(advHash, hash);
            }
        }
        return false;
    }


    private void onScanFilter(final BluetoothDevice device, final int rssi, final byte[] scanRecord) {
//        if (!device.getAddress().toUpperCase().contains("01:FF")) return;
//        if (!device.getAddress().toUpperCase().contains("FF:BB:CC:DD")) return;
        synchronized (SCAN_LOCK) {
            if (!isScanning) return;
            boolean connectIntent = false;
            if (actionMode == Mode.MODE_AUTO_CONNECT) {
                connectIntent = validateProxyAdv(scanRecord);
            } else if (actionMode == Mode.MODE_OTA) {
                int nodeAddress = mActionParams.getInt(Parameters.ACTION_OTA_MESH_ADDRESS, -1);
                connectIntent = validateTargetNodeIdentity(scanRecord, nodeAddress);
            } else if (actionMode == Mode.MODE_BIND) {
                BindingDevice bindingDevice = (BindingDevice) mActionParams.get(Parameters.ACTION_BINDING_TARGET);
                if (bindingDevice.getBearer() == BindingBearer.GattOnly) {
                    connectIntent = validateTargetNodeIdentity(scanRecord, bindingDevice.getMeshAddress());
                } else {
                    connectIntent = validateProxyAdv(scanRecord);
                }
                /*if (device.getAddress().equals(bindingDevice.getMacAddress())) {
                    connectIntent = true;
                }*/
            } else if (actionMode == Mode.MODE_SCAN) {
//                connectIntent = false;
                LeScanFilter filter = (LeScanFilter) mActionParams.get(Parameters.SCAN_FILTERS);
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
                    boolean single = mActionParams.getBool(Parameters.SCAN_SINGLE_MODE, false);
                    if (single) {
                        stopScan();
                    }
                    onDeviceFound(new AdvertisingDevice(device, rssi, scanRecord));
                }
            }

            if (connectIntent) {
                isScanning = false;

                connect(device);
                /*mDelayHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        connect(device);
                    }
                });*/
            }
        }
    }

    private ScanningController.ScanCallback scanCallback = new ScanningController.ScanCallback() {
        @Override
        public void onLeScan(BluetoothDevice device, int rssi, byte[] scanRecord) {
//            TelinkLog.w("device type: " + device.getType());
//            if (!device.getAddress().toUpperCase().equals("A4:C1:38:3F:4C:05")) return;
            TelinkLog.w(TAG + "scan:" + device.getName() + " --mac: " + device.getAddress() + " --record: " + Arrays.bytesToHexString(scanRecord, ":"));
            onScanFilter(device, rssi, scanRecord);
        }

        @Override
        public void onScanFail(int errorCode) {
            isScanning = false;
            MeshController.this.onScanFail();
        }

        @Override
        public void onStartedScan() {
            isScanning = true;
        }

        @Override
        public void onStoppedScan() {

        }

        @Override
        public void onScanTimeout() {
            MeshController.this.onScanTimeout();
        }
    };

    private void postProvisioningEvent(String eventType, ProvisioningDevice provisioningDevice, String desc) {

        ProvisioningEvent provisioningEvent = new ProvisioningEvent(this, eventType, provisioningDevice, desc);
        onEventPrepared(provisioningEvent);
    }


    private void onProvisionFailed(ProvisioningDevice provisioningDevice, String desc) {
        TelinkLog.d(TAG + "provisioning failed: " + desc + " -- " + provisioningDevice.getUnicastAddress());
        onProvisionComplete();
        postProvisioningEvent(ProvisioningEvent.EVENT_TYPE_PROVISION_FAIL, provisioningDevice, desc);
    }

    private void onProvisionSuccess(ProvisioningDevice provisioningDevice, String desc) {
        onProvisionComplete();
        postProvisioningEvent(ProvisioningEvent.EVENT_TYPE_PROVISION_SUCCESS, provisioningDevice, desc);
    }

    private void onProvisionComplete() {

        isActionStarted = false;
        idle(false);
    }

    @Override
    public void onProvisionStateChanged(int state, String desc) {
        switch (state) {
            case ProvisioningController.STATE_COMPLETE:
                ProvisioningDevice device = mProvisioningController.getProvisioningDevice();
                this.mNetworkingController.addDeviceKey(device.getUnicastAddress(), device.getDeviceKey());
                this.meshConfiguration.deviceKeyMap.put(device.getUnicastAddress(), device.getDeviceKey());
                onProvisionSuccess(device, desc);
                TelinkLog.d(TAG + "provision success");
                break;

            case ProvisioningController.STATE_FAILED:
                TelinkLog.d(TAG + "provision failed, " + desc);
                device = mProvisioningController.getProvisioningDevice();
                onProvisionFailed(device, desc);
                break;

            case ProvisioningController.STATE_CAPABILITY:
                TelinkLog.d(TAG + "on device capability received");
                break;
        }
    }


    @Override
    public void onCommandPrepared(byte type, byte[] data) {
        if (mGattController != null) {
            mGattController.sendData(type, data);
        }

    }

    @Override
    public void onNetworkInfoUpdate(int sequenceNumber, int ivIndex) {
        this.meshConfiguration.sequenceNumber = sequenceNumber;
        this.meshConfiguration.ivIndex = ivIndex;
        handleNetworkInfoUpdate(sequenceNumber, ivIndex);
    }

    public void handleNetworkInfoUpdate(int sequenceNumber, int ivIndex) {
        TelinkLog.d(TAG + "handleNetworkInfoUpdate : " + sequenceNumber + " -- " + ivIndex);

        NetworkInfoUpdateEvent networkInfoUpdateEvent = new NetworkInfoUpdateEvent(this,
                NetworkInfoUpdateEvent.EVENT_TYPE_NETWORKD_INFO_UPDATE,
                sequenceNumber, ivIndex);
        onEventPrepared(networkInfoUpdateEvent);
    }

    @Override
    public void onMeshMessageReceived(int src, int dst, int opcode, byte[] params) {
        TelinkLog.d(TAG + String.format("mesh model message received: src -- 0x%04X | dst -- 0x%04X | opcode -- 0x%04X | params -- ", src, dst, opcode)
                + Arrays.bytesToHexString(params, "") + " | action mode -- " + actionMode);
        onInnerMessageHandle(src, dst, opcode, params);

        NotificationMessage notificationMessage = new NotificationMessage(src, dst, opcode, params);
        if (actionMode == Mode.MODE_BIND) {
            mBindingController.onMessageNotification(notificationMessage);
        } else if (actionMode == Mode.MODE_MESH_OTA) {
            mMeshUpdatingController.onMessageNotification(notificationMessage);
        } else if (actionMode == Mode.MODE_REMOTE_PROVISION) {
            mRemoteProvisioningController.onMessageNotification(notificationMessage);
        }
        this.onMeshMessageNotification(notificationMessage);
    }


    @Override
    public void onProxyInitComplete(boolean success, int address) {

        if (success) {
            this.directDeviceAddress = address;
            onProxyLoginSuccess();
        } else {
            TelinkLog.w(TAG + "proxy init fail!");
            mGattController.disconnect();
        }
    }

    @Override
    public void onHeartbeatMessageReceived(int src, int dst, byte[] data) {

    }

    @Override
    public void onReliableMessageComplete(boolean success, int opcode, int rspMax, int rspCount) {
        if (actionMode == Mode.MODE_MESH_OTA) {
            mMeshUpdatingController.onUpdatingCommandComplete(success, opcode, rspMax, rspCount);
        } else if (actionMode == Mode.MODE_REMOTE_PROVISION) {
            mRemoteProvisioningController.onRemoteProvisioningCommandComplete(success, opcode, rspMax, rspCount);
        }
    }

    private void onInnerMessageHandle(int src, int dst, int opcode, byte[] params) {

        if (opcode == Opcode.NODE_RESET_STATUS.value) {
            /*
             * handle node reset message, update mesh configuration info
             */
            this.meshConfiguration.deviceKeyMap.delete(src);
            if (this.actionMode == Mode.MODE_AUTO_CONNECT) {
                validateAutoConnectTargets();
            }
        }
    }

    private void onMeshMessageNotification(NotificationMessage notificationMessage) {
        String eventType;
        StatusMessage statusMessage = notificationMessage.getStatusMessage();

        // if message can be parsed
        if (statusMessage == null) {
            eventType = StatusNotificationEvent.EVENT_TYPE_NOTIFICATION_MESSAGE_UNKNOWN;
        } else {
            eventType = statusMessage.getClass().getName();
        }

        TelinkLog.d(TAG + "mesh message notification: " + eventType);
        StatusNotificationEvent statusNotificationEvent = new StatusNotificationEvent(this,
                eventType,
                notificationMessage);
        onEventPrepared(statusNotificationEvent);
    }


    @Override
    public boolean onAccessMessagePrepared(MeshMessage meshMessage, int mode) {
        return this.sendMeshMessage(meshMessage);
    }

    /********************************************************
     * binding event
     ********************************************************/
    private void onBindingSuccess(String desc) {
        BindingDevice bindingDevice = mBindingController.getBindingDevice();
        BindingEvent bindingEvent = new BindingEvent(this, BindingEvent.EVENT_TYPE_BIND_SUCCESS, bindingDevice, desc);
        onEventPrepared(bindingEvent);
    }

    private void onBindingFail(String desc) {
        BindingDevice bindingDevice = (BindingDevice) mActionParams.get(Parameters.ACTION_BINDING_TARGET);
        BindingEvent bindingEvent = new BindingEvent(this, BindingEvent.EVENT_TYPE_BIND_FAIL, bindingDevice, desc);
        onEventPrepared(bindingEvent);
    }

    private void onBindingComplete() {
        isActionStarted = false;
        idle(false);
    }

    private void onMeshUpdatingEvent(String eventType, MeshUpdatingDevice device, String desc, int progress) {
        MeshUpdatingEvent updatingEvent = new MeshUpdatingEvent(this, eventType);
        updatingEvent.setUpdatingDevice(device);
        updatingEvent.setDesc(desc);
        updatingEvent.setProgress(progress);
        onEventPrepared(updatingEvent);
    }

    private void onMeshUpdatingComplete() {
        isActionStarted = false;
        idle(false);
    }

    private void onRemoteProvisioningComplete(String eventType, RemoteProvisioningDevice device, String desc) {
        this.actionMode = Mode.MODE_IDLE;
        RemoteProvisioningEvent event = new RemoteProvisioningEvent(this, eventType);
        event.setRemoteProvisioningDevice(device);
        event.setDesc(desc);
        onEventPrepared(event);
    }

    /**
     * access state changed
     */
    @Override
    public void onAccessStateChanged(int state, String desc, int mode, Object obj) {
        TelinkLog.d(TAG + "access state changed: " + state + " -- " + desc);
        if (actionMode == Mode.MODE_BIND && mode == AccessBridge.MODE_BINDING) {
            if (state == BindingController.STATE_FAIL) {
                onBindingComplete();
                onBindingFail(desc);
            } else if (state == BindingController.STATE_SUCCESS) {
                onBindingComplete();
                onBindingSuccess(desc);
            }
        } else if (actionMode == Mode.MODE_MESH_OTA && mode == AccessBridge.MODE_FIRMWARE_UPDATING) {
            switch (state) {
                case MeshUpdatingController.STATE_SUCCESS:
                    // at least one device success
                    onMeshUpdatingComplete();
                    onMeshUpdatingEvent(MeshUpdatingEvent.EVENT_TYPE_UPDATING_SUCCESS, null, desc, -1);
                    break;
                case MeshUpdatingController.STATE_FAIL:
                    onMeshUpdatingComplete();
                    onMeshUpdatingEvent(MeshUpdatingEvent.EVENT_TYPE_UPDATING_FAIL, null, desc, -1);
                    break;

                case MeshUpdatingController.STATE_DEVICE_FAIL:
                    onMeshUpdatingEvent(MeshUpdatingEvent.EVENT_TYPE_DEVICE_FAIL, (MeshUpdatingDevice) obj, desc, -1);
                    break;

                case MeshUpdatingController.STATE_DEVICE_SUCCESS:
                    onMeshUpdatingEvent(MeshUpdatingEvent.EVENT_TYPE_DEVICE_SUCCESS, (MeshUpdatingDevice) obj, desc, -1);
                    break;

                case MeshUpdatingController.STATE_PROGRESS:
                    int progress = (int) obj;
                    onMeshUpdatingEvent(MeshUpdatingEvent.EVENT_TYPE_UPDATING_PROGRESS, null, desc, progress);
                    break;

                case MeshUpdatingController.STATE_STOPPED:
                    onMeshUpdatingComplete();
                    onMeshUpdatingEvent(MeshUpdatingEvent.EVENT_TYPE_UPDATING_STOPPED, null, desc, -1);
                    break;
            }
        } else if (actionMode == Mode.MODE_REMOTE_PROVISION && mode == AccessBridge.MODE_REMOTE_PROVISIONING) {
            if (state == RemoteProvisioningController.STATE_PROVISION_FAIL) {
                onRemoteProvisioningComplete(RemoteProvisioningEvent.EVENT_TYPE_REMOTE_PROVISIONING_FAIL, (RemoteProvisioningDevice) obj, "remote provisioning fail");
            } else if (state == RemoteProvisioningController.STATE_PROVISION_SUCCESS) {
                RemoteProvisioningDevice device = (RemoteProvisioningDevice) obj;
                this.mNetworkingController.addDeviceKey(device.getUnicastAddress(), device.getDeviceKey());
                this.meshConfiguration.deviceKeyMap.put(device.getUnicastAddress(), device.getDeviceKey());
                onRemoteProvisioningComplete(RemoteProvisioningEvent.EVENT_TYPE_REMOTE_PROVISIONING_SUCCESS, device, "remote provisioning success");
            }
        }


    }


    enum Mode {
        /*
         * Action modes
         *
         *
         * MODE_IDLE: no scan and connect actions
         */
        MODE_IDLE,


        /**
         * scan
         */
        MODE_SCAN,

        /**
         * MODE_PROVISION: auto scan, connect, provision
         */
        MODE_PROVISION,

        /**
         * MODE_AUTO_CONNECT: auto scan, connect, get device state
         */
        MODE_AUTO_CONNECT,

        /**
         * MODE_OTA: auto scan, connect, start ota
         */
        MODE_OTA,

        /**
         * bind app key
         * connect target device without scan
         */
        MODE_BIND,

        /**
         * remote provision
         */
        MODE_REMOTE_PROVISION,


        MODE_REMOTE_BIND,

        /**
         * mesh firmware updating
         * (mesh ota)
         */
        MODE_MESH_OTA;


    }

    private Callback callback;

    public void setCallback(Callback callback) {
        this.callback = callback;
    }

    interface Callback {
        void onEventPrepared(Event<String> event);
    }
}
