/********************************************************************************************************
 * @file MeshController.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2017
 *
 * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/
package com.telink.ble.mesh.foundation;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGattService;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.ParcelUuid;
import android.text.TextUtils;
import android.util.SparseArray;

import com.telink.ble.mesh.core.Encipher;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.access.AccessBridge;
import com.telink.ble.mesh.core.access.BindingBearer;
import com.telink.ble.mesh.core.access.BindingController;
import com.telink.ble.mesh.core.access.FastProvisioningController;
import com.telink.ble.mesh.core.access.RemoteProvisioningController;
import com.telink.ble.mesh.core.access.fu.DistributorType;
import com.telink.ble.mesh.core.access.fu.FUController;
import com.telink.ble.mesh.core.access.fu.FUState;
import com.telink.ble.mesh.core.ble.BleScanner;
import com.telink.ble.mesh.core.ble.GattConnection;
import com.telink.ble.mesh.core.ble.GattOtaController;
import com.telink.ble.mesh.core.ble.GattRequest;
import com.telink.ble.mesh.core.ble.LeScanFilter;
import com.telink.ble.mesh.core.ble.LeScanSetting;
import com.telink.ble.mesh.core.ble.MeshScanRecord;
import com.telink.ble.mesh.core.ble.UUIDInfo;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.MessageResponder;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.StatusMessage;
import com.telink.ble.mesh.core.message.config.ConfigStatus;
import com.telink.ble.mesh.core.message.config.NodeIdentity;
import com.telink.ble.mesh.core.message.config.NodeIdentitySetMessage;
import com.telink.ble.mesh.core.message.config.NodeIdentityStatusMessage;
import com.telink.ble.mesh.core.networking.ExtendBearerMode;
import com.telink.ble.mesh.core.networking.NetworkingBridge;
import com.telink.ble.mesh.core.networking.NetworkingController;
import com.telink.ble.mesh.core.networking.beacon.SecureNetworkBeacon;
import com.telink.ble.mesh.core.provisioning.ProvisioningBridge;
import com.telink.ble.mesh.core.provisioning.ProvisioningController;
import com.telink.ble.mesh.core.proxy.ProxyPDU;
import com.telink.ble.mesh.entity.AdvertisingDevice;
import com.telink.ble.mesh.entity.BindingDevice;
import com.telink.ble.mesh.entity.ConnectionFilter;
import com.telink.ble.mesh.entity.FastProvisioningConfiguration;
import com.telink.ble.mesh.entity.FastProvisioningDevice;
import com.telink.ble.mesh.entity.FirmwareUpdateConfiguration;
import com.telink.ble.mesh.entity.MeshAdvFilter;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.entity.ProvisioningDevice;
import com.telink.ble.mesh.entity.RemoteProvisioningDevice;
import com.telink.ble.mesh.foundation.event.AutoConnectEvent;
import com.telink.ble.mesh.foundation.event.BindingEvent;
import com.telink.ble.mesh.foundation.event.BluetoothEvent;
import com.telink.ble.mesh.foundation.event.FastProvisioningEvent;
import com.telink.ble.mesh.foundation.event.GattConnectionEvent;
import com.telink.ble.mesh.foundation.event.GattNotificationEvent;
import com.telink.ble.mesh.foundation.event.GattOtaEvent;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.event.NetworkInfoUpdateEvent;
import com.telink.ble.mesh.foundation.event.OnlineStatusEvent;
import com.telink.ble.mesh.foundation.event.ProvisioningEvent;
import com.telink.ble.mesh.foundation.event.ReliableMessageProcessEvent;
import com.telink.ble.mesh.foundation.event.RemoteProvisioningEvent;
import com.telink.ble.mesh.foundation.event.ScanEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.foundation.parameter.AutoConnectParameters;
import com.telink.ble.mesh.foundation.parameter.BindingParameters;
import com.telink.ble.mesh.foundation.parameter.FastProvisioningParameters;
import com.telink.ble.mesh.foundation.parameter.GattConnectionParameters;
import com.telink.ble.mesh.foundation.parameter.GattOtaParameters;
import com.telink.ble.mesh.foundation.parameter.MeshOtaParameters;
import com.telink.ble.mesh.foundation.parameter.Parameters;
import com.telink.ble.mesh.foundation.parameter.ProvisioningParameters;
import com.telink.ble.mesh.foundation.parameter.ScanParameters;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.ContextUtil;
import com.telink.ble.mesh.util.MeshLogger;

import java.nio.ByteBuffer;
import java.util.Iterator;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;
import java.util.UUID;

/**
 * Created by kee on 2018/12/11.
 */

public final class MeshController implements ProvisioningBridge, NetworkingBridge, AccessBridge {

    /**
     * log tag
     */
    private static final String LOG_TAG = "MeshController";

    /**
     * proxy node advertising networkId
     */
    private static final int PROXY_ADV_TYPE_NETWORK_ID = 0x00;

    /**
     * proxy node advertising nodeIdentity
     */
    private static final int PROXY_ADV_TYPE_NODE_IDENTITY = 0x01;

    /**
     * proxy node advertising private network id
     */
    private static final int PROXY_ADV_TYPE_PRIVATE_NETWORK_ID = 0x02;

    /**
     * proxy node advertising private node identity
     */
    private static final int PROXY_ADV_TYPE_PRIVATE_NODE_IDENTITY = 0x03;


    /**
     * period of scanning for node identity when binding
     */
    private static final long BINDING_NODE_IDENTITY_SCAN_PERIOD = 8 * 1000;
    /**
     * scanning controller for bluetooth device scan
     */
    private BleScanner mBleScanner;

    /**
     * gatt controller for gatt connection
     */
    private GattConnection mGattConnection;

//    private Handler mMainThreadHandler = new Handler();

    private Context mContext;

    private HandlerThread handlerThread;

    private Handler mDelayHandler;


    /*
     * controllers
     */
    private ProvisioningController mProvisioningController;

    // mesh network
    private NetworkingController mNetworkingController;

    // key binding
    private BindingController mBindingController;

    // mesh ota
//    private MeshUpdatingController mMeshUpdatingController;

    // firmware update
//    private FirmwareUpdatingController mFirmwareUpdatingController;
    private FUController fuController;

    /**
     * rely on ProvisioningController
     */
    private RemoteProvisioningController mRemoteProvisioningController;

    /**
     * fast-provision
     * telink private protocol
     */
    private FastProvisioningController mFastProvisioningController;

    /**
     * gatt OTA
     */
    private GattOtaController mGattOtaController;

    /**
     * current active action
     */
    private Mode actionMode = Mode.IDLE;

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

    /**
     * flag of bluetooth scanning
     */
    private boolean isScanning = false;

    /**
     * cache of bluetooth scanning records
     */
    private Set<AdvertisingDevice> advDevices = new LinkedHashSet<>();

    /**
     * current mesh configs
     */
    private MeshConfiguration meshConfiguration;

    /**
     * networkId, networkIdentityKey, networkBeaconKey and privateBeaconKey are calculated by networkKey in {@link #meshConfiguration}
     */
    private byte[] networkId = null;

    private byte[] networkIdentityKey = null;

    private byte[] networkBeaconKey = null;

    private byte[] privateBeaconKey = null;

    private boolean isLogin = false;

    /**
     * action started flag when provisioning, binding, ota,  mesh-updating
     */
    private boolean isActionStarted = false;

    /**
     * max connection retry count if gatt connect failed
     */
    private static final int MAX_CONNECT_RETRY = 3;

    /**
     * current connection retry index
     */
    private int connectRetry = 0;

    /**
     * event callback
     */
    private EventCallback eventCallback;

    /**
     * direct connected device unicast address
     */
    private int directDeviceAddress = 0;

    /**
     * OTA reconnect
     * used when ota target device's advertising identification Type is NetworkID,
     * set to node identity when first connect mesh network, and reconnect by node identity
     */
    private boolean isProxyReconnect = false;

    /**
     * reconnect target proxy node by node identity
     */
    private static final long TARGET_PROXY_CONNECT_TIMEOUT = 60 * 1000;

    /**
     * rest when receive NodeIdentityStatus message
     */
    private long lastNodeSetTimestamp = 0;

    /**
     * binding target valued when device scanned
     */
    private BluetoothDevice reconnectTarget;

    /**
     * for binding on flex bearer
     */
    private long bindingStartTick = 0;

    /**
     * In this method, the following steps are performed:
     * 1. Create and start A new HandlerThread named "Mesh-Controller".
     * 2. Create handler based on HandlerThread .
     * 3. Various controllers are initialized using the handlerThread:
     * a. ScanningController is initialized.
     * b. GattConnection is initialized.
     * c. GattOtaController is initialized.
     * d. ProvisioningController is initialized.
     * e. NetworkingController is initialized.
     * f. AccessController is initialized.
     * 5. An IntentFilter is created and configured to listen for BluetoothAdapter state changes.
     * 6. The mBluetoothReceiver is registered as a broadcast receiver to receive the BluetoothAdapter state change broadcasts.
     * <p>
     *
     * @param context
     */
    void start(Context context) {

        handlerThread = new HandlerThread("Mesh-Controller");
        handlerThread.start();
        mDelayHandler = new Handler(handlerThread.getLooper());

        mContext = context.getApplicationContext();
        initScanningController(handlerThread);
        initGattConnection(handlerThread);
        initGattOtaController();
        initProvisioningController(handlerThread);
        initNetworkingController(handlerThread);
        initAccessController(handlerThread);

        IntentFilter filter = new IntentFilter();
        filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        mContext.registerReceiver(mBluetoothReceiver, filter);
    }


    /**
     * Used to listen for changes in the Bluetooth state of the device.
     * <p>
     * The onReceive() method is called when the BroadcastReceiver receives a broadcast intent. It checks the action of the received intent and if it matches the BluetoothAdapter.ACTION_STATE_CHANGED action, it retrieves the current state of the Bluetooth adapter from the intent and calls the onBluetoothStateUpdate() method with the state as a parameter.
     */
    private BroadcastReceiver mBluetoothReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action == null) return;
            if (BluetoothAdapter.ACTION_STATE_CHANGED.equals(action)) {
                int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, 0);
                onBluetoothStateUpdate(state);
            }
        }
    };


    /**
     * This method is used to stop the current operation and reset all the variables and controllers. It performs the following actions:
     * <p>
     * 1. Sets the action mode to IDLE.
     * 2. Resets the direct device address to 0.
     * 3. Sets the login status to false.
     * 4. Calls the stopScan() method to stop scanning for devices.
     * 5. Clears the list of advertised devices.
     * 6. Disconnects the GATT connection if it is not null and sets the connection callback to null.
     * 7. Clears the provisioning controller if it is not null.
     * 8. Clears the networking controller if it is not null and resets the direct address.
     * 9. Clears the binding controller if it is not null.
     * 10. Clears the firmware updating controller if it is not null. (This part is commented out in the code)
     * 11. Removes all callbacks and messages from the delay handler if it is not null.
     * 12. Quits the handler thread safely if it is not null.
     * 13. Unregisters the Bluetooth receiver from the context.
     * <p>
     */
    void stop() {
        this.actionMode = Mode.IDLE;
        this.directDeviceAddress = 0;
        this.isLogin = false;
        stopScan();
        if (advDevices != null) {
            advDevices.clear();
        }

        if (mGattConnection != null) {
            mGattConnection.disconnect();
            mGattConnection.setConnectionCallback(null);
            mGattConnection = null;
        }

        if (mProvisioningController != null) {
            mProvisioningController.clear();
            mProvisioningController = null;
        }

        if (mNetworkingController != null) {
            mNetworkingController.clear();
            mNetworkingController.resetDirectAddress();
            mNetworkingController = null;
        }

        if (mBindingController != null) {
            mBindingController.clear();
            mBindingController = null;
        }

        /*if (mFirmwareUpdatingController != null) {
            mFirmwareUpdatingController.clear();
            mFirmwareUpdatingController = null;
        }*/

        if (mDelayHandler != null) {
            mDelayHandler.removeCallbacksAndMessages(null);
            mDelayHandler = null;
        }

        if (handlerThread != null) {
            handlerThread.quitSafely();
            handlerThread = null;
        }

        mContext.unregisterReceiver(mBluetoothReceiver);
    }

    /**
     * The returned value represents the mode in which the program is currently operating.
     */
    Mode getMode() {
        return actionMode;
    }

    /**
     * Check the state of the Bluetooth adapter.
     */
    void checkBluetoothState() {
        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        onBluetoothStateUpdate(adapter.getState());
    }

    /**
     * It handles different Bluetooth states and performs necessary actions accordingly.
     *
     * @param state The updated Bluetooth state.
     */
    private void onBluetoothStateUpdate(int state) {
        String stateInfo;
        switch (state) {
            case BluetoothAdapter.STATE_OFF:
                stateInfo = ("bluetooth disabled");
                if (mGattConnection != null) {
                    mGattConnection.disconnect();
                }
                break;
            case BluetoothAdapter.STATE_ON:
                stateInfo = ("bluetooth enabled");
                break;
            case BluetoothAdapter.STATE_TURNING_OFF:
                stateInfo = ("bluetooth turning off");
                break;
            case BluetoothAdapter.STATE_TURNING_ON:
                stateInfo = ("bluetooth turning on");
                break;
            default:
                stateInfo = "unknown";
                break;
        }
        onBluetoothEvent(state, stateInfo);
    }

    /**
     * This function sets up a mesh network using the provided configuration.
     * It initializes various keys required for network communication and sets up the networking controller with the provided configuration.
     *
     * @param configuration config
     */
    void setupMeshNetwork(MeshConfiguration configuration) {
        this.meshConfiguration = configuration;
        this.networkId = Encipher.k3(configuration.networkKey);
        this.networkIdentityKey = Encipher.generateIdentityKey(configuration.networkKey);
        this.networkBeaconKey = Encipher.generateBeaconKey(configuration.networkKey);
        this.privateBeaconKey = Encipher.generatePrivateBeaconKey(configuration.networkKey);
        mNetworkingController.setup(configuration);
//        mNetworkingController.proxyFilterInit(); //  for test
    }

    /**
     * This method removes a device from the mesh network configuration. First, it removes the device key associated with the given mesh address from the device key map in the mesh configuration. Then, it calls the networking controller to remove the device key as well.
     * <p>
     * If the action mode is set to AUTO_CONNECT, it then validates the auto connect targets to ensure they are still valid after the removal of the device.
     * <p>
     * Overall, this method is responsible for removing a device from the mesh network and updating the necessary components accordingly.
     *
     * @param meshAddress target mesh address of the device
     */
    void removeDevice(int meshAddress) {
        this.meshConfiguration.deviceKeyMap.remove(meshAddress);
        mNetworkingController.removeDeviceKey(meshAddress);
        if (this.actionMode == Mode.AUTO_CONNECT) {
            validateAutoConnectTargets();
        }
    }

    /**
     * Initializes the scanning controller by creating a new instance of the BleScanner class and setting the scanner type to DEFAULT. It takes a HandlerThread as a parameter, which is used for handling the scanning operations in a separate thread.
     * The scanner callback is set to the current instance of the class implementing the ScannerCallback interface.
     */
    private void initScanningController(HandlerThread handlerThread) {
        mBleScanner = new BleScanner(BleScanner.ScannerType.DEFAULT, handlerThread);
        mBleScanner.setScannerCallback(this.scanCallback);
    }

    /**
     * Initializes the GattConnection object and sets the connection callback.
     *
     * @param handlerThread The handler thread to be used for GattConnection.
     */
    private void initGattConnection(HandlerThread handlerThread) {
        mGattConnection = new GattConnection(mContext, handlerThread);
        mGattConnection.setConnectionCallback(connectionCallback);
    }

    /**
     * Initializes the GattOtaController by creating a new instance and setting its callback.
     * The GattOtaController is responsible for handling OTA (Over-The-Air) firmware updates over GATT (Generic Attribute Profile).
     * It requires a GattConnection object to establish the connection with the remote device.
     * The GattOtaController's callback is used to receive updates and notifications during the OTA process.
     */
    private void initGattOtaController() {
        mGattOtaController = new GattOtaController(mGattConnection);
        mGattOtaController.setCallback(gattOtaCallback);
    }

    /**
     * This method initializes the provisioning controller by creating a new instance of the ProvisioningController class and setting it as the provisioning bridge.
     * The provisioning controller is responsible for managing the provisioning process.
     *
     * @param handlerThread The HandlerThread to be used by the NetworkingController.
     */
    private void initProvisioningController(HandlerThread handlerThread) {
        mProvisioningController = new ProvisioningController(handlerThread);
        mProvisioningController.setProvisioningBridge(this);
    }

    /**
     * Initializes the NetworkingController with the specified HandlerThread.
     * The NetworkingController is responsible for handling network-related operations.
     *
     * @param handlerThread The HandlerThread to be used by the NetworkingController.
     */
    private void initNetworkingController(HandlerThread handlerThread) {
        mNetworkingController = new NetworkingController(handlerThread);
        mNetworkingController.setNetworkingBridge(this);
    }

    /**
     * Initializes various controllers by creating instances of them and registering them with a handler thread.
     *
     * @param handlerThread The HandlerThread to be used by the NetworkingController.
     */
    private void initAccessController(HandlerThread handlerThread) {
        mBindingController = new BindingController(handlerThread);
        mBindingController.register(this);

        fuController = new FUController(handlerThread);
        fuController.register(this);
//        mFirmwareUpdatingController = new FirmwareUpdatingController(handlerThread);
//        mFirmwareUpdatingController.register(this);

        mRemoteProvisioningController = new RemoteProvisioningController(handlerThread);
        mRemoteProvisioningController.register(this);

        mFastProvisioningController = new FastProvisioningController(handlerThread);
        mFastProvisioningController.register(this);
    }

    /**
     * Returns a boolean value indicating whether the provisioner is logged in as a proxy.
     *
     * @return true if the provisioner is logged in as a proxy, false otherwise.
     */
    boolean isProxyLogin() {
        return isLogin;
    }

    /**
     * Returns the direct connected address of a node.
     */
    int getDirectNodeAddress() {
        return directDeviceAddress;
    }

    /**
     * This method is used to put the service in an idle state.
     * It takes a boolean parameter "disconnect" which indicates whether the device should be disconnected or not.
     * It stops the ongoing scan by calling the "stopScan()" method.
     *
     * @param disconnect true if need to disconnect the currenct gatt connection, false otherwise
     */
    public void idle(boolean disconnect) {
        log("---idle--- " + disconnect);
        mDelayHandler.removeCallbacksAndMessages(null);
        validateActionMode(Mode.IDLE);
        if (disconnect) {
            mGattConnection.disconnect();
        }
        stopScan();
    }

    /**
     * Checks if Bluetooth is enabled on the device.
     *
     * @return true if Bluetooth is enabled, false otherwise.
     */
    public boolean isBluetoothEnabled() {
        return BluetoothAdapter.getDefaultAdapter().isEnabled();
    }

    /**
     * This method enables Bluetooth on the device
     * by using the BluetoothAdapter class and calling the enable() method.
     */
    public void enableBluetooth() {
        BluetoothAdapter.getDefaultAdapter().enable();
    }

    /**
     * This method retrieves the MAC address of the current device.
     * If the Gatt connection is established, it returns the MAC address
     * obtained from the Gatt connection. Otherwise, it returns null.
     *
     * @return The MAC address of the current device, or null if not connected.
     */
    public String getCurDeviceMac() {
        if (mGattConnection.isConnected()) {
            return mGattConnection.getMacAddress();
        }
        return null;
    }

    /**
     * Stops the scanning process.
     * Sets the "isScanning" flag to false and calls the stopScan() method of the mBleScanner object.
     */
    public void stopScan() {
        isScanning = false;
        mBleScanner.stopScan();
    }

    /**
     * This code snippet connects to a Bluetooth device by stopping any ongoing scans
     * and initiating a GATT connection.
     *
     * @param device target BluetoothDevice
     */
    public void connect(final BluetoothDevice device) {
        stopScan();
        mGattConnection.connect(device);
    }

    /**
     * Disconnects the Gatt connection.
     */
    public void disconnect() {
        mGattConnection.disconnect();
    }

    /**
     * This method starts a scan for Bluetooth devices using the provided scan parameters.
     * 1.  checks if the action mode is valid for scanning. If not, it returns without performing any further actions.
     * 2.  removes any pending callbacks and messages from the delay handler.
     * 3.  sets the action mode to scan and clears the list of advertisement devices.
     * 4.  assigns the scan parameters to the action parameters and resets the action.
     * If the GATT connection is not successfully disconnected, it starts the scan. Otherwise, it logs a message indicating that it is waiting for the disconnect.
     *
     * @param scanParameters
     */
    public void startScan(ScanParameters scanParameters) {
        if (!validateActionMode(Mode.SCAN)) return;
        mDelayHandler.removeCallbacksAndMessages(null);
        this.actionMode = Mode.SCAN;
        advDevices.clear();
        this.mActionParams = scanParameters;
        resetAction();
        if (!mGattConnection.disconnect()) {
            startScan();
        } else {
            log("waiting for disconnect...");
        }
    }

    /**
     * Starts the BLE scan with the specified parameters.
     */
    private void startScan() {
        log("start scan: " + actionMode);
        LeScanFilter filter = (LeScanFilter) mActionParams.get(Parameters.SCAN_FILTERS);
        long timeout = mActionParams.getLong(Parameters.COMMON_SCAN_TIMEOUT, 10 * 1000);
        long spacing = mActionParams.getLong(Parameters.COMMON_SCAN_MIN_SPACING, 500);
        LeScanSetting scanSetting = new LeScanSetting(spacing, timeout);
        mBleScanner.startScan(filter, scanSetting);
    }


    /**
     * Used to initiate the provisioning process.
     * rebuild the provisioning device with the params in {@link MeshConfiguration};
     * <p>
     * If the GATT is connected, destroy the connection
     *
     * @param parameters
     * @return returns if the provisioning process is successfully initiated.
     */
    boolean startProvisioning(ProvisioningParameters parameters) {
        log("start provision");
        if (!validateActionMode(Mode.PROVISION)) return false;
        mDelayHandler.removeCallbacksAndMessages(null);
        this.actionMode = Mode.PROVISION;
        this.mProvisioningController.setProvisioningBridge(this);
        this.mActionParams = parameters;
        resetAction();
        ProvisioningDevice provisioningDevice = (ProvisioningDevice) parameters.get(Parameters.ACTION_PROVISIONING_TARGET);
        rebuildProvisioningDevice(provisioningDevice);
        if (mGattConnection.disconnect()) {
            this.isDisconnectWaiting = true;
        } else {
            connect(provisioningDevice.getBluetoothDevice());
        }
        return true;
    }


    boolean continueProvision(int address) {
        if (actionMode != Mode.PROVISION) {
            return false;
        }
        this.mProvisioningController.continueProvision(address);
        return true;
    }

    boolean continueRemoteProvision(int address) {
        if (actionMode != Mode.REMOTE_PROVISION) {
            return false;
        }
        this.mRemoteProvisioningController.continueProvision(address);
        return true;
    }

    /**
     * Used to rebuild the provisioning device by setting various parameters.
     * <p>
     * 1. set the IV Index of the provisioning device to the value stored in the meshConfiguration object.
     * 2. set the Network Key and Network Key Index of the provisioning device to the values stored in the meshConfiguration object.
     * 3. set the IV Update Flag and Key Refresh Flag of the provisioning device to 0.
     *
     * @param provisioningDevice target
     */
    private void rebuildProvisioningDevice(ProvisioningDevice provisioningDevice) {
        provisioningDevice.setIvIndex(this.meshConfiguration.ivIndex);
        provisioningDevice.setNetworkKey(this.meshConfiguration.networkKey);
        provisioningDevice.setNetworkKeyIndex(this.meshConfiguration.netKeyIndex);
        provisioningDevice.setIvUpdateFlag((byte) 0);
        provisioningDevice.setKeyRefreshFlag((byte) 0);
    }

    /**
     * This method starts the process of binding application key to node.
     * <p>
     * If the GATT connection is already established with the proxy node and the binding target is either the direct device address or the binding bearer is set to Any or Flex, it calls the onConnectSuccess() method.
     * <p>
     * Otherwise, it sets the binding start tick to the current system time and checks if disconnecting from the GATT connection is successful. If so, it sets the isDisconnectWaiting flag to true. If not, it starts scanning for devices.
     */
    void startBinding(BindingParameters bindingParameters) {
        if (!validateActionMode(Mode.MODE_BIND)) {
            log("binding currently");
            return;
        }
        mDelayHandler.removeCallbacksAndMessages(null);
        BindingDevice bindingDevice = (BindingDevice) bindingParameters.get(Parameters.ACTION_BINDING_TARGET);

        if (!MeshUtils.validUnicastAddress(bindingDevice.getMeshAddress())) {
            log("start bind fail: node address invalid", MeshLogger.LEVEL_WARN);
            onBindingFail("node address invalid");
            return;
        }

        this.mActionParams = bindingParameters;
        this.actionMode = Mode.MODE_BIND;
        this.reconnectTarget = null;
        resetAction();

        // if mac address
        int bindingTarget = bindingDevice.getMeshAddress();
        BindingBearer bindingBearer = bindingDevice.getBearer();
        if (mGattConnection.isProxyNodeConnected() && (directDeviceAddress == bindingTarget || bindingBearer == BindingBearer.Any || bindingBearer == BindingBearer.Flex)) {
            onConnectSuccess();
        } else {
            this.bindingStartTick = System.currentTimeMillis();
            if (mGattConnection.disconnect()) {
                // waiting for callback
                isDisconnectWaiting = true;
            } else {
                startScan();
            }
        }
    }

    /**
     * Starts the remote provisioning process for a given remote provisioning device.
     * If the proxy node is not connected, it calls the onRemoteProvisioningComplete method with a fail event type and an error message stating that the proxy node is not connected.
     * If the proxy node is connected, it sets the action mode to remote provisioning, rebuilds the provisioning device, and calls the begin method on the remote provisioning controller with the provisioning controller and the remote provisioning device as parameters.
     *
     * @param remoteProvisioningDevice target device
     */
    void startRemoteProvision(RemoteProvisioningDevice remoteProvisioningDevice) {
        if (!validateActionMode(Mode.REMOTE_PROVISION)) {
            log("remote provisioning currently");
            return;
        }
        mDelayHandler.removeCallbacksAndMessages(null);
        if (!mGattConnection.isProxyNodeConnected()) {
            onRemoteProvisioningComplete(RemoteProvisioningEvent.EVENT_TYPE_REMOTE_PROVISIONING_FAIL,
                    remoteProvisioningDevice, "proxy node not connected");
            return;
        }
        this.actionMode = Mode.REMOTE_PROVISION;
        rebuildProvisioningDevice(remoteProvisioningDevice);
        mRemoteProvisioningController.begin(this.mProvisioningController, remoteProvisioningDevice);
    }

    /**
     * Starts the fast provisioning process with the given parameters.
     * If the proxy node is already connected, it calls the onConnectSuccess() method.
     * Otherwise, it starts scanning for nearby devices.
     */
    void startFastProvision(FastProvisioningParameters parameters) {
        if (!validateActionMode(Mode.FAST_PROVISION)) {
            log("fast provisioning currently");
            return;
        }
        this.actionMode = Mode.FAST_PROVISION;
        mDelayHandler.removeCallbacksAndMessages(null);
        this.mActionParams = parameters;
        FastProvisioningConfiguration fastProvisioningConfiguration = (FastProvisioningConfiguration) parameters.get(Parameters.ACTION_FAST_PROVISION_CONFIG);
        mFastProvisioningController.init(fastProvisioningConfiguration, meshConfiguration);
        if (mGattConnection.isProxyNodeConnected()) {
            onConnectSuccess();
        } else {
            startScan();
        }
    }

    /**
     * This method is used to automatically connect to a device based on the provided parameters.
     */
    void autoConnect(AutoConnectParameters parameters) {
        if (!validateActionMode(Mode.AUTO_CONNECT)) {
            log("auto connect currently");
            return;
        }
        mDelayHandler.removeCallbacksAndMessages(null);
        this.mActionParams = parameters;
        log("auto connect");
        this.actionMode = Mode.AUTO_CONNECT;
        if (!validateAutoConnectTargets()) {
            return;
        }
        if (mGattConnection.isProxyNodeConnected()) {
            if (isLogin) {
                onAutoConnectSuccess();
            } else {
                proxyFilterInit();
            }
        } else {
            if (mGattConnection.disconnect()) {
                // waiting for disconnected
                isDisconnectWaiting = true;
            } else {
                startScan();
            }
        }
    }

    /**
     * Starts the GATT OTA (Over-the-Air) process with the given parameters.
     * <p>
     * It retrieves the connection filter from the OTA parameters and checks if the GATT connection is valid based on the filter.
     * If it is valid, it calls the onConnectSuccess() method. If not, it starts the safety scan.
     *
     * @param otaParameters
     */
    void startGattOta(GattOtaParameters otaParameters) {
        if (!validateActionMode(Mode.GATT_OTA)) {
            log("ota running currently");
            return;
        }
        mDelayHandler.removeCallbacksAndMessages(null);
        this.mActionParams = otaParameters;
        this.actionMode = Mode.GATT_OTA;
        this.isProxyReconnect = false;
        this.lastNodeSetTimestamp = 0;
        this.reconnectTarget = null;
        resetAction();
        ConnectionFilter filter = (ConnectionFilter) otaParameters.get(Parameters.ACTION_CONNECTION_FILTER);
        // && directDeviceAddress == meshAddress
        if (validateGattConnection(filter)) {
            onConnectSuccess();
        } else {
            startSafetyScan();
        }
    }

    /**
     * Starts a GATT connection with the specified GattConnectionParameters.
     * <p>
     * The method then resets the action and gets the connection filter from the connection parameters. If the GATT connection is valid based on the connection filter, the onConnectSuccess method is called. Otherwise, it starts a safety scan.
     *
     * @param connectionParameters params
     */
    void startGattConnection(GattConnectionParameters connectionParameters) {
        if (!validateActionMode(Mode.GATT_CONNECTION)) {
            log("gatt connection running currently");
        }

        mDelayHandler.removeCallbacksAndMessages(null);
        this.mActionParams = connectionParameters;
        this.actionMode = Mode.GATT_CONNECTION;
        this.isProxyReconnect = false;
        this.lastNodeSetTimestamp = 0;
        this.reconnectTarget = null;
        resetAction();
        ConnectionFilter filter = (ConnectionFilter) connectionParameters.get(Parameters.ACTION_CONNECTION_FILTER);
        if (validateGattConnection(filter)) {
            onConnectSuccess();
        } else {
            startSafetyScan();
        }
    }

    /**
     * Used to validate the GATT connection based on the provided connection filter.
     * <p>
     * The method first checks if the proxy node is connected. If it is not, then the method returns false.
     * <p>
     * If the connection filter type is "device name", the method compares the target device name with the connected device name obtained from the GATT connection. It returns true if both names are not empty and are equal.
     * <p>
     * If the connection filter type is "MAC address", the method compares the target MAC address with the connected device's MAC address obtained from the GATT connection. It returns true if both MAC addresses are not empty and are equal (ignoring case).
     * <p>
     * If the connection filter type is "mesh address", the method always returns true.
     * <p>
     * If none of the above conditions are met, the method returns false.
     * <p>
     * Please let me know if you need any further clarification.
     *
     * @param filter target filter
     * @return validate result
     */
    private boolean validateGattConnection(ConnectionFilter filter) {
        if (!mGattConnection.isProxyNodeConnected()) {
            return false;
        }

        if (filter.type == ConnectionFilter.TYPE_DEVICE_NAME) {
            String name = (String) filter.target;
            String connectName = mGattConnection.getDeviceName();
            return !TextUtils.isEmpty(connectName)
                    && !TextUtils.isEmpty(name)
                    && connectName.equals(name);
        } else if (filter.type == ConnectionFilter.TYPE_MAC_ADDRESS) {
            String mac = (String) filter.target;
            String connectMac = mGattConnection.getMacAddress();
            return !TextUtils.isEmpty(connectMac)
                    && !TextUtils.isEmpty(mac)
                    && connectMac.equalsIgnoreCase(mac);
        } else if (filter.type == ConnectionFilter.TYPE_MESH_ADDRESS) {
            return true;
        }
        return false;
    }

    /**
     * Sends a GattRequest to the GattConnection.
     *
     * @param request the GattRequest to be sent
     * @return true if the request was successfully sent, false otherwise
     */
    boolean sendGattRequest(GattRequest request) {
        return mGattConnection.sendRequest(request);
    }

    /**
     * Retrieves the Maximum Transmission Unit (MTU) value of the current GATT connection.
     * The MTU represents the maximum size of data packets that can be transmitted over the connection.
     *
     * @return The MTU value of the GATT connection.
     */
    int getMtu() {
        return mGattConnection.getMtu();
    }

    /**
     * start or continue meshOTA (firmware update),
     */
    void startMeshOTA(MeshOtaParameters meshOtaParameters) {
        if (!validateActionMode(Mode.MESH_OTA)) {
            log("mesh updating running currently");
            return;
        }
        mDelayHandler.removeCallbacksAndMessages(null);
        this.mActionParams = meshOtaParameters;
        this.actionMode = Mode.MESH_OTA;
        this.isProxyReconnect = false;
        this.lastNodeSetTimestamp = 0;
        resetAction();
        if (mGattConnection.isProxyNodeConnected()) {
            onConnectSuccess();
        } else {
            startSafetyScan();
        }
    }


    /**
     * Used to stop the Mesh OTA (Over-The-Air) update process.
     */
    void stopMeshOta() {
        if (actionMode != Mode.MESH_OTA) {
            log("mesh updating stop: not running...");
            return;
        }
        log("stop mesh ota");
        mNetworkingController.clear();
        fuController.stop();
//        mFirmwareUpdatingController.stop();
    }

    /**
     * This method is used to initiate a safety scan.
     * It first checks if the GATT connection is successfully disconnected using the  `disconnect()`  method.
     * If the disconnection is successful, it sets a flag  `isDisconnectWaiting`  to true, indicating that a callback is expected.
     * If the disconnection fails, it proceeds to start the scan by calling the  `startScan()`  method.
     */
    private void startSafetyScan() {
        if (mGattConnection.disconnect()) {
            // waiting for callback
            isDisconnectWaiting = true;
        } else {
            startScan();
        }
    }

    /**
     * Validates the specified action mode against the current action mode.
     *
     * @param targetMode The target action mode to validate.
     * @return True if the action mode is valid and has been updated, false otherwise.
     */
    private boolean validateActionMode(Mode targetMode) {
        if (actionMode == targetMode) {
            return false;
        } else {
            if (actionMode == Mode.REMOTE_PROVISION) {
                mRemoteProvisioningController.clear();
            } else if (actionMode == Mode.PROVISION) {
                mProvisioningController.clear();
            } else if (actionMode == Mode.MODE_BIND) {
                mBindingController.clear();
            } else if (actionMode == Mode.MESH_OTA) {
                fuController.clear();
//                mFirmwareUpdatingController.clear();
            } else if (actionMode == Mode.FAST_PROVISION) {
                mFastProvisioningController.clear();
                isLogin = false;
                setupMeshNetwork(this.meshConfiguration);
            }
            actionMode = targetMode;
            return true;
        }
    }

    /**
     * This method starts the GATT OTA (Over-the-Air) process.
     * It retrieves the OTA firmware data from the action parameters.
     * If the firmware data is not found, it calls the onOtaComplete method with a failure message.
     * Otherwise, it calls the onActionStart method and begins the GATT OTA process using the firmware data.
     */
    private void startGattOta() {
        byte[] otaFirmware = (byte[]) mActionParams.get(Parameters.ACTION_OTA_FIRMWARE);
        if (otaFirmware == null) {
            onOtaComplete(false, "firmware not found");
        } else {
            onActionStart();
            mGattOtaController.begin(otaFirmware);
        }
    }

    /**
     * check the device key map in MeshConfiguration
     */
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

    /**
     * This method is called when the GATT connection is complete.
     * It determines the event type based on the success of the connection,
     * either GattConnectionEvent.EVENT_TYPE_CONNECT_SUCCESS or
     * GattConnectionEvent.EVENT_TYPE_CONNECT_FAIL.
     * It then creates a new GattConnectionEvent with the appropriate event type and description.
     * Finally, it calls the onEventPrepared method to handle the prepared event.
     *
     * @param success true if the connection was successful, false otherwise
     * @param desc    the description of the connection status
     */
    public void onGattConnectionComplete(boolean success, String desc) {
        resetAction();
        this.idle(false);
        String evenType = success ?
                GattConnectionEvent.EVENT_TYPE_CONNECT_SUCCESS : GattConnectionEvent.EVENT_TYPE_CONNECT_FAIL;
        GattConnectionEvent event = new GattConnectionEvent(this, evenType, desc);
        onEventPrepared(event);
    }

    /**
     * Generates a Bluetooth event with the given state and description, and triggers the onEventPrepared method.
     *
     * @param state The state of the Bluetooth event.
     * @param desc  The description of the Bluetooth event.
     */
    private void onBluetoothEvent(int state, String desc) {
        BluetoothEvent event = new BluetoothEvent(this, BluetoothEvent.EVENT_TYPE_BLUETOOTH_STATE_CHANGE);
        event.setState(state);
        event.setDesc(desc);
        onEventPrepared(event);
    }

    /**
     * Handle a mesh event.
     * Creates a new MeshEvent object with the given eventType and desc parameters,
     * and then calls the onEventPrepared method passing the meshEvent as a parameter.
     *
     * @param eventType event type
     * @param desc      description
     */
    private void onMeshEvent(String eventType, String desc) {
        MeshEvent meshEvent = new MeshEvent(this, eventType, desc);
        onEventPrepared(meshEvent);
    }

    /**
     * This method is used to handle reliable message process events.
     *
     * @param eventType event type in {@link RemoteProvisioningEvent}
     * @param success   is reliable message success
     * @param opcode    message opcode
     * @param rspMax    response max
     * @param rspCount  response count
     * @param desc      description
     */
    private void onReliableMessageProcessEvent(String eventType, boolean success, int opcode, int rspMax, int rspCount, String desc) {
        ReliableMessageProcessEvent event = new ReliableMessageProcessEvent(this, eventType, success, opcode, rspMax, rspCount, desc);
        onEventPrepared(event);
    }

    /**
     * notify the event callback when an event has been prepared.
     *
     * @param event event
     */
    private void onEventPrepared(Event event) {
        if (eventCallback != null) {
            eventCallback.onEventPrepared(event);
        }
    }

    /**
     * Used to indicate that an action has started.
     */
    private void onActionStart() {
        isActionStarted = true;
    }

    /**
     * Reset the action status and connection retry counter.
     */
    private void resetAction() {
        isActionStarted = false;
        connectRetry = 0;
    }

    /**
     * connected and filter set success
     */
    private void onAutoConnectSuccess() {
        mGattConnection.enableOnlineStatus();
        onAutoConnectEvent(AutoConnectEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN);
    }

    /**
     * This method is called when an auto connect event occurs.
     * It creates an AutoConnectEvent object with the given event type and direct device address.
     * Then, it calls the onEventPrepared method passing the autoConnectEvent object.
     */
    private void onAutoConnectEvent(String eventType) {
        AutoConnectEvent autoConnectEvent = new AutoConnectEvent(this, eventType, this.directDeviceAddress);
        onEventPrepared(autoConnectEvent);
    }

    /**
     * This method retrieves the online status of the mesh network.
     * It checks if the user is logged in and if the GATT connection is enabled for online status.
     * Otherwise, it returns false.
     *
     * @return true if send gatt command success, otherwise false
     */
    public boolean getOnlineStatus() {
        return isLogin && mGattConnection.getOnlineStatus();
    }

    /**
     * Resets the extend bearer mode to the specified value.
     *
     * @param extendBearerMode the extend bearer mode to be set
     */
    public void resetExtendBearerMode(ExtendBearerMode extendBearerMode) {
        if (mNetworkingController != null) {
            mNetworkingController.setExtendBearerMode(extendBearerMode);
        }
    }


    /**
     * send mesh message
     * include config message and common message
     */
    public boolean sendMeshMessage(MeshMessage meshMessage) {
        if (!isLogin) {
            log("not login when sending message");
            return false;
        }
        log("send mesh message: " + meshMessage.getClass().getSimpleName()
                + String.format(" opcode: 0x%04X -- dst: 0x%04X", meshMessage.getOpcode(), meshMessage.getDestinationAddress())
                + " isReliable: " + meshMessage.isReliable()
                + " retryCnt: " + meshMessage.getRetryCnt()
                + " rspMax: " + meshMessage.getResponseMax());
        final boolean sent = mNetworkingController.sendMeshMessage(meshMessage);
        if (meshMessage.isReliable()) {
            if (sent) {
                // sent
                onReliableMessageProcessEvent(ReliableMessageProcessEvent.EVENT_TYPE_MSG_PROCESSING,
                        false,
                        meshMessage.getOpcode(),
                        meshMessage.getResponseMax(),
                        0,
                        "mesh message processing");
            } else {
                // busy
                onReliableMessageProcessEvent(ReliableMessageProcessEvent.EVENT_TYPE_MSG_PROCESS_ERROR,
                        false,
                        meshMessage.getOpcode(),
                        meshMessage.getResponseMax(),
                        0,
                        "mesh message send fail");
            }
        }
        return sent;
    }

    /**
     * initialize the proxy filter.
     */
    private void proxyFilterInit() {
        log("filter init start");
        mNetworkingController.proxyFilterInit();
    }

    /**
     * receive online status notification
     * post online_status event
     *
     * @param data decrypted notification data
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
            byte proxyPduType = (byte) (completePacket[0] & ProxyPDU.MASK_TYPE);

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
                        byte beaconType = payloadData[0];
                        if (beaconType == SecureNetworkBeacon.BEACON_TYPE_SECURE_NETWORK) {
                            mNetworkingController.parseSecureBeacon(payloadData, this.networkBeaconKey);
                        } else if (beaconType == SecureNetworkBeacon.BEACON_TYPE_MESH_PRIVATE) {
                            mNetworkingController.parsePrivateBeacon(payloadData, this.privateBeaconKey);
                        }
                    }
                    break;
            }
        }
    }

    /**
     * This method is called when an unexpected notification is received from a specific service and characteristic identified by their UUIDs, along with the data received.
     * It creates a GattNotificationEvent object with the necessary information and passes it to the onEventPrepared() method.
     *
     * @param serviceUUID        service
     * @param characteristicUUID characteristic
     * @param data               notification data
     */
    private void onUnexpectedNotify(UUID serviceUUID, UUID characteristicUUID, byte[] data) {
        GattNotificationEvent event = new GattNotificationEvent(this, GattNotificationEvent.EVENT_TYPE_UNEXPECTED_NOTIFY,
                serviceUUID, characteristicUUID, data);
        onEventPrepared(event);
    }

    /**
     * start provision action
     */
    private void beginProvision() {
        ProvisioningDevice provisioningDevice = (ProvisioningDevice) mActionParams.get(Parameters.ACTION_PROVISIONING_TARGET);
        onProvisionBegin(provisioningDevice, "provision begin");
        mProvisioningController.begin(provisioningDevice);
    }


    /**
     * gatt connect success
     */
    private void onConnectSuccess() {

        if (actionMode == Mode.PROVISION) {
            onActionStart();
            beginProvision();
        } else if (actionMode == Mode.FAST_PROVISION) {
            onProxyLoginSuccess();
        } else {
            boolean isFilterInitNeeded
                    = mActionParams.getBool(Parameters.COMMON_PROXY_FILTER_INIT_NEEDED, false);

            if ((!isLogin && isFilterInitNeeded) || (directDeviceAddress == 0)) {
                proxyFilterInit();
            } else {
                // no need to init proxy filter
                onProxyLoginSuccess();
            }
        }
    }

    /**
     * This method is used to set the identity of a node in a mesh network.
     * <p>
     * call sendMeshMessage send the NodeIdentitySetMessage to the mesh network.
     *
     * @param targetAddress target node address
     */
    private void setNodeIdentity(int targetAddress) {
        log(String.format("set node identity on %04X", targetAddress));
        isProxyReconnect = false;
        NodeIdentitySetMessage message = new NodeIdentitySetMessage(targetAddress);
        message.setNetKeyIndex(meshConfiguration.netKeyIndex);
        message.setIdentity(NodeIdentity.RUNNING.code);
        sendMeshMessage(message);
    }

    /**
     * This method is called when the proxy login is successful. It sets the login status to true and performs different actions based on the action mode.
     * <p>
     * - If the action mode is AUTO_CONNECT, it calls the onAutoConnectSuccess() method.
     * - If the action mode is MODE_BIND, it delays the execution of the binding process by 500 milliseconds and then starts the binding process by calling the onActionStart() method. It retrieves the binding target device from the action parameters and begins the binding process using the specified network key index and application key index.
     * - If the action mode is GATT_OTA, it checks if there is a connection target device. If there is, it sets the node identity for that device. If not, it starts the GATT OTA process.
     * - If the action mode is GATT_CONNECTION, it checks if there is a connection target device. If there is, it sets the node identity for that device. If not, it completes the GATT connection process.
     * - If the action mode is MESH_OTA, it checks if there is a Mesh OTA target device. If there is, it sets the node identity for that device. If not, it starts the Mesh OTA process.
     * - If the action mode is FAST_PROVISION, it starts the fast provisioning process by calling the onActionStart() method and then begins the fast provisioning controller.
     */
    private void onProxyLoginSuccess() {
        isLogin = true;
        switch (actionMode) {
            case AUTO_CONNECT:
                this.onAutoConnectSuccess();
                break;

            case MODE_BIND:
                mDelayHandler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        onActionStart();
                        BindingDevice bindingDevice = (BindingDevice) mActionParams.get(Parameters.ACTION_BINDING_TARGET);
                        int netKeyIndex = bindingDevice.getNetKeyIndex();
                        if (netKeyIndex == -1) {
                            netKeyIndex = meshConfiguration.netKeyIndex;
                        }
                        mBindingController.begin(netKeyIndex,
                                meshConfiguration.appKeyMap.get(bindingDevice.getAppKeyIndex()), bindingDevice);
                    }
                }, 500);
                break;

            case GATT_OTA: {
                int address = getConnectionTarget();
                if (address != -1) {
                    setNodeIdentity(address);
                } else {
                    startGattOta();
                }
                break;
            }

            case GATT_CONNECTION: {
                int address = getConnectionTarget();
                if (address != -1) {
                    setNodeIdentity(address);
                } else {
                    onGattConnectionComplete(true, "connect success");
                }
                break;
            }

            case MESH_OTA: {
                int address = getMeshOTATarget();
                if (address != -1) {
                    setNodeIdentity(address);
                } else {
                    onActionStart();
                    FirmwareUpdateConfiguration configuration = (FirmwareUpdateConfiguration) mActionParams.get(Parameters.ACTION_MESH_OTA_CONFIG);
                    rebuildFirmwareUpdatingDevices(configuration);
                    fuController.begin(configuration, directDeviceAddress);
                }

                break;
            }


            case FAST_PROVISION:
                onActionStart();
                mFastProvisioningController.begin();
                break;

        }
    }

    /**
     * This method retrieves the connection target based on the specified connection filter.
     * If the connection filter type is not of type TYPE_MESH_ADDRESS, it returns -1.
     * Otherwise, it retrieves the target address from the filter and checks if it is different from the direct device address.
     * If it is different, it returns the target address. Otherwise, it returns -1.
     *
     * @return The connection target address, or -1 if not applicable.
     */
    private int getConnectionTarget() {
        final ConnectionFilter filter = (ConnectionFilter) mActionParams.get(Parameters.ACTION_CONNECTION_FILTER);
        if (filter.type != ConnectionFilter.TYPE_MESH_ADDRESS) {
            return -1;
        }
        int targetAdr = (int) filter.target;
        if (directDeviceAddress != targetAdr) {
            return targetAdr;
        }
        return -1;
    }

    /**
     * This method is used to retrieve the target address for a Mesh OTA (Over-The-Air) firmware update.
     * Checks if the direct device address is different from the last address.
     * If they are different, it returns the last address as the target for the Mesh OTA. Otherwise, it returns -1 to indicate that there is no target address.
     *
     * @return address
     */
    private int getMeshOTATarget() {
        FirmwareUpdateConfiguration configuration = (FirmwareUpdateConfiguration) mActionParams.get(Parameters.ACTION_MESH_OTA_CONFIG);
        int lastAddress = configuration.getProxyAddress();
        if (configuration.getDistributorType() == DistributorType.PHONE) {
            // reconnect last connected address
            log("reconnect proxy device when mesh ota - " + lastAddress);
            if (directDeviceAddress != lastAddress) {
                return lastAddress;
            }
        }
        return -1;
    }

    /**
     * move direct device to last
     * Used to rebuild the list of devices that need a firmware update based on the provided configuration.
     * It first retrieves the list of devices from the configuration object and iterates through them using an iterator.
     * It then checks if the mesh address of the current device matches the direct device address and removes it from the list if it does.
     * If a direct device is found, it is stored in a variable for later use.
     * Finally, the direct device is added back to the list of devices if it was found.
     * The method also sets the extend bearer mode based on the current networking controller.
     */
    private void rebuildFirmwareUpdatingDevices(FirmwareUpdateConfiguration configuration) {
        List<MeshUpdatingDevice> devices = configuration.getUpdatingDevices();
        Iterator<MeshUpdatingDevice> iterator = devices.iterator();
        MeshUpdatingDevice device;
        MeshUpdatingDevice directDevice = null;
        while (iterator.hasNext()) {
            device = iterator.next();
            if (device.meshAddress == directDeviceAddress) {
                directDevice = device;
                iterator.remove();
            }
        }
        if (directDevice != null) {
            devices.add(directDevice);
        }
        configuration.setExtendBearerMode(mNetworkingController.getExtendBearerMode());
    }

    /**
     * called if action started when gatt disconnected
     * This method handles the event of a connection interruption. It performs different actions based on the current action mode.
     * <p>
     * In the case of the "PROVISION" mode, it retrieves the provisioning device and calls the "onProvisionFailed" method with the device and a description of the interruption.
     * <p>
     * In the case of the "MODE_BIND" mode, it calls the "onBindingFail" method with the description of the interruption.
     * <p>
     * In the case of the "GATT_OTA" mode, it calls the "onOtaComplete" method with a flag indicating the OTA completion status (false in this case) and the description of the interruption.
     * <p>
     * In the case of the "MESH_OTA" mode, it checks if the firmware update controller is distributing by device. If so, it starts scanning for devices again. Otherwise, it dispatches the firmware update state with a flag indicating the completion status (false in this case) and the description of the interruption.
     * <p>
     * In the case of the "FAST_PROVISION" mode, it calls the "onFastProvisioningComplete" method with a flag indicating the completion status (false in this case) and a description of the interruption.
     */
    private void onConnectionInterrupt() {
        String desc = "connection interrupt";
        switch (actionMode) {
            case PROVISION:
                ProvisioningDevice device = mProvisioningController.getProvisioningDevice();
                onProvisionFailed(device, desc);
                break;
            case MODE_BIND:
                onBindingFail(desc);
                break;

            case GATT_OTA:
                onOtaComplete(false, desc);
                break;

            case MESH_OTA:
                if (fuController.isDistributingByDevice()) {
                    // device may disconnected after update applying
                    startScan();
                } else {
                    dispatchFUState(false, desc);
                }

                break;

            case FAST_PROVISION:
                onFastProvisioningComplete(false, "connection interrupt");
                break;
        }
    }

    /**
     * This method is called when a connection fails. It determines the action mode and performs specific actions based on the mode.
     * <p>
     * If the action mode is "PROVISION", it retrieves the provisioning device and calls the "onProvisionFailed" method, passing the device and a description of the failure.
     * <p>
     * If the action mode is "MODE_BIND", it calls the "onBindingFail" method, passing the description of the failure.
     * <p>
     * If the action mode is "GATT_OTA", it calls the "onOtaComplete" method with a false flag and the description of the failure.
     * <p>
     * If the action mode is "MESH_OTA", it dispatches the false flag and the description of the failure to the "dispatchFUState" method.
     * <p>
     * If the action mode is "FAST_PROVISION", it calls the "onFastProvisioningComplete" method with a false flag and a description of the failure.
     * <p>
     * If the action mode is "GATT_CONNECTION", it calls the "onGattConnectionComplete" method with a false flag and a description of the failure.
     */
    private void onConnectionFail() {
        String desc = "connect fail";
        switch (actionMode) {
            case PROVISION:
                ProvisioningDevice provisioningDevice = (ProvisioningDevice) mActionParams.get(Parameters.ACTION_PROVISIONING_TARGET);
                onProvisionFailed(provisioningDevice, "connect fail");
                break;
            case MODE_BIND:
                onBindingFail(desc);
                break;

            case GATT_OTA:
                onOtaComplete(false, desc);
                break;

            case MESH_OTA:
                dispatchFUState(false, desc);
                break;

            case FAST_PROVISION:
                onFastProvisioningComplete(false, "connect fail");
                break;

            case GATT_CONNECTION:
                onGattConnectionComplete(false, "connect fail");
                break;
        }
    }

    /**
     * This method is called when the GATT connection is disconnected.
     * The purpose of this method is to handle the disconnection event and perform necessary cleanup and actions based on the current action mode.
     */
    private void onGattDisconnected() {
        mDelayHandler.removeCallbacksAndMessages(null);
        onMeshEvent(MeshEvent.EVENT_TYPE_DISCONNECTED, "disconnected when: " + actionMode);
        mNetworkingController.clear();
        mNetworkingController.resetDirectAddress();
        if (isDisconnectWaiting) {
            isDisconnectWaiting = false;
            connectRetry = -1;
        }
        isLogin = false;
        directDeviceAddress = 0;
        if (actionMode != Mode.IDLE) {
            if (actionMode == Mode.MESH_OTA && fuController.needAutoConnect()) {
                fuController.hold();
            }
            mDelayHandler.postDelayed(DISCONNECTION_TASK, 500);
        }
    }

    /**
     * It is used to handle the disconnection of a Bluetooth device in a specific action mode.
     */
    private final Runnable DISCONNECTION_TASK = new Runnable() {
        @Override
        public void run() {

            boolean needScan = actionMode == Mode.SCAN
                    || actionMode == Mode.AUTO_CONNECT
                    || (actionMode == Mode.MESH_OTA && fuController.needAutoConnect());
            if (needScan) {
                startScan();
            } else {
                if (actionMode == Mode.REMOTE_PROVISION) {
                    // remote provision
                    RemoteProvisioningDevice device = mRemoteProvisioningController.getProvisioningDevice();
                    mRemoteProvisioningController.clear();
                    onRemoteProvisioningComplete(RemoteProvisioningEvent.EVENT_TYPE_REMOTE_PROVISIONING_FAIL, device, "connection interrupt");
                } else if (actionMode == Mode.MESH_OTA) {
                    dispatchFUState(false, "device disconnected");
                } else if (actionMode == Mode.PROVISION
                        || actionMode == Mode.MODE_BIND || actionMode == Mode.GATT_OTA
                        || actionMode == Mode.GATT_CONNECTION
                        || actionMode == Mode.FAST_PROVISION) {
                    if (isActionStarted) {
                        onConnectionInterrupt();
                    } else {
                        connectRetry++;
                        if (connectRetry >= MAX_CONNECT_RETRY) {
                            // connect retry max
                            onConnectionFail();
                        } else {
                            // reconnect when provision and re-scan when other mode
                            if (actionMode == Mode.PROVISION) {
                                ProvisioningDevice provisioningDevice = (ProvisioningDevice) mActionParams.get(Parameters.ACTION_PROVISIONING_TARGET);
                                log("provisioning connect retry: " + connectRetry);
                                connect(provisioningDevice.getBluetoothDevice());
                            } else if (actionMode == Mode.MODE_BIND
                                    || actionMode == Mode.GATT_CONNECTION
                                    || actionMode == Mode.GATT_OTA) {
                                final BluetoothDevice device = reconnectTarget;
                                if (device != null) {
                                    connect(device);
                                } else {
                                    startScan();
                                }
                            } else {
                                startScan();
                            }
                        }
                    }
                }
            }

        }
    };

    /**
     * This method is called when the OTA (Over-the-Air) update is complete.
     * It resets the action and sets the device to idle mode.
     * It then determines the event type based on the success of the OTA update,
     * and calls the onOtaEvent method to notify the listeners of the event.
     *
     * @param success - a boolean indicating whether the OTA update was successful or not
     * @param desc    - a String description of the OTA update result
     */
    private void onOtaComplete(boolean success, String desc) {
        resetAction();
        this.idle(false);
        String evenType = success ?
                GattOtaEvent.EVENT_TYPE_OTA_SUCCESS : GattOtaEvent.EVENT_TYPE_OTA_FAIL;
        onOtaEvent(evenType, 0, desc);
    }

    /**
     * This method is called to update the progress of the OTA (Over-The-Air) update.
     *
     * @param progress The current progress of the OTA update.
     */
    private void onOtaProgress(int progress) {
        onOtaEvent(GattOtaEvent.EVENT_TYPE_OTA_PROGRESS, progress, "ota progress update");
    }

    /**
     * handles the OTA (Over-The-Air) events in the application.
     *
     * @param eventType represents the type of OTA event that occurred
     * @param progress  indicates the progress of the OTA process
     * @param desc      provides a description or additional information about the OTA event
     */
    private void onOtaEvent(String eventType, int progress, String desc) {
//        int meshAddress = mActionParams.getInt(Parameters.ACTION_CONNECTION_MESH_ADDRESS, -1);
        GattOtaEvent otaEvent = new GattOtaEvent(this, eventType, progress, desc);
        onEventPrepared(otaEvent);
    }


    /**
     * This is a private field in the class that represents a callback for GATT connection events.
     * <p>
     * The  `onConnected()`  method is not implemented and is ignored.
     * <p>
     * onServicesDiscovered() method is implemented and performs some actions after discovering the Bluetooth GATT services.
     * <p>
     * onNotify() method is implemented and handles notifications received from the Bluetooth GATT service.
     */
    private GattConnection.ConnectionCallback connectionCallback = new GattConnection.ConnectionCallback() {
        @Override
        public void onConnected() { /* ignore */}

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
                    if (actionMode == Mode.PROVISION) {
                        mGattConnection.provisionInit();
                    } else {
                        mGattConnection.proxyInit();
                        mNetworkingController.checkSequenceNumber(networkId, networkBeaconKey);
                    }

                    mDelayHandler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            onConnectSuccess();
                        }
                    }, 200);
                }
            }, 500);
        }

        @Override
        public void onNotify(UUID serviceUUID, UUID charUUID, byte[] data) {
            if (charUUID.equals(UUIDInfo.CHARACTERISTIC_ONLINE_STATUS)) {
                log("online status encrypted data: " + Arrays.bytesToHexString(data, ":"));
                MeshLogger.d("online data: " + Arrays.bytesToHexString(data));
                MeshLogger.d("online key: " + Arrays.bytesToHexString(networkBeaconKey));
                byte[] decrypted = Encipher.decryptOnlineStatus(data, networkBeaconKey);
                MeshLogger.d("online dec: " + Arrays.bytesToHexString(decrypted));
                if (decrypted != null) {
                    log("online status decrypted data: " + Arrays.bytesToHexString(decrypted, ":"));
                    onOnlineStatusNotify(decrypted);
                } else {
                    log("online status decrypt err");
                }
            } else if (charUUID.equals(UUIDInfo.CHARACTERISTIC_PROXY_OUT) || charUUID.equals(UUIDInfo.CHARACTERISTIC_PB_OUT)) {
                onGattNotification(data);
            } else {
                onUnexpectedNotify(serviceUUID, charUUID, data);
            }
        }
    };

    /**
     * handle the OTA state changed during OTA processing
     */
    private GattOtaController.GattOtaCallback gattOtaCallback = new GattOtaController.GattOtaCallback() {

        @Override
        public void onOtaStateChanged(int state) {
            switch (state) {
                case GattOtaController.OTA_STATE_FAILURE:
                    onOtaComplete(false, "gatt command fail");
                    break;

                case GattOtaController.OTA_STATE_SUCCESS:
                    onOtaComplete(true, "ota complete");
                    break;

                case GattOtaController.OTA_STATE_PROGRESS:
                    onOtaProgress(mGattOtaController.getOtaProgress());
                    break;
            }
        }
    };

    /**
     * This method is called when a new advertising device is found during scanning.
     * If the device is successfully added, it prepares a scan event with the device information and calls the onEventPrepared method.
     */
    private void onDeviceFound(AdvertisingDevice device) {
        log("on device found: " + device.device.getAddress());
        if (advDevices.add(device)) {
            ScanEvent scanEvent = new ScanEvent(this, ScanEvent.EVENT_TYPE_DEVICE_FOUND, device);
            onEventPrepared(scanEvent);
        } else {
            log("Device already exist");
        }
    }

    /**
     * It creates a ScanEvent object with the EVENT_TYPE_SCAN_FAIL event type and no additional data.
     * The onEventPrepared method is then called to handle the prepared event.
     */
    private void onScanFail() {
//        idle(false);
        ScanEvent scanEvent = new ScanEvent(this, ScanEvent.EVENT_TYPE_SCAN_FAIL, null);
        onEventPrepared(scanEvent);
    }

    /**
     * This method is called when the ble scanning for devices times out.
     * <p>
     * If the action mode is "SCAN", it calls the onScanTimeoutEvent method. If the action mode is "MODE_BIND", it calls the onBindingFail method with the message "device not found when scanning".
     * <p>
     * If the action mode is "AUTO_CONNECT" or "MESH_OTA", it schedules a delayed task to restart the scanning after 3 seconds.
     * <p>
     * If the action mode is "GATT_OTA", it calls the onOtaComplete method with the parameters false and "ota fail: scan timeout".
     * <p>
     * If the action mode is "GATT_CONNECTION", it calls the onGattConnectionComplete method with the parameters false and "connection fail: scan timeout".
     * <p>
     * If the action mode is "FAST_PROVISION", it calls the onFastProvisioningComplete method with the parameters false and "no unprovisioned device found".
     * <p>
     * Finally, if no device was found during scanning and the location is disabled, it calls the onLocationDisableWarning method.
     *
     * @param anyDeviceFound
     */
    private void onScanTimeout(boolean anyDeviceFound) {
        log("scanning timeout: " + actionMode);
        switch (actionMode) {
            case SCAN:
                onScanTimeoutEvent();
                break;

            case MODE_BIND:
                onBindingFail("device not found when scanning");
                break;

            case AUTO_CONNECT:
            case MESH_OTA:
                mDelayHandler.postDelayed(restartScanTask, 3 * 1000);
                break;

            case GATT_OTA:
                onOtaComplete(false, "ota fail: scan timeout");
                break;

            case GATT_CONNECTION:
                onGattConnectionComplete(false, "connection fail: scan timeout");
                break;

            case FAST_PROVISION:
                this.onFastProvisioningComplete(false, "no unprovisioned device found");
                break;
        }
        if (!anyDeviceFound && !ContextUtil.isLocationEnable(mContext)) {
            onLocationDisableWarning();
        }
    }

    /**
     * The purpose of this task is to restart a scan if the current action mode is either "AUTO_CONNECT" or "MESH_OTA".
     * In other words, if the program is in the process of automatically connecting or performing a mesh over-the-air update, the "startScan()" method will be invoked again.
     * <p>
     * This task can be used to schedule the restart of a scan in a background thread or within a specific time interval.
     */
    private final Runnable restartScanTask = new Runnable() {
        @Override
        public void run() {
            if (actionMode == Mode.AUTO_CONNECT || actionMode == Mode.MESH_OTA)
                startScan();
        }
    };


    /**
     * This method is triggered when a scan timeout event occurs.
     * Creates a new ScanEvent object with the event type set to "EVENT_TYPE_SCAN_TIMEOUT" and passes null as the additional data.
     */
    private void onScanTimeoutEvent() {
        idle(false);
        ScanEvent scanEvent = new ScanEvent(this, ScanEvent.EVENT_TYPE_SCAN_TIMEOUT, null);
        onEventPrepared(scanEvent);
    }

    /**
     * Generates a warning event when the location is disabled.
     * This method creates a ScanEvent object with the event type set to EVENT_TYPE_SCAN_LOCATION_WARNING
     * and sends it to the onEventPrepared method.
     */
    private void onLocationDisableWarning() {
        ScanEvent scanEvent = new ScanEvent(this, ScanEvent.EVENT_TYPE_SCAN_LOCATION_WARNING, null);
        onEventPrepared(scanEvent);
    }

    /**
     * validate advertising device when auto connect {@link Mode#AUTO_CONNECT}
     *
     * @param scanRecord scan record
     * @return is validate ok
     */
    private boolean validateProxyAdv(byte[] scanRecord) {
        MeshScanRecord sr = MeshScanRecord.parseFromBytes(scanRecord);
        if (sr == null) return false;
        byte[] serviceData = sr.getServiceData(ParcelUuid.fromString(UUIDInfo.SERVICE_PROXY.toString()));
        if (serviceData != null && serviceData.length >= 9) {
            int type = serviceData[0];
            if (type == PROXY_ADV_TYPE_NETWORK_ID || type == PROXY_ADV_TYPE_PRIVATE_NETWORK_ID) {
                // validate network id matches
                return validateNetworkId(scanRecord);
            } else if (type == PROXY_ADV_TYPE_NODE_IDENTITY || type == PROXY_ADV_TYPE_PRIVATE_NODE_IDENTITY) {
                boolean nodeIdentityCheck = validateNodeIdentity(serviceData);
                log("check node identity pass? " + nodeIdentityCheck);
                return nodeIdentityCheck;
            }
        }
        return false;
    }

    /**
     * This method is used to validate the network ID in a given scan record.
     * <p>
     * The method takes in a byte array called "scanRecord" as a parameter. It first attempts to parse the scan record using the MeshScanRecord class. If the scan record cannot be parsed, the method returns false.
     * <p>
     * Next, the method retrieves the service data from the scan record using the UUID "SERVICE_PROXY". If the service data is not null and its length is greater than or equal to 9, the method proceeds to validate the network ID.
     * <p>
     * If the type of the service data is "PROXY_ADV_TYPE_NETWORK_ID", the method extracts the advertising network ID from the service data and compares it with the given network ID. If they match, the method returns true.
     * <p>
     * If the type of the service data is "PROXY_ADV_TYPE_PRIVATE_NETWORK_ID", the method extracts the hash and random values from the service data. It then calculates the hash using the network ID and random values along with the network identity key. The calculated hash is compared with the extracted hash, and if they match, the method returns true.
     * <p>
     * If the type of the service data is neither "PROXY_ADV_TYPE_NETWORK_ID" nor "PROXY_ADV_TYPE_PRIVATE_NETWORK_ID", an error message is logged.
     * <p>
     * If the service data is null or its length is less than 9, the method returns false.
     * <p>
     * Overall, this method checks if the network ID in the scan record matches the given network ID and returns true if it does. Otherwise, it returns false.
     *
     * @param scanRecord given scan record
     * @return is validate ok
     */
    private boolean validateNetworkId(byte[] scanRecord) {
        MeshScanRecord sr = MeshScanRecord.parseFromBytes(scanRecord);
        if (sr == null) return false;
        byte[] serviceData = sr.getServiceData(ParcelUuid.fromString(UUIDInfo.SERVICE_PROXY.toString()));
        if (serviceData != null && serviceData.length >= 9) {
            int type = serviceData[0];
            if (type == PROXY_ADV_TYPE_NETWORK_ID) {
                // validate network id matches
                final int networkIdLen = 8;
                byte[] advertisingNetworkId = new byte[networkIdLen];
                System.arraycopy(serviceData, 1, advertisingNetworkId, 0, networkIdLen);
                boolean networkIdCheck = Arrays.equals(networkId, advertisingNetworkId);
                log("check network id pass (normal) ? " + networkIdCheck);
                return networkIdCheck;
            } else if (type == PROXY_ADV_TYPE_PRIVATE_NETWORK_ID) {
                // validate network id matches
                final int randomLen = 8;
                byte[] hash = new byte[randomLen];
                System.arraycopy(serviceData, 1, hash, 0, randomLen);

                final int hashLen = 8;
                byte[] random = new byte[hashLen];
                System.arraycopy(serviceData, 9, random, 0, hashLen);

                byte[] calcHash = Encipher.aes(ByteBuffer.allocate(16).put(networkId).put(random).array(), networkIdentityKey);
                byte[] hashRe = new byte[hashLen];
                System.arraycopy(calcHash, 8, hashRe, 0, hashLen);
                boolean networkIdCheck = Arrays.equals(hash, hashRe);
                log("check network id pass (private) ? " + networkIdCheck);
                return networkIdCheck;
            } else {
                log("check network id error: not broadcasting network id type");
            }
        }
        return false;
    }

    /**
     * This method is used to validate the identity of a node based on the provided service data.
     * <p>
     *
     * @param serviceData service data
     * @return is validate ok
     */
    private boolean validateNodeIdentity(byte[] serviceData) {
        if (this.networkIdentityKey == null || meshConfiguration.deviceKeyMap.size() == 0 || serviceData.length < 17)
            return false;

        SparseArray<byte[]> deviceKeyMap = meshConfiguration.deviceKeyMap;
        int nodeAddress;
        for (int i = 0; i < deviceKeyMap.size(); i++) {
            nodeAddress = deviceKeyMap.keyAt(i);
            if (validateTargetNodeIdentity(serviceData, nodeAddress)) {
                directDeviceAddress = nodeAddress;
                log("reset direct device address");
                return true;
            }
        }

        return false;
    }

    /**
     * This method is used to validate the identity of a target node based on the service data and node address.
     * It takes the service data and node address as input parameters.
     * <p>
     * The method first checks if the service data is not null and has a length of at least 9 bytes.
     * If the conditions are met, it proceeds with the validation process.
     * <p>
     * It extracts the type of advertisement from the service data, which is stored in the first byte.
     * If the type is either PROXY_ADV_TYPE_NODE_IDENTITY or PROXY_ADV_TYPE_PRIVATE_NODE_IDENTITY,
     * it continues with the validation process.
     * <p>
     * The method then extracts the advertisement hash and random values from the service data.
     * These values are used to calculate the hash of the node identity.
     * <p>
     * If the type is PROXY_ADV_TYPE_NODE_IDENTITY, it generates the node identity hash using the networkIdentityKey,
     * random values, and node address. Otherwise, if the type is PROXY_ADV_TYPE_PRIVATE_NODE_IDENTITY,
     * it generates the private node identity hash using the same parameters.
     * <p>
     * The method compares the calculated hash with the advertisement hash.
     * If they are equal, it sets the pass variable to true.
     * <p>
     * Finally, it logs the result of the identity validation and returns the pass variable.
     */
    private boolean validateTargetNodeIdentity(byte[] serviceData, int nodeAddress) {
        boolean pass = false;
        if (serviceData != null && serviceData.length >= 9) {
            int type = serviceData[0];
            if (type == PROXY_ADV_TYPE_NODE_IDENTITY || type == PROXY_ADV_TYPE_PRIVATE_NODE_IDENTITY) {
                final int dataLen = 8;
                int index = 1;
                byte[] advHash = new byte[dataLen];
                System.arraycopy(serviceData, index, advHash, 0, dataLen);
                index += dataLen;

                byte[] random = new byte[dataLen];
                System.arraycopy(serviceData, index, random, 0, dataLen);

                byte[] hash;
                if (type == PROXY_ADV_TYPE_NODE_IDENTITY) {
                    log("get node identity hash");
                    hash = Encipher.generateNodeIdentityHash(networkIdentityKey, random, nodeAddress);
                } else {
                    log("get private node identity hash");
                    hash = Encipher.generatePrivateNodeIdentityHash(networkIdentityKey, random, nodeAddress);
                }
                pass = Arrays.equals(advHash, hash);
            }
        }
        log("check target node identity pass? " + pass);
        return pass;
    }


    /**
     * This method is called when a Bluetooth device is found during scanning.
     * It checks if the device should be connected based on the current action mode.
     * <p>
     * If the action mode is AUTO_CONNECT, it checks if the device's advertisement data is valid for proxy connection.
     * <p>
     * If the action mode is GATT_OTA or GATT_CONNECTION, it checks the connection filter type and validates the device based on the filter target.
     * <p>
     * If the action mode is MODE_BIND, it checks if the device's advertisement data is valid for binding.
     * <p>
     * If the action mode is FAST_PROVISION, it always connects to the device.
     * <p>
     * If the action mode is SCAN, it notifies the listener about the found device.
     * <p>
     * If the action mode is MESH_OTA, it checks if the device's advertisement data is valid for mesh OTA.
     * <p>
     * If the device is eligible for connection, the scanning is stopped and the device is connected.
     *
     * @param device     device
     * @param rssi       rssi
     * @param scanRecord scan record
     */
    private void onScanFilter(final BluetoothDevice device, final int rssi, final byte[] scanRecord) {
        synchronized (SCAN_LOCK) {
            if (!isScanning) return;
            boolean connectIntent = false;
            MeshScanRecord sr = MeshScanRecord.parseFromBytes(scanRecord);
            if (actionMode == Mode.AUTO_CONNECT) {
                connectIntent = validateProxyAdv(scanRecord);
            } else if (actionMode == Mode.GATT_OTA || actionMode == Mode.GATT_CONNECTION) {
                final ConnectionFilter filter = (ConnectionFilter) mActionParams.get(Parameters.ACTION_CONNECTION_FILTER);
                if (filter == null) {
                    return;
                }
                switch (filter.type) {
                    case ConnectionFilter.TYPE_MESH_ADDRESS:
                        int nodeAddress = (int) filter.target;
                        if (filter.advFilter == MeshAdvFilter.NODE_ID_ONLY) {
                            connectIntent = validateTargetNodeIdentity(sr.getProxyServiceData(), nodeAddress);
                            break;
                        } else if (filter.advFilter == MeshAdvFilter.NETWORK_ID_ONLY) {
                            connectIntent = validateNetworkId(scanRecord);
                        } else {
                            // any
                            long during = System.currentTimeMillis() - lastNodeSetTimestamp;
                            if (isProxyReconnect && during < TARGET_PROXY_CONNECT_TIMEOUT) {
                                connectIntent = validateTargetNodeIdentity(sr.getProxyServiceData(), nodeAddress);
                            } else {
                                connectIntent = validateProxyAdv(scanRecord);
                            }
                            if (connectIntent && directDeviceAddress == nodeAddress) {
                                reconnectTarget = device;
                            }

                        }
                        break;

                    case ConnectionFilter.TYPE_MAC_ADDRESS:
                        String mac = (String) filter.target;
                        connectIntent = mac.equalsIgnoreCase(device.getAddress());
                        if (connectIntent) {
                            reconnectTarget = device;
                            MeshLogger.d("connect by mac address: " + mac);
                        }
                        break;

                    case ConnectionFilter.TYPE_DEVICE_NAME:
                        String name = (String) filter.target;
                        connectIntent = !TextUtils.isEmpty(device.getName()) && !TextUtils.isEmpty(name) && device.getName().equals(name);
                        if (connectIntent) {
                            reconnectTarget = device;
                            MeshLogger.d("connect by name: " + name);
                        }
                        break;
                }


            } else if (actionMode == Mode.MODE_BIND) {
                BindingDevice bindingDevice = (BindingDevice) mActionParams.get(Parameters.ACTION_BINDING_TARGET);

                if (bindingDevice.getBearer() == BindingBearer.GattOnly) {
                    connectIntent = validateTargetNodeIdentity(sr.getProxyServiceData(), bindingDevice.getMeshAddress());
                    log("bind check node identity pass? " + connectIntent);
                } else if (bindingDevice.getBearer() == BindingBearer.Flex) {
                    if (System.currentTimeMillis() - bindingStartTick > BINDING_NODE_IDENTITY_SCAN_PERIOD) {
                        connectIntent = validateProxyAdv(scanRecord);
                    } else {
                        connectIntent = validateTargetNodeIdentity(sr.getProxyServiceData(), bindingDevice.getMeshAddress());
                    }
                } else {
                    connectIntent = validateProxyAdv(scanRecord);
                }
                if (connectIntent && directDeviceAddress == bindingDevice.getMeshAddress()) {
                    reconnectTarget = device;
                }/*
                BindingDevice bindingDevice = (BindingDevice) mActionParams.get(Parameters.ACTION_BINDING_TARGET);
                if (bindingDevice.getBearer() == BindingBearer.GattOnly) {
                    connectIntent = validateTargetNodeIdentity(scanRecord, bindingDevice.getMeshAddress());
                    log("bind check node identity pass? " + connectIntent);
                } else {
                    connectIntent = validateProxyAdv(scanRecord);
                }*/
            } else if (actionMode == Mode.FAST_PROVISION) {
                connectIntent = true;
            } else if (actionMode == Mode.SCAN) {
                boolean single = mActionParams.getBool(Parameters.SCAN_SINGLE_MODE, false);
                if (single) {
                    stopScan();
                }
                onDeviceFound(new AdvertisingDevice(device, rssi, scanRecord));
            } else if (actionMode == Mode.MESH_OTA) {
                FirmwareUpdateConfiguration configuration = (FirmwareUpdateConfiguration) mActionParams.get(Parameters.ACTION_MESH_OTA_CONFIG);
                int lastAddress = configuration.getProxyAddress();
                if (configuration.getDistributorType() == DistributorType.PHONE && fuController.isDistributingByPhone()) {
                    // reconnect last connected address
                    log("reconnect proxy device when mesh ota - " + lastAddress);
                    long during = System.currentTimeMillis() - lastNodeSetTimestamp;
                    if (isProxyReconnect && during < TARGET_PROXY_CONNECT_TIMEOUT) {
                        connectIntent = validateTargetNodeIdentity(sr.getProxyServiceData(), lastAddress);
                    } else {
                        connectIntent = validateProxyAdv(scanRecord);
                    }

                } else {
                    connectIntent = validateProxyAdv(scanRecord);
                }
            }

            if (connectIntent) {
                isScanning = false;
                connect(device);
            }
        }
    }


    /**
     * Implementation of the BleScanner.ScannerCallback interface.
     * It provides callback methods for handling Bluetooth Low Energy (BLE) scanning events.
     * <p>
     * - onLeScan(): This method is called when a BLE device is detected during scanning. It receives the BluetoothDevice object representing the detected device, the received signal strength indicator (RSSI), and the scan record data. It logs the device's name, MAC address, and scan record in a specific format. It then invokes the onScanFilter() method to process the detected device.
     * <p>
     * - onScanFail(): This method is called when the BLE scanning fails. It sets the scanning flag to false and invokes the onScanFail() method in the MeshController class.
     * <p>
     * - onStartedScan(): This method is called when the BLE scanning starts. It sets the scanning flag to true.
     * <p>
     * - onStoppedScan(): This method is called when the BLE scanning stops.
     * <p>
     * - onScanTimeout(): This method is called when the BLE scanning times out. It receives a boolean parameter indicating whether any device was found during the scan. It then invokes the onScanTimeout() method in the MeshController class.
     */
    private BleScanner.ScannerCallback scanCallback = new BleScanner.ScannerCallback() {
        @Override
        public void onLeScan(BluetoothDevice device, int rssi, byte[] scanRecord) {
            log("scan:" + device.getName() + " --mac: " + device.getAddress() + " --record: " + Arrays.bytesToHexString(scanRecord, ":"));
//            if (!device.getAddress().contains("FF:FF:BB:CC:DD")) return;
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
        public void onScanTimeout(boolean anyDeviceFound) {
            MeshController.this.onScanTimeout(anyDeviceFound);
        }
    };

    /**
     * This method is used to post a provisioning event.
     *
     * @param eventType          event type
     * @param provisioningDevice provisioning device
     * @param desc               description
     */
    private void postProvisioningEvent(String eventType, ProvisioningDevice provisioningDevice, String desc) {

        ProvisioningEvent provisioningEvent = new ProvisioningEvent(this, eventType, provisioningDevice, desc);
        onEventPrepared(provisioningEvent);
    }

    /**
     * This method is called when the process of getting the revision for a provisioning device starts.
     *
     * @param device ProvisioningDevice
     * @param desc   description
     */
    private void onGetRevisionStart(ProvisioningDevice device, String desc) {
        postProvisioningEvent(ProvisioningEvent.EVENT_TYPE_PROVISION_GET_REVISION, device, desc);
    }

    /**
     * This method is called when the provisioning of a device begins.
     *
     * @param device ProvisioningDevice
     * @param desc   description
     */
    private void onProvisionBegin(ProvisioningDevice device, String desc) {
        postProvisioningEvent(ProvisioningEvent.EVENT_TYPE_PROVISION_BEGIN, device, desc);
    }

    /**
     * This method is called when the provisioning of a device fails.
     * It calls the onProvisionComplete method with a false parameter and posts a ProvisioningEvent with the EVENT_TYPE_PROVISION_FAIL type, the provisioning device, and the failure description.
     * <p>
     *
     * @param provisioningDevice ProvisioningDevice
     * @param desc               description
     */
    private void onProvisionFailed(ProvisioningDevice provisioningDevice, String desc) {
        log("provisioning failed: " + desc + " -- " + provisioningDevice.getUnicastAddress());
        onProvisionComplete(false);
        postProvisioningEvent(ProvisioningEvent.EVENT_TYPE_PROVISION_FAIL, provisioningDevice, desc);
    }

    /**
     * This method is called when the provisioning process is successful.
     * It updates the directDeviceAddress variable with the unicast address of the provisioned device.
     * It also triggers a postProvisioningEvent with the EVENT_TYPE_PROVISION_SUCCESS type, passing the provisioningDevice and desc as parameters.
     *
     * @param provisioningDevice ProvisioningDevice
     * @param desc               description
     */
    private void onProvisionSuccess(ProvisioningDevice provisioningDevice, String desc) {
        onProvisionComplete(true);
        this.directDeviceAddress = provisioningDevice.getUnicastAddress();
        postProvisioningEvent(ProvisioningEvent.EVENT_TYPE_PROVISION_SUCCESS, provisioningDevice, desc);
    }


    private void onProvisionCapabilityReceived(ProvisioningDevice provisioningDevice, String desc) {
        postProvisioningEvent(ProvisioningEvent.EVENT_TYPE_CAPABILITY_RECEIVED, provisioningDevice, desc);
    }

    /**
     * This method is called when the provisioning process is complete.
     * It takes a boolean parameter "success" which indicates whether the provisioning was successful or not.
     *
     * @param success is provisioning successful
     */
    private void onProvisionComplete(boolean success) {
        isActionStarted = false;
        // disconnect when provision fail
        idle(!success);
    }

    /**
     * This method is an override of the onProvisionStateChanged method from a superclass.
     * It is called when the provisioning state of a device changes.
     * <p>
     * The method takes two parameters: state, which represents the new state of the provisioning process, and desc, which provides a description or additional information about the state change.
     * <p>
     * Inside the method, there is a switch statement that handles different states of the provisioning process.
     * <p>
     * - In the case of ProvisioningController.STATE_COMPLETE, the method retrieves the provisioning device and updates the device key map with the device's unicast address and device key. It then calls the onProvisionSuccess method and logs a message indicating the success of the provisioning process.
     * <p>
     * - In the case of ProvisioningController.STATE_FAILED, the method logs a message indicating the failure of the provisioning process and retrieves the provisioning device. It then calls the onProvisionFailed method.
     * <p>
     * - In the case of ProvisioningController.STATE_CAPABILITY, the method logs a message indicating that the device's capability has been received.
     * <p>
     * Overall, this method handles different states of the provisioning process and performs appropriate actions based on the state.
     *
     * @param state provisioning state
     * @param desc  desc
     */
    @Override
    public void onProvisionStateChanged(int state, String desc) {
        switch (state) {
            case ProvisioningController.STATE_COMPLETE:
                ProvisioningDevice device = mProvisioningController.getProvisioningDevice();
                updateDeviceKeyMap(device.getUnicastAddress(), device.getDeviceKey());
                onProvisionSuccess(device, desc);
                log("provision success");
                break;

            case ProvisioningController.STATE_FAILED:
                log("provision failed, " + desc);
                device = mProvisioningController.getProvisioningDevice();
                onProvisionFailed(device, desc);
                break;

            case ProvisioningController.STATE_CAPABILITY:
                onProvisionCapabilityReceived(mProvisioningController.getProvisioningDevice(), "device capability received");
                log("on device capability received");
                break;
        }
    }

    /**
     * This method is called when a command is prepared to be sent.
     * <p>
     * Handles communication with a Bluetooth Low Energy (BLE) device using the Generic Attribute Profile (GATT) protocol. The sendMeshData() method is responsible for sending the prepared command to the BLE device.
     *
     * @param type proxy pdu typeValue {@link ProxyPDU#type}
     * @param data gatt data
     */
    @Override
    public void onCommandPrepared(byte type, byte[] data) {
        if (mGattConnection != null) {
            mGattConnection.sendMeshData(type, data);
        }

    }

    /**
     * Method to handle the update of network information: the sequence number and IV index.
     *
     * @param sequenceNumber The new sequence number.
     * @param ivIndex        The new IV index.
     */
    @Override
    public void onNetworkInfoUpdate(int sequenceNumber, int ivIndex) {
        this.meshConfiguration.sequenceNumber = sequenceNumber;
        if (actionMode != Mode.FAST_PROVISION) {
            this.meshConfiguration.ivIndex = ivIndex;
        }
        handleNetworkInfoUpdate(sequenceNumber, this.meshConfiguration.ivIndex);
    }

    /**
     * This method handles the update of network information with the given sequence number and IV index.
     * Creates a NetworkInfoUpdateEvent object with the provided sequence number and IV index.
     * Calls the onEventPrepared method to handle the prepared network info update event.
     *
     * @param sequenceNumber The sequence number of the network information update.
     * @param ivIndex        The IV index of the network information update.
     */
    public void handleNetworkInfoUpdate(int sequenceNumber, int ivIndex) {
        log(String.format("handleNetworkInfoUpdate : sequenceNumber -- %06X | ivIndex -- %08X", sequenceNumber, ivIndex));

        NetworkInfoUpdateEvent networkInfoUpdateEvent = new NetworkInfoUpdateEvent(this,
                NetworkInfoUpdateEvent.EVENT_TYPE_NETWORKD_INFO_UPDATE,
                sequenceNumber, ivIndex);
        onEventPrepared(networkInfoUpdateEvent);
    }

    /**
     * Handle mesh notification message
     * Creates a NotificationMessage object using the received parameters and calls the onMeshMessageNotification() method, passing the notificationMessage object as a parameter.
     *
     * @param src    source address
     * @param dst    destination address
     * @param opcode operation code
     * @param params parameters
     */
    @Override
    public void onMeshMessageReceived(int src, int dst, int opcode, byte[] params) {
        log(String.format("mesh model message received: src -- 0x%04X | dst -- 0x%04X | opcode -- 0x%04X | params -- ", src, dst, opcode)
                + Arrays.bytesToHexString(params, "") + " | action mode -- " + actionMode);
        NotificationMessage notificationMessage = new NotificationMessage(src, dst, opcode, params);
        this.onMeshMessageNotification(notificationMessage);
    }


    /**
     * Callback method called when the proxy initialization is complete.
     *
     * @param success true if the proxy initialization was successful, false otherwise
     * @param address the address of the direct device
     */
    @Override
    public void onProxyInitComplete(boolean success, int address) {
        log("filter init complete, success? " + success);
        if (success) {
            this.directDeviceAddress = address;
            onProxyLoginSuccess();
        } else {
            log("proxy init fail!", MeshLogger.LEVEL_WARN);
            mGattConnection.disconnect();
        }
    }

    /**
     * callback function, indicating that it is called when a heartbeat message is received.
     *
     * @param src  source address
     * @param dst  destination address
     * @param data heartbeat data
     */
    @Override
    public void onHeartbeatMessageReceived(int src, int dst, byte[] data) {

    }

    /**
     * This method is called when a reliable message is complete.
     * <p>
     * Depending on the action mode, different controllers are called to handle the completion of the message. If the action mode is MODE_BIND, the binding controller is called. If the action mode is MESH_OTA, the firmware updating controller is called. If the action mode is REMOTE_PROVISION, the remote provisioning controller is called. If the action mode is FAST_PROVISION, the fast provisioning controller is called.
     * <p>
     * If the message was not successful, the onInnerMessageFailed() method is called. Finally, a reliable message process event is triggered with the relevant information.
     * <p>
     *
     * @param success  if response received
     * @param opcode   command opcode
     * @param rspMax   expect response max
     * @param rspCount received response count
     */
    @Override
    public void onReliableMessageComplete(boolean success, int opcode, int rspMax, int rspCount) {
        if (actionMode == Mode.MODE_BIND) {
            mBindingController.onBindingCommandComplete(success, opcode, rspMax, rspCount);
        } else if (actionMode == Mode.MESH_OTA) {
            fuController.onUpdatingCommandComplete(success, opcode, rspMax, rspCount);
//            mFirmwareUpdatingController.onUpdatingCommandComplete(success, opcode, rspMax, rspCount);
        } else if (actionMode == Mode.REMOTE_PROVISION) {
            mRemoteProvisioningController.onRemoteProvisioningCommandComplete(success, opcode, rspMax, rspCount);
        } else if (actionMode == Mode.FAST_PROVISION) {
            mFastProvisioningController.onFastProvisioningCommandComplete(success, opcode, rspMax, rspCount);
        }
        if (!success) {
            onInnerMessageFailed(opcode);
        }
        onReliableMessageProcessEvent(ReliableMessageProcessEvent.EVENT_TYPE_MSG_PROCESS_COMPLETE,
                success, opcode, rspMax, rspCount, "mesh message send complete");
    }

    /**
     * It is called when a segment message is complete.
     * <p>
     * In this method, there is a check if the current action mode is MESH_OTA. If it is, then the onSegmentComplete method of the fuController object is called with the success parameter.
     * <p>
     *
     * @param success is the segment message send success
     */
    @Override
    public void onSegmentMessageComplete(boolean success) {
        if (actionMode == Mode.MESH_OTA) {
//            mFirmwareUpdatingController.onSegmentComplete(success);
            fuController.onSegmentComplete(success);
        }
    }

    /**
     * This method is called when an inner message fails to be processed.
     * It checks if the opcode of the failed message is equal to the value of the NODE_ID_SET opcode.
     * If it is, it further checks the action mode to determine the appropriate action to take.
     * If the action mode is GATT_OTA, it calls the onOtaComplete method with a failure status and a corresponding error message.
     * If the action mode is GATT_CONNECTION, it calls the onGattConnectionComplete method with a failure status and a corresponding error message.
     *
     * @param opcode operation code
     */
    public void onInnerMessageFailed(int opcode) {
        if (opcode == Opcode.NODE_ID_SET.value) {
            if (actionMode == Mode.GATT_OTA) {
                onOtaComplete(false, "node identity set failed");
            } else if (actionMode == Mode.GATT_CONNECTION) {
                onGattConnectionComplete(false, "node identity set failed");
            }
        }
    }

    // , int src, int dst, int opcode, byte[] params

    /**
     * deal mesh config related message
     */
    private void onInnerMessageHandle(NotificationMessage notificationMessage) {
        int src = notificationMessage.getSrc();
        int opcode = notificationMessage.getOpcode();
        if (opcode == Opcode.NODE_RESET_STATUS.value) {
            /*
             * handle node reset message, update mesh configuration info
             */
            this.meshConfiguration.deviceKeyMap.delete(src);
            if (this.actionMode == Mode.AUTO_CONNECT) {
                validateAutoConnectTargets();
            }
        } else if (opcode == Opcode.NODE_ID_STATUS.value) {
            NodeIdentityStatusMessage identityStatusMessage = (NodeIdentityStatusMessage) notificationMessage.getStatusMessage();
            if (this.actionMode == Mode.GATT_OTA || this.actionMode == Mode.GATT_CONNECTION) {
                ConnectionFilter filter = (ConnectionFilter) mActionParams.get(Parameters.ACTION_CONNECTION_FILTER);
                if (filter.type == ConnectionFilter.TYPE_MESH_ADDRESS) {
                    int target = (int) filter.target;
                    onNodeIdentityStatusMessageReceived(src, identityStatusMessage, target);
                }
            } else if (this.actionMode == Mode.MESH_OTA) {
                FirmwareUpdateConfiguration configuration = (FirmwareUpdateConfiguration) mActionParams.get(Parameters.ACTION_MESH_OTA_CONFIG);
                int lastAddress = configuration.getProxyAddress();
                if (configuration.getDistributorType() == DistributorType.PHONE) {
                    onNodeIdentityStatusMessageReceived(src, identityStatusMessage, lastAddress);
                }
            }
            // response of NodeIdentitySet
        }
    }


    /**
     * This method is called when a node identity status message is received from a specific source.
     * The method checks if the source matches the connection target. If it does, it retrieves the status from the identity status message and checks if it is a success. If it is, it further checks the identity of the node. If the identity is "running", it indicates that the target device needs to be reconnected. If the identity is different, it indicates a node identity check error. If the status is not a success, it indicates a node identity status error.
     * <p>
     * If the target device needs to be reconnected, a flag is set to indicate that a proxy reconnect is required and the timestamp of the last node set is updated. Then, a safety scan is started.
     * <p>
     * If the target device does not need to be reconnected, the method checks the action mode. If it is in GATT OTA mode, it calls the "onOtaComplete" method with a false flag and a description. If it is in GATT CONNECTION mode, it calls the "onGattConnectionComplete" method with a false flag and a description. If it is in MESH OTA mode, it dispatches the false flag and description to the FU state.
     * <p>
     * The purpose of this method is to handle the node identity status message received from a specific source and take appropriate actions based on the status and identity of the node.
     *
     * @param src                   source address
     * @param identityStatusMessage notification message
     * @param connectionTarget      target address
     */
    private void onNodeIdentityStatusMessageReceived(int src, NodeIdentityStatusMessage identityStatusMessage, int connectionTarget) {
        if (src == connectionTarget) {
            final int status = identityStatusMessage.getStatus();
            boolean success = false;
            String desc = "";

            if (status == ConfigStatus.SUCCESS.code) {
                final int identity = identityStatusMessage.getIdentity();
                if (identity == NodeIdentity.RUNNING.code) {
                    // reconnect target device
                    log("reconnect target device");
                    success = true;
                } else {
                    // node identity check error
                    desc = "node identity check error: " + identity;
                }
            } else {
                desc = "node identity status error: " + status;
            }

            if (success) {
                this.isProxyReconnect = true;
                this.lastNodeSetTimestamp = System.currentTimeMillis();
                startSafetyScan();
            } else {
                if (actionMode == Mode.GATT_OTA) {
                    onOtaComplete(false, desc);
                } else if (actionMode == Mode.GATT_CONNECTION) {
                    onGattConnectionComplete(false, desc);
                } else if (actionMode == Mode.MESH_OTA) {
                    dispatchFUState(false, desc);
                }
            }
        }
    }

    /**
     * This method handles incoming mesh message notifications.
     * It first calls the "onInnerMessageHandle" method to process the notification message.
     * Then, based on the current action mode, it delegates the handling of the notification to the appropriate controller.
     * <p>
     * If the action mode is set to "MODE_BIND", it calls the "onMessageNotification" method of the "mBindingController" to handle the notification.
     * If the action mode is set to "MESH_OTA", it calls the "onMessageNotification" method of the "fuController" to handle the notification.
     * If the action mode is set to "REMOTE_PROVISION", it calls the "onMessageNotification" method of the "mRemoteProvisioningController" to handle the notification.
     * If the action mode is set to "FAST_PROVISION", it calls the "onMessageNotification" method of the "mFastProvisioningController" to handle the notification.
     * <p>
     * After handling the notification, the method determines the event type based on the status message contained in the notification.
     * If the status message is null, the event type is set to "EVENT_TYPE_NOTIFICATION_MESSAGE_UNKNOWN".
     * Otherwise, the event type is set to the name of the status message class.
     * <p>
     *
     * @param notificationMessage mesh notification message
     */
    private void onMeshMessageNotification(NotificationMessage notificationMessage) {
        onInnerMessageHandle(notificationMessage);
        if (actionMode == Mode.MODE_BIND) {
            mBindingController.onMessageNotification(notificationMessage);
        } else if (actionMode == Mode.MESH_OTA) {
            fuController.onMessageNotification(notificationMessage);
//            mFirmwareUpdatingController.onMessageNotification(notificationMessage);
        } else if (actionMode == Mode.REMOTE_PROVISION) {
            mRemoteProvisioningController.onMessageNotification(notificationMessage);
        } else if (actionMode == Mode.FAST_PROVISION) {
            mFastProvisioningController.onMessageNotification(notificationMessage);
        }

        String eventType;
        StatusMessage statusMessage = notificationMessage.getStatusMessage();

        // if message can be parsed
        if (statusMessage == null) {
            eventType = StatusNotificationEvent.EVENT_TYPE_NOTIFICATION_MESSAGE_UNKNOWN;
        } else {
            eventType = statusMessage.getClass().getName();
        }

        log("mesh message notification: " + eventType);
        StatusNotificationEvent statusNotificationEvent = new StatusNotificationEvent(this,
                eventType,
                notificationMessage);
        onEventPrepared(statusNotificationEvent);
    }


    /**
     * This method is used to handle prepared mesh messages before they are sent.
     * <p>
     * The method first checks if the destination address of the mesh message is the local message address.
     * If it is, it further checks if the message is reliable.
     * If the message is reliable, it calls the  `responseMessage`  method from the  `MessageResponder`  class to generate a notification message.
     * It then calls the  `onMeshMessageNotification`  method to handle the notification message.
     * <p>
     * If the destination address is not the local message address or the message is not reliable, it returns true to indicate that the message should be skipped.
     * <p>
     * If none of the above conditions are met, it calls the  `sendMeshMessage`  method to send the mesh message.
     *
     * @param meshMessage MeshMessage
     * @param mode        mode
     * @return is message send success
     */
    @Override
    public boolean onAccessMessagePrepared(MeshMessage meshMessage, int mode) {
        if (meshMessage.getDestinationAddress() == MeshUtils.LOCAL_MESSAGE_ADDRESS) {
            if (meshMessage.isReliable()) {
                NotificationMessage notificationMessage = MessageResponder.responseMessage(meshMessage);
                onMeshMessageNotification(notificationMessage);
            }
            return true;

            // skip unreliable local message
        }
        return this.sendMeshMessage(meshMessage);
    }

    /********************************************************
     * binding event
     ********************************************************/

    /**
     * Method called when the binding process is successful.
     *
     * @param desc description
     */
    private void onBindingSuccess(String desc) {
        onBindingComplete();
        BindingDevice bindingDevice = mBindingController.getBindingDevice();
        BindingEvent bindingEvent = new BindingEvent(this, BindingEvent.EVENT_TYPE_BIND_SUCCESS, bindingDevice, desc);
        onEventPrepared(bindingEvent);
    }

    /**
     * Method called when the binding process is fail.
     *
     * @param desc description
     */
    private void onBindingFail(String desc) {
        onBindingComplete();
        BindingDevice bindingDevice = (BindingDevice) mActionParams.get(Parameters.ACTION_BINDING_TARGET);
        BindingEvent bindingEvent = new BindingEvent(this, BindingEvent.EVENT_TYPE_BIND_FAIL, bindingDevice, desc);
        onEventPrepared(bindingEvent);
    }

    /**
     * This method is called when the binding process is complete.
     * It clears the networking controller, sets the isActionStarted flag to false,
     * and calls the idle method with a parameter of false to indicate that the app is no longer idle.
     */
    private void onBindingComplete() {
        mNetworkingController.clear();
        isActionStarted = false;
        idle(false);
    }

    /**
     * This method is used to dispatch the state of a firmware update operation.
     *
     * @param success is success
     * @param msg     message
     */
    private void dispatchFUState(boolean success, String msg) {
        if (!success) {
            fuController.dispatchError(msg);
        }
        isActionStarted = false;
        idle(false);
    }

    /**
     * This method is called when remote provisioning is complete.
     * The event is then passed to the onEventPrepared method.
     *
     * @param eventType event type
     * @param device    remote provisioning device
     * @param desc      description
     */
    private void onRemoteProvisioningComplete(String eventType, RemoteProvisioningDevice device, String desc) {
        idle(false);
        RemoteProvisioningEvent event = new RemoteProvisioningEvent(this, eventType);
        event.setRemoteProvisioningDevice(device);
        event.setDesc(desc);
        onEventPrepared(event);
    }


    private void onRemoteCapabilityReceived(RemoteProvisioningDevice device, String desc) {
        RemoteProvisioningEvent event = new RemoteProvisioningEvent(this, RemoteProvisioningEvent.EVENT_TYPE_REMOTE_CAPABILITY_RECEIVED);
        event.setRemoteProvisioningDevice(device);
        event.setDesc(desc);
        onEventPrepared(event);
    }

    /**
     * This method updates the device key map with the given address and device key.
     * It adds the device key to the networking controller and also to the mesh configuration's device key map.
     *
     * @param address   node address
     * @param deviceKey device key
     */
    private void updateDeviceKeyMap(int address, byte[] deviceKey) {
        this.mNetworkingController.addDeviceKey(address, deviceKey);
        this.meshConfiguration.deviceKeyMap.put(address, deviceKey);
    }

    /**
     * This method is used to handle the state changes
     *
     * @param state new state
     * @param desc  desc
     * @param mode  access mode
     * @param obj   object
     */
    @Override
    public void onAccessStateChanged(int state, String desc, int mode, Object obj) {
        log("access state changed: " + state + " -- " + desc);
        if (actionMode == Mode.MODE_BIND && mode == AccessBridge.MODE_BINDING) {
            if (state == BindingController.STATE_FAIL) {
                onBindingFail(desc);
            } else if (state == BindingController.STATE_SUCCESS) {
                onBindingSuccess(desc);
            }
        } else if (actionMode == Mode.MESH_OTA && mode == AccessBridge.MODE_FIRMWARE_UPDATING) {
            if (state == FUState.UPDATE_COMPLETE.value || state == FUState.UPDATE_FAIL.value) {
                isActionStarted = false;
                idle(false);
            }
        } else if (actionMode == Mode.REMOTE_PROVISION && mode == AccessBridge.MODE_REMOTE_PROVISIONING) {
            if (state == RemoteProvisioningController.STATE_PROVISION_FAIL) {
                onRemoteProvisioningComplete(RemoteProvisioningEvent.EVENT_TYPE_REMOTE_PROVISIONING_FAIL, (RemoteProvisioningDevice) obj, "remote provisioning fail");
            } else if (state == RemoteProvisioningController.STATE_PROVISION_SUCCESS) {
                RemoteProvisioningDevice device = (RemoteProvisioningDevice) obj;
                updateDeviceKeyMap(device.getUnicastAddress(), device.getDeviceKey());
                onRemoteProvisioningComplete(RemoteProvisioningEvent.EVENT_TYPE_REMOTE_PROVISIONING_SUCCESS, device, "remote provisioning success");
            } else if (state == RemoteProvisioningController.STATE_CAPABILITY_RECEIVED) {
                RemoteProvisioningDevice device = (RemoteProvisioningDevice) obj;
                onRemoteCapabilityReceived(device, "remote provisioning success");
            }
        } else if (actionMode == Mode.FAST_PROVISION && mode == AccessBridge.MODE_FAST_PROVISION) {
            if (state == FastProvisioningController.STATE_RESET_NETWORK) {
                switchNetworking(false);
            } else if (state == FastProvisioningController.STATE_SUCCESS
                    || state == FastProvisioningController.STATE_FAIL) {
                onFastProvisioningComplete(state == FastProvisioningController.STATE_SUCCESS, desc);
            } else if (state == FastProvisioningController.STATE_SET_ADDR_SUCCESS) {
                FastProvisioningDevice device = (FastProvisioningDevice) obj;

                /*
                 * add device key into map
                 */
                updateDeviceKeyMap(device.getNewAddress(), device.getDeviceKey());


                /**
                 * post event
                 */
                String eventType = FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_ADDRESS_SET;
                FastProvisioningEvent event = new FastProvisioningEvent(this, eventType);
                event.setFastProvisioningDevice(device);
                onEventPrepared(event);
            }
        }
    }

    /**
     * This method is called when fast provisioning is complete.
     *
     * @param success true if fast provisioning was successful, false otherwise
     * @param desc    a description of the fast provisioning result
     */
    private void onFastProvisioningComplete(boolean success, String desc) {
        switchNetworking(true);
        this.isLogin = false;
        idle(false);
        resetAction();
        String eventType = success ? FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_SUCCESS :
                FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_FAIL;
        FastProvisioningEvent event = new FastProvisioningEvent(this, eventType);
        event.setDesc(desc);
        onEventPrepared(event);
    }

    /**
     *
     */

    /**
     * This method is used to switch the networking configuration based on the provision completion status.
     * If the switch is complete (pvComplete = true), it sets up the original mesh configuration.
     * If the switch is not complete (pvComplete = false), it sets up the fast provisioning configuration.
     *
     * @param pvComplete if fast provision action complete
     *                   true: change networking config back
     *                   false: change networking config to default
     */
    public void switchNetworking(boolean pvComplete) {
        log("switch networking: " + pvComplete);
        if (pvComplete) {
            log("setup config back: " + this.meshConfiguration.ivIndex);
            mNetworkingController.setup(this.meshConfiguration);
        } else {
            FastProvisioningConfiguration configuration = mFastProvisioningController.getConfiguration();
            MeshConfiguration meshConfiguration = new MeshConfiguration();
            meshConfiguration.sequenceNumber = this.meshConfiguration.sequenceNumber;
            meshConfiguration.ivIndex = configuration.getIvIndex();
            meshConfiguration.deviceKeyMap = new SparseArray<>();
            meshConfiguration.netKeyIndex = configuration.getDefaultNetworkKeyIndex();
            meshConfiguration.networkKey = configuration.getDefaultNetworkKey();

            meshConfiguration.appKeyMap = new SparseArray<>();
            meshConfiguration.appKeyMap.put(
                    configuration.getDefaultAppKeyIndex(), configuration.getDefaultAppKey()
            );
            meshConfiguration.localAddress = this.meshConfiguration.localAddress;
            log("setup config fast: " + meshConfiguration.ivIndex);
            mNetworkingController.setup(meshConfiguration);
        }
    }

    /**
     * Sets the event callback for this object.
     *
     * @param callback The event callback to be set.
     */
    public void setEventCallback(EventCallback callback) {
        this.eventCallback = callback;
    }

    /**
     * Event callback
     *
     * @see Event
     */
    interface EventCallback {
        /**
         * This method is called when an event has been prepared for execution.
         *
         * @param event the event that has been prepared
         */
        void onEventPrepared(Event<String> event);
    }


    /**
     * This is an enumeration that represents different modes of operation for a device. Each mode is defined as a constant value and has a corresponding name.
     * <p>
     * - IDLE: Represents the idle state where no scan and connect actions are performed.
     * - SCAN: Represents the scanning mode where the device scans for nearby devices.
     * - PROVISION: Represents the mode where the device automatically scans, connects, and provisions a device.
     * - AUTO_CONNECT: Represents the mode where the device automatically scans, connects, and retrieves the state of a device.
     * - GATT_OTA: Represents the mode where the device automatically scans, connects, and starts over-the-air (OTA) firmware updates.
     * - MODE_BIND: Represents the mode where the device binds an application key to a target device without scanning.
     * - REMOTE_PROVISION: Represents the mode for remote provisioning of a device.
     * - REMOTE_BIND: Represents the mode for remote binding of a device.
     * - FAST_PROVISION: Represents the mode for fast provisioning, a private feature specific to the Telink platform.
     * - MESH_OTA: Represents the mode for mesh firmware updating, specifically for mesh OTA updates.
     * - GATT_CONNECTION: Represents the mode for establishing a GATT connection with a target device.
     * <p>
     * Please let me know if you need any further clarification.
     */
    public enum Mode {

        /*
         * no scan and connect actions
         */
        IDLE,


        /**
         * scan
         */
        SCAN,

        /**
         * MODE_PROVISION: auto scan, connect, provision
         */
        PROVISION,

        /**
         * MODE_AUTO_CONNECT: auto scan, connect, get device state
         */
        AUTO_CONNECT,

        /**
         * MODE_OTA: auto scan, connect, start ota
         */
        GATT_OTA,

        /**
         * bind app key
         * connect target device without scan
         */
        MODE_BIND,

        /**
         * remote provision
         */
        REMOTE_PROVISION,

        /**
         * remote bind
         */
        REMOTE_BIND,

        /**
         * fast provision (telink private)
         */
        FAST_PROVISION,

        /**
         * mesh firmware updating
         * (mesh ota)
         */
        MESH_OTA,

        /**
         * gatt connection for target device
         */
        GATT_CONNECTION;
    }


    /**
     * This method is used to log a message with the default debug level.
     *
     * @param logMessage log message
     */
    private void log(String logMessage) {
        log(logMessage, MeshLogger.LEVEL_DEBUG);
    }

    /**
     * This method is used to log a message with a specified log level.
     *
     * @param logMessage log message
     * @param level      log level
     */
    private void log(String logMessage, int level) {
        MeshLogger.log(logMessage, LOG_TAG, level);
    }

}
