package com.telink.ble.mesh.core.ble;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.content.Context;
import android.os.Build;
import android.os.Handler;
import android.os.HandlerThread;

import com.telink.ble.mesh.core.proxy.ProxyPDU;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.LogInfo;
import com.telink.ble.mesh.util.OtaPacketParser;
import com.telink.ble.mesh.util.TelinkLog;

import java.lang.reflect.Method;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

public class GattController extends BluetoothGattCallback {

    private final String TAG = "GATT -- ";

    public static final int OTA_STATE_SUCCESS = 1;
    public static final int OTA_STATE_FAILURE = 0;
    public static final int OTA_STATE_PROGRESS = 2;


    private static final int CONN_STATE_IDLE = 1;
    private static final int CONN_STATE_CONNECTING = 2;
    private static final int CONN_STATE_CONNECTED = 4;
    private static final int CONN_STATE_DISCONNECTING = 8;
    private static final int CONN_STATE_CLOSED = 16;


    private static final int RSSI_UPDATE_TIME_INTERVAL = 2000;

    private static final int CONNECTION_TIMEOUT = 20 * 1000;

    protected final Queue<CommandContext> mInputCommandQueue = new ConcurrentLinkedQueue<>();

    protected final Queue<CommandContext> mOutputCommandQueue = new ConcurrentLinkedQueue<>();
    protected final Map<String, CommandContext> mNotificationCallbacks = new ConcurrentHashMap<>();

    protected Handler mTimeoutHandler;
    protected Handler mRssiUpdateHandler;
    protected Handler mDelayHandler;
    protected final Runnable mConnectionTimeoutRunnable = new ConnectionTimeoutRunnable();
    protected final Runnable mRssiUpdateRunnable = new RssiUpdateRunnable();
    protected final Runnable mCommandTimeoutRunnable = new CommandTimeoutRunnable();
    protected final Runnable mCommandDelayRunnable = new CommandDelayRunnable();

    protected BluetoothDevice device;
    protected BluetoothGatt gatt;
    protected int rssi = 0xFF;
    private int mtu = 20;
    //    protected String mac;
//    protected int type;
    protected List<BluetoothGattService> mServices;

    protected boolean processing = false;

    protected boolean monitorRssi;
    protected int updateIntervalMill = 5 * 1000;
    protected int commandTimeoutMill = 10 * 1000;
    protected long lastTime;
    private AtomicInteger mConnState = new AtomicInteger(CONN_STATE_IDLE);

    private final Object mProcessLock = new Object();

    public static final int OTA_PREPARE = 0xFF00;
    public static final int OTA_START = 0xFF01;
    public static final int OTA_END = 0xFF02;

    private static final int TAG_OTA_WRITE = 1;
    private static final int TAG_OTA_READ = 2;
    private static final int TAG_OTA_LAST = 3;
    private static final int TAG_OTA_PRE_READ = 4;
    private static final int TAG_OTA_PREPARE = 5; // prepare
    private static final int TAG_OTA_START = 7;
    private static final int TAG_OTA_END = 8;

    private static final int FLAG_OTA = 0x20;
    private static final int FLAG_GENERAL = 0x10;

    private DeviceCallback callback;

    private boolean serviceRefreshed = false;

    private Context mContext;

    public GattController(Context context, HandlerThread handlerThread) {
        mTimeoutHandler = new Handler(handlerThread.getLooper());
        mRssiUpdateHandler = new Handler(handlerThread.getLooper());
        mDelayHandler = new Handler(handlerThread.getLooper());
        this.mContext = context;
    }

    private AtomicBoolean isConnectWaiting = new AtomicBoolean(false);

    public void setDevice(BluetoothDevice device) {
        this.device = device;
    }

    public BluetoothDevice getDevice() {
        return this.device;
    }

    public String getMacAddress() {
        return this.device == null ? null : this.device.getAddress();
    }

    public List<BluetoothGattService> getServices() {
        return mServices;
    }

    // segment buffer by proxy notify
    private byte[] proxyNotificationSegBuffer;

    public boolean isConnected() {
        return this.mConnState.get() == CONN_STATE_CONNECTED;
    }


    /**
     * @return proxy node connected
     */
    public boolean isProxyNodeConnected() {
        if (isConnected()) {
            return getProxyService() != null;
        }
        return false;
    }

    /**
     * @return un-provisioned node connected
     */
    public boolean isUnPvNodeConnected() {
        if (isConnected()) {
            if (mServices != null) {
                for (BluetoothGattService service : mServices) {
                    if (service.getUuid().equals(UuidInfo.PROVISION_SERVICE_UUID)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    public void connect(BluetoothDevice bluetoothDevice) {
        if (isConnected() && bluetoothDevice.equals(device)) {
            this.onConnect();
            // check if discovering services executed, false means discovering is processing
            if (mServices != null) {
                onServicesDiscovered(mServices);
            }
        } else {
            this.device = bluetoothDevice;
            if (this.disconnect()) {
                // waiting for disconnected callback
                log(" waiting for disconnect -- ");
                isConnectWaiting.set(true);
            } else {
                log(" already disconnected -- ");
                // execute connecting action
                this.connect();
            }
        }
    }


    private void connect() {
        this.lastTime = 0;
        if (this.device == null) return;
        if (this.mConnState.get() == CONN_STATE_IDLE) {
            log("connect "
                    + this.getMacAddress(), LogInfo.LogLevel.WARN);
            this.mConnState.set(CONN_STATE_CONNECTING);
            this.gatt = this.device.connectGatt(mContext, false, this);
            if (this.gatt == null) {
                this.disconnect();
                this.mConnState.set(CONN_STATE_IDLE);
                this.onDisconnect();
            } else {
                mDelayHandler.postDelayed(mConnectionTimeoutRunnable, CONNECTION_TIMEOUT);
            }
        }
    }

    /**
     * @return if disconnect invoke
     * true onDisconnect will callback
     * false no callback
     */
    public boolean disconnect() {

        log("disconnect " + this.getMacAddress() + " -- " + mConnState.get());
        this.clear();
        int connState = this.mConnState.get();
        if (connState != CONN_STATE_CONNECTING && connState != CONN_STATE_CONNECTED && connState != CONN_STATE_DISCONNECTING)
            return false;
        if (this.gatt != null) {
            if (connState == CONN_STATE_CONNECTED) {
                this.mConnState.set(CONN_STATE_DISCONNECTING);
                this.gatt.disconnect();
                return true;
            } else if (connState == CONN_STATE_CONNECTING) {
                this.gatt.disconnect();
                this.gatt.close();
                this.mConnState.set(CONN_STATE_IDLE);
                return false;
            } else {
                return true;
            }
        } else {
            this.mConnState.set(CONN_STATE_IDLE);
            return false;
        }
    }

    /**
     * @param type proxy pdu type
     * @param data data excluding type
     */
    public void sendData(byte type, byte[] data) {
        final int mtu = this.mtu;
        final boolean isProvisioningPdu = type == ProxyPDU.TYPE_PROVISIONING_PDU;
        if (data.length > mtu - 1) {
            double ceil = Math.ceil(((double) data.length) / (mtu - 1));
            int pktNum = (int) ceil;
            byte oct0;
            byte[] pkt;
            for (int i = 0; i < pktNum; i++) {
                if (i != pktNum - 1) {
                    if (i == 0) {
                        oct0 = (byte) (ProxyPDU.SAR_SEG_FIRST | type);
                    } else {
                        oct0 = (byte) (ProxyPDU.SAR_SEG_CONTINUE | type);
                    }
                    pkt = new byte[mtu];
                    pkt[0] = oct0;
                    System.arraycopy(data, (mtu - 1) * i, pkt, 1, mtu - 1);
                } else {
                    oct0 = (byte) (ProxyPDU.SAR_SEG_LAST | type);
                    int restSize = data.length - (mtu - 1) * i;
                    pkt = new byte[restSize + 1];
                    pkt[0] = oct0;
                    System.arraycopy(data, (mtu - 1) * i, pkt, 1, restSize);
                }
                log("send segment pkt: " + Arrays.bytesToHexString(pkt, ":"));
                if (isProvisioningPdu) {
                    sendProvisionCmd(pkt);
                } else {
                    sendControlCmd(pkt);
                }
            }
        } else {
            byte[] proxyData = new byte[data.length + 1];
            proxyData[0] = type;
            System.arraycopy(data, 0, proxyData, 1, data.length);
            log("send unsegment pkt: " + Arrays.bytesToHexString(proxyData, ":"));
//            mGattController.sendProvisionCmd(proxyData);
            if (isProvisioningPdu) {
                sendProvisionCmd(proxyData);
            } else {
                sendControlCmd(proxyData);
            }
        }
    }

    public boolean enableOnlineStatus() {
        GattCommand cmd = GattCommand.newInstance();
        if (mConnState.get() != CONN_STATE_CONNECTED) return false;
        if (!checkOnlineStatusService()) return false;
        cmd.serviceUUID = UuidInfo.SERVICE_UUID_ONLINE_STATUS;
        cmd.characteristicUUID = UuidInfo.CHARACTERISTIC_UUID_ONLINE_STATUS;
        cmd.data = new byte[]{0x01};
        cmd.type = GattCommand.CommandType.WRITE_NO_RESPONSE;
        sendCommand(cmd);
        return true;
    }

    public void sendProvisionCmd(byte[] data) {
        GattCommand cmd = GattCommand.newInstance();
        cmd.characteristicUUID = UuidInfo.PB_IN_CHARACTERISTIC_UUID;
//        cmd.serviceUUID = UuidInfo.PROVISION_SERVICE_UUID;
        BluetoothGattService service = getProvisionService();

        if (service == null) return;
        cmd.serviceUUID = service.getUuid();
        cmd.data = data.clone();
        cmd.type = GattCommand.CommandType.WRITE_NO_RESPONSE;
        sendCommand(cmd);
    }

    public void sendControlCmd(byte[] data) {
        GattCommand cmd = GattCommand.newInstance();
//        cmd.serviceUUID = UuidInfo.PROXY_SERVICE_UUID;
        BluetoothGattService service = getProxyService();
        if (service == null) return;
        cmd.serviceUUID = service.getUuid();
        cmd.characteristicUUID = UuidInfo.PROXY_IN_CHARACTERISTIC_UUID;
        cmd.data = data.clone();
        cmd.type = GattCommand.CommandType.WRITE_NO_RESPONSE;
        sendCommand(cmd);
    }

    private boolean checkOnlineStatusService() {
        if (this.mServices == null) return false;
        for (BluetoothGattService service : mServices) {
            if (service.getUuid().equals(UuidInfo.SERVICE_UUID_ONLINE_STATUS)) {
                for (BluetoothGattCharacteristic characteristic : service.getCharacteristics()) {
                    if (characteristic.getUuid().equals(UuidInfo.CHARACTERISTIC_UUID_ONLINE_STATUS)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    private BluetoothGattService getProvisionService() {
        if (this.mServices == null) return null;
        for (BluetoothGattService service :
                mServices) {
            if (service.getUuid().equals(UuidInfo.PROVISION_SERVICE_UUID) || service.getUuid().equals(UuidInfo.UNUSED_SERVICE_UUID)) {
                for (BluetoothGattCharacteristic characteristic : service.getCharacteristics()) {
                    if (characteristic.getUuid().equals(UuidInfo.PB_IN_CHARACTERISTIC_UUID)) {
                        return service;
                    }
                }
            }
        }
        return null;
    }

    private BluetoothGattService getProxyService() {
        if (this.mServices == null) return null;
        for (BluetoothGattService service :
                mServices) {
            if (service.getUuid().equals(UuidInfo.PROXY_SERVICE_UUID) || service.getUuid().equals(UuidInfo.UNUSED_SERVICE_UUID)) {
                for (BluetoothGattCharacteristic characteristic : service.getCharacteristics()) {
                    if (characteristic.getUuid().equals(UuidInfo.PROXY_IN_CHARACTERISTIC_UUID)) {
                        return service;
                    }
                }

            }
        }
        return null;
    }

    public void proxyInit() {
        enableNotifications();
        writeCCCForPx();
    }

    public void provisionInit() {
        enableNotifications();
        writeCCCForPv();
    }

    // 27 18
    public void writeCCCForPv() {
        log("write ccc in provision service");
        GattCommand cmd = GattCommand.newInstance();
//        cmd.serviceUUID = UuidInfo.PROVISION_SERVICE_UUID;
        BluetoothGattService service = getProvisionService();
        if (service == null) return;
        cmd.serviceUUID = service.getUuid();
        cmd.characteristicUUID = UuidInfo.PB_OUT_CHARACTERISTIC_UUID;
        cmd.descriptorUUID = UuidInfo.CFG_DESCRIPTOR_UUID;
        cmd.data = new byte[]{0x01, 0x00};
        cmd.type = GattCommand.CommandType.WRITE_DESCRIPTOR;
        sendCommand(cmd);
    }


    // 28 18
    public void writeCCCForPx() {
        log("write ccc in proxy service");
        GattCommand cmd = GattCommand.newInstance();
//        cmd.serviceUUID = UuidInfo.PROXY_SERVICE_UUID;
        BluetoothGattService service = getProxyService();
        if (service == null) return;
        cmd.serviceUUID = service.getUuid();
        cmd.characteristicUUID = UuidInfo.PROXY_OUT_CHARACTERISTIC_UUID;
        cmd.descriptorUUID = UuidInfo.CFG_DESCRIPTOR_UUID;
        cmd.data = new byte[]{0x01, 0x00};
        cmd.type = GattCommand.CommandType.WRITE_DESCRIPTOR;
        sendCommand(cmd);
    }

    private void clear() {
        this.processing = false;
        this.stopMonitoringRssi();
        this.cancelCommandTimeoutTask();
        this.mInputCommandQueue.clear();
        this.mOutputCommandQueue.clear();
        this.mNotificationCallbacks.clear();
        this.mDelayHandler.removeCallbacksAndMessages(null);
    }

    public boolean refreshCache() {
        log("GattController#refreshCache", LogInfo.LogLevel.INFO);
        if (gatt == null) {
            log("refresh error: gatt null", LogInfo.LogLevel.INFO);
            return false;
        }
        try {
            BluetoothGatt localBluetoothGatt = gatt;
            Method localMethod = localBluetoothGatt.getClass().getMethod("refresh", new Class[0]);
            if (localMethod != null) {
                boolean bool = ((Boolean) localMethod.invoke(localBluetoothGatt, new Object[0])).booleanValue();
                if (bool) {
                    mDelayHandler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            gatt.discoverServices();
                        }
                    }, 0);
                }
                return bool;
            }
        } catch (Exception localException) {
            log("An exception occured while refreshing device", LogInfo.LogLevel.WARN);
        }
        return false;
    }

    public void setCallback(DeviceCallback deviceCallback) {
        this.callback = deviceCallback;
    }


    protected void onNotify(UUID charUUID, byte[] data) {
        if (charUUID.equals(UuidInfo.CHARACTERISTIC_UUID_ONLINE_STATUS)) {
            this.callback.onNotify(charUUID, data);
            return;
        }
        log("onNotify --> " + Arrays.bytesToHexString(data, ":"), LogInfo.LogLevel.VERBOSE);

        if (data == null || data.length == 0) {
            log("empty packet received!", LogInfo.LogLevel.WARN);
            return;
        }
        byte[] completePacket = getCompletePacket(data);
        if (completePacket == null) {
            log("waiting for segment pkt", LogInfo.LogLevel.VERBOSE);
            return;
        }
        log("completed notification data: " + Arrays.bytesToHexString(completePacket, ":"));
        if (completePacket.length <= 1) {
            log("complete notification length err", LogInfo.LogLevel.WARN);
            return;
        }
        this.callback.onNotify(charUUID, completePacket);
    }

    private byte[] getCompletePacket(byte[] data) {
        byte sar = (byte) (data[0] & ProxyPDU.BITS_SAR);
        switch (sar) {
            case ProxyPDU.SAR_COMPLETE:
                return data;

            case ProxyPDU.SAR_SEG_FIRST:
                data[0] = (byte) (data[0] & ProxyPDU.BITS_TYPE);
                proxyNotificationSegBuffer = data;
                return null;

            case ProxyPDU.SAR_SEG_CONTINUE:
            case ProxyPDU.SAR_SEG_LAST:
                if (proxyNotificationSegBuffer != null) {
                    int segType = proxyNotificationSegBuffer[0] & ProxyPDU.BITS_TYPE;
                    int dataType = data[0] & ProxyPDU.BITS_TYPE;

                    // check if pkt typeValue equals
                    if (segType == dataType && data.length > 1) {
                        byte[] tempBuffer = new byte[proxyNotificationSegBuffer.length + data.length - 1];
                        System.arraycopy(proxyNotificationSegBuffer, 0, tempBuffer, 0, proxyNotificationSegBuffer.length);
                        System.arraycopy(data, 1, tempBuffer, proxyNotificationSegBuffer.length, data.length - 1);
                        if (sar == ProxyPDU.SAR_SEG_CONTINUE) {
                            proxyNotificationSegBuffer = tempBuffer;
                            return null;
                        } else {
                            proxyNotificationSegBuffer = null;
                            return tempBuffer;
                        }
                    } else {
                        log("other segment ", LogInfo.LogLevel.WARN);
                    }
                } else {
                    log("segment first pkt no found", LogInfo.LogLevel.WARN);
                }

            default:
                return null;
        }
    }


    protected void onEnableNotify() {
        this.callback.onNotifyEnable();
    }

    protected void onDisableNotify() {

    }

    protected void onRssiChanged() {
    }

    protected void enableMonitorRssi(boolean enable) {

        if (enable) {
            this.mRssiUpdateHandler.removeCallbacks(this.mRssiUpdateRunnable);
            this.mRssiUpdateHandler.postDelayed(this.mRssiUpdateRunnable, this.updateIntervalMill);
        } else {
            this.mRssiUpdateHandler.removeCallbacks(this.mRssiUpdateRunnable);
            this.mRssiUpdateHandler.removeCallbacksAndMessages(null);
        }
    }


    private void postCommand(CommandContext commandContext) {

//        log("postCommand");

        this.mInputCommandQueue.add(commandContext);
        synchronized (this.mProcessLock) {
            if (!this.processing) {
                this.processCommand();
            }
        }
    }

    private void processCommand() {

//        log("processing : " + this.processing);

        CommandContext commandContext;
        GattCommand.CommandType commandType;

        synchronized (mInputCommandQueue) {
            if (this.mInputCommandQueue.isEmpty()) {
                return;
            }
            commandContext = this.mInputCommandQueue.poll();

        }

        if (commandContext == null)
            return;
        commandType = commandContext.gattCommand.type;

        if (commandType != GattCommand.CommandType.ENABLE_NOTIFY && commandType != GattCommand.CommandType.DISABLE_NOTIFY) {
            this.processing = true;
            synchronized (mOutputCommandQueue) {
                this.mOutputCommandQueue.add(commandContext);
            }
        }
        int delay = commandContext.gattCommand.delay;
        if (delay > 0) {
            long currentTime = System.currentTimeMillis();
            if (lastTime == 0 || (currentTime - lastTime) >= delay)
                this.processCommand(commandContext);
            else
                this.mDelayHandler.postDelayed(this.mCommandDelayRunnable, delay);
        } else {
            this.processCommand(commandContext);
        }
    }

    synchronized private void processCommand(CommandContext commandContext) {
        GattCommand gattCommand = commandContext.gattCommand;
        GattCommand.CommandType commandType = gattCommand.type;

        log("processCommand : " + gattCommand.toString(), LogInfo.LogLevel.VERBOSE);

        switch (commandType) {
            case READ:
                this.postCommandTimeoutTask();
                this.readCharacteristic(commandContext, gattCommand.serviceUUID,
                        gattCommand.characteristicUUID);
                break;
            case WRITE:
                this.postCommandTimeoutTask();
                this.writeCharacteristic(commandContext, gattCommand.serviceUUID,
                        gattCommand.characteristicUUID,
                        BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT,
                        gattCommand.data);
                break;
            case READ_DESCRIPTOR:
                this.postCommandTimeoutTask();
                this.readDescriptor(commandContext, gattCommand.serviceUUID, gattCommand.characteristicUUID, gattCommand.descriptorUUID);
                break;
            case WRITE_NO_RESPONSE:
                this.postCommandTimeoutTask();
                this.writeCharacteristic(commandContext, gattCommand.serviceUUID,
                        gattCommand.characteristicUUID,
                        BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE,
                        gattCommand.data);
                break;

            case WRITE_DESCRIPTOR:
                this.postCommandTimeoutTask();
                this.writeDescriptor(commandContext, gattCommand.serviceUUID, gattCommand.characteristicUUID, gattCommand.descriptorUUID, gattCommand.data);
                break;
            case ENABLE_NOTIFY:
                this.enableNotification(gattCommand.serviceUUID, gattCommand.characteristicUUID);
                break;
            case DISABLE_NOTIFY:
                this.disableNotification(commandContext, gattCommand.serviceUUID,
                        gattCommand.characteristicUUID);
                break;
        }
    }


    public boolean sendCommand(GattCommand gattCommand) {
        if (this.mConnState.get() != CONN_STATE_CONNECTED)
            return false;

        CommandContext commandContext = new CommandContext(gattCommand);
        this.postCommand(commandContext);

        return true;
    }

    public final void startMonitoringRssi(int interval) {

        this.monitorRssi = true;

        if (interval <= 0)
            this.updateIntervalMill = RSSI_UPDATE_TIME_INTERVAL;
        else
            this.updateIntervalMill = interval;
    }

    public final void stopMonitoringRssi() {
        this.monitorRssi = false;
        this.mRssiUpdateHandler.removeCallbacks(this.mRssiUpdateRunnable);
        this.mRssiUpdateHandler.removeCallbacksAndMessages(null);
    }

    public final boolean requestConnectionPriority(int connectionPriority) {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP && this.gatt.requestConnectionPriority(connectionPriority);
    }


    protected void onConnect() {
        this.mDelayHandler.removeCallbacks(mConnectionTimeoutRunnable);
        if (this.callback != null)
            this.callback.onConnected();
    }

    protected void onDisconnect() {
        serviceRefreshed = false;
        this.mServices = null;
        if (isConnectWaiting.get()) {
            this.connect();
        } else {
            if (this.callback != null)
                this.callback.onDisconnected();
        }

    }


    protected void onServicesDiscovered(List<BluetoothGattService> services) {

        if (serviceRefreshed) {
            this.callback.onServicesDiscovered(services);
        } else {
            serviceRefreshed = true;
            this.refreshCache();
        }

    }


    /********************************************************************************
     * OTA API start
     *******************************************************************************/

    private final OtaPacketParser mOtaParser = new OtaPacketParser();

    private int readCnt = 0;

    public void startOta(byte[] firmware) {

        log("Start OTA");
        this.resetOta();
        this.mOtaParser.set(firmware);
        this.sendOTAPrepareCommand();
    }

    public int getOtaProgress() {
        return this.mOtaParser.getProgress();
    }

    private void resetOta() {
        this.readCnt = 0;
        this.mDelayHandler.removeCallbacksAndMessages(null);
        this.mOtaParser.clear();
    }

    private void setOtaProgressChanged() {

        if (this.mOtaParser.invalidateProgress()) {
            onOtaProgress();
        }
    }

    protected void onOtaSuccess() {
        if (callback != null) {
            callback.onOtaStateChanged(OTA_STATE_SUCCESS);
        }
    }

    protected void onOtaFailure() {
        if (callback != null) {
            callback.onOtaStateChanged(OTA_STATE_FAILURE);
        }
    }

    protected void onOtaProgress() {
        if (callback != null) {
            callback.onOtaStateChanged(OTA_STATE_PROGRESS);
        }
    }

    private void sendOTAPrepareCommand() {
        GattCommand prepareCmd = GattCommand.newInstance();
        prepareCmd.serviceUUID = UuidInfo.SERVICE_UUID_OTA;
        prepareCmd.characteristicUUID = UuidInfo.CHARACTERISTIC_UUID_OTA;
        prepareCmd.type = GattCommand.CommandType.WRITE_NO_RESPONSE;
        prepareCmd.tag = TAG_OTA_PREPARE;
        prepareCmd.flag = FLAG_OTA;
        prepareCmd.data = new byte[]{OTA_PREPARE & 0xFF, (byte) (OTA_PREPARE >> 8 & 0xFF)};
        sendCommand(prepareCmd);
    }

    // send ota start command
    private void sendOtaStartCommand() {
        GattCommand startCmd = GattCommand.newInstance();
        startCmd.serviceUUID = UuidInfo.SERVICE_UUID_OTA;
        startCmd.characteristicUUID = UuidInfo.CHARACTERISTIC_UUID_OTA;
        startCmd.type = GattCommand.CommandType.WRITE_NO_RESPONSE;
        startCmd.tag = TAG_OTA_START;
        startCmd.flag = FLAG_OTA;
        startCmd.data = new byte[]{OTA_START & 0xFF, (byte) (OTA_START >> 8 & 0xFF)};
        sendCommand(startCmd);
    }

    private void sendOtaEndCommand() {
        GattCommand endCmd = GattCommand.newInstance();
        endCmd.serviceUUID = UuidInfo.SERVICE_UUID_OTA;
        endCmd.characteristicUUID = UuidInfo.CHARACTERISTIC_UUID_OTA;
        endCmd.type = GattCommand.CommandType.WRITE_NO_RESPONSE;
        endCmd.tag = TAG_OTA_END;
        endCmd.flag = FLAG_OTA;
        int index = mOtaParser.getIndex();
        /*endCmd.data = new byte[]{OTA_END & 0xFF, (byte) (OTA_END >> 8 & 0xFF),
                (byte) (index & 0xFF), (byte) (index >> 8 & 0xFF),
                (byte) (~index & 0xFF), (byte) (~index >> 8 & 0xFF)
        };*/
        byte[] data = new byte[6];
        data[0] = OTA_END & 0xFF;
        data[1] = (byte) ((OTA_END >> 8) & 0xFF);
        data[2] = (byte) (index & 0xFF);
        data[3] = (byte) (index >> 8 & 0xFF);
        data[4] = (byte) (~index & 0xFF);
        data[5] = (byte) (~index >> 8 & 0xFF);

//        int crc = mOtaParser.crc16(data);
//        mOtaParser.fillCrc(data, crc);
        endCmd.data = data;
        sendCommand(endCmd);
    }


    private boolean sendNextOtaPacketCommand(int delay) {
        boolean result = false;
        if (this.mOtaParser.hasNextPacket()) {
            GattCommand cmd = GattCommand.newInstance();
            cmd.serviceUUID = UuidInfo.SERVICE_UUID_OTA;
            cmd.characteristicUUID = UuidInfo.CHARACTERISTIC_UUID_OTA;
            cmd.type = GattCommand.CommandType.WRITE_NO_RESPONSE;
            cmd.flag = FLAG_OTA;
            cmd.data = this.mOtaParser.getNextPacket();
            if (this.mOtaParser.isLast()) {
                cmd.tag = TAG_OTA_LAST;
                result = true;
            } else {
                cmd.tag = TAG_OTA_WRITE;
            }
            cmd.delay = delay;
            this.sendCommand(cmd);

        }
        return result;
    }

    private boolean validateOta() {
        /**
         * read
         */
        int sectionSize = 16 * 8;
        int sendTotal = this.mOtaParser.getNextPacketIndex() * 16;
//        log("ota onCommandSampled byte length : " + sendTotal);
        if (sendTotal > 0 && sendTotal % sectionSize == 0) {
            log("onCommandSampled ota read packet " + mOtaParser.getNextPacketIndex(), LogInfo.LogLevel.VERBOSE);
            GattCommand cmd = GattCommand.newInstance();
            cmd.serviceUUID = UuidInfo.SERVICE_UUID_OTA;
            cmd.characteristicUUID = UuidInfo.CHARACTERISTIC_UUID_OTA;
            cmd.type = GattCommand.CommandType.READ;
            cmd.tag = TAG_OTA_READ;
            cmd.flag = FLAG_OTA;
            readCnt++;
            log("cur read count: " + readCnt);
            this.sendCommand(cmd);
            return true;
        }
        return false;
    }


    private void onOTACmdSuccess(GattCommand command, Object data) {

        if (command.tag.equals(TAG_OTA_PRE_READ)) {
            log("read =========> " + Arrays.bytesToHexString((byte[]) data, "-"));
        } else if (command.tag.equals(TAG_OTA_PREPARE)) {
            sendOtaStartCommand();
        } else if (command.tag.equals(TAG_OTA_START)) {
            sendNextOtaPacketCommand(0);
        } else if (command.tag.equals(TAG_OTA_END)) {
            setOtaProgressChanged();
            resetOta();
            onOtaSuccess();
        } else if (command.tag.equals(TAG_OTA_LAST)) {
            sendOtaEndCommand();
        } else if (command.tag.equals(TAG_OTA_WRITE)) {

            if (!validateOta()) {
                sendNextOtaPacketCommand(0);
            }
            setOtaProgressChanged();
        } else if (command.tag.equals(TAG_OTA_READ)) {
            sendNextOtaPacketCommand(0);
        }


    }

    /********************************************************************************
     * OTA API end
     *******************************************************************************/


    private void commandCompleted() {

        log("commandCompleted", LogInfo.LogLevel.VERBOSE);

        synchronized (this.mProcessLock) {
            if (this.processing)
                this.processing = false;
            this.processCommand();
        }
    }

    private void commandSuccess(CommandContext commandContext, byte[] data) {
        log("commandSuccess", LogInfo.LogLevel.VERBOSE);
        this.lastTime = System.currentTimeMillis();
        if (commandContext != null) {
            GattCommand gattCommand = commandContext.gattCommand;
            if (this.callback != null) {
                this.callback.onCommandSuccess(gattCommand.characteristicUUID, data);
            }
            if (gattCommand.flag == FLAG_OTA) {
                onOTACmdSuccess(gattCommand, data);
            }
//            log("commandSuccess -- " + Arrays.bytesToHexString(gattCommand.data, ":"));
            commandContext.clear();
        }
    }


    private void commandSuccess(byte[] data) {
        CommandContext commandContext;
        commandContext = this.mOutputCommandQueue.poll();
        this.commandSuccess(commandContext, data);
    }

    private void commandError(CommandContext commandContext, String errorMsg) {

        log("commandError:" + errorMsg, LogInfo.LogLevel.WARN);
        this.callback.onCommandError(errorMsg);
        this.lastTime = System.currentTimeMillis();
        if (commandContext != null) {

            GattCommand gattCommand = commandContext.gattCommand;
            commandContext.clear();

            if (gattCommand.flag == FLAG_OTA) {
                onOtaCmdError(gattCommand);
            }
        }
    }

    private void onOtaCmdError(GattCommand gattCommand) {
        if (gattCommand.tag.equals(TAG_OTA_END)) {
            // ota success
            setOtaProgressChanged();
            resetOta();
            onOtaSuccess();
        } else {
            resetOta();
            onOtaFailure();
        }
    }


    private void commandError(String errorMsg) {

        CommandContext commandContext;
        commandContext = this.mOutputCommandQueue.poll();
        this.commandError(commandContext, errorMsg);
    }

    private boolean commandTimeout(CommandContext commandContext) {
        log("commandTimeout", LogInfo.LogLevel.WARN);
        this.lastTime = System.currentTimeMillis();
        if (commandContext != null) {

            GattCommand gattCommand = commandContext.gattCommand;
            commandContext.clear();
            if (gattCommand.flag == FLAG_OTA) {
                onOtaCmdTimeout(gattCommand);
            }
        }

        return false;
    }

    private void onOtaCmdTimeout(GattCommand gattCommand) {
        if (gattCommand.tag.equals(TAG_OTA_END)) {
            // ota success
            setOtaProgressChanged();
            resetOta();
            onOtaSuccess();
        } else {
            resetOta();
            onOtaFailure();
        }
    }

    private void postCommandTimeoutTask() {

        if (this.commandTimeoutMill <= 0)
            return;

        this.mTimeoutHandler.removeCallbacksAndMessages(null);
        this.mTimeoutHandler.postDelayed(this.mCommandTimeoutRunnable, this.commandTimeoutMill);
    }

    private void cancelCommandTimeoutTask() {
        this.mTimeoutHandler.removeCallbacksAndMessages(null);
    }


    private void readDescriptor(CommandContext commandContext,
                                UUID serviceUUID, UUID characteristicUUID, UUID descriptorUUID) {

        boolean success = true;
        String errorMsg = "";

        BluetoothGattService service = this.gatt.getService(serviceUUID);

        if (service != null) {

            BluetoothGattCharacteristic characteristic = service
                    .getCharacteristic(characteristicUUID);

            if (characteristic != null) {

                BluetoothGattDescriptor descriptor = characteristic.getDescriptor(descriptorUUID);
                if (descriptor != null) {
                    if (!this.gatt.readDescriptor(descriptor)) {
                        success = false;
                        errorMsg = "read descriptor error";
                    }
                } else {
                    success = false;
                    errorMsg = "read descriptor error";
                }

            } else {
                success = false;
                errorMsg = "read characteristic error";
            }
        } else {
            success = false;
            errorMsg = "service is not offered by the remote device";
        }

        if (!success) {
            this.commandError(errorMsg);
            this.commandCompleted();
        }
    }


    private void writeDescriptor(CommandContext commandContext,
                                 UUID serviceUUID, UUID characteristicUUID, UUID descriptorUUID,
                                 byte[] data) {

        boolean success = true;
        String errorMsg = "";

        BluetoothGattService service = this.gatt.getService(serviceUUID);

        if (service != null) {
            BluetoothGattCharacteristic characteristic = service.getCharacteristic(characteristicUUID);

            if (characteristic != null) {

                BluetoothGattDescriptor descriptor = characteristic.getDescriptor(descriptorUUID);
                if (descriptor != null) {
                    descriptor.setValue(data);
                    if (!this.gatt.writeDescriptor(descriptor)) {
                        success = false;
                        errorMsg = "write characteristic error";
                    }
                } else {
                    success = false;
                    errorMsg = "no descriptor";
                }


            } else {
                success = false;
                errorMsg = "no characteristic";
            }
        } else {
            success = false;
            errorMsg = "service is not offered by the remote device";
        }

        if (!success) {
            this.commandError(errorMsg);
            this.commandCompleted();
        }
    }


    private void readCharacteristic(CommandContext commandContext,
                                    UUID serviceUUID, UUID characteristicUUID) {

        boolean success = true;
        String errorMsg = "";

        BluetoothGattService service = this.gatt.getService(serviceUUID);

        if (service != null) {

            BluetoothGattCharacteristic characteristic = service
                    .getCharacteristic(characteristicUUID);

            if (characteristic != null) {

                if (!this.gatt.readCharacteristic(characteristic)) {
                    success = false;
                    errorMsg = "read characteristic error";
                }

            } else {
                success = false;
                errorMsg = "read characteristic error";
            }
        } else {
            success = false;
            errorMsg = "service is not offered by the remote device";
        }

        if (!success) {
            this.commandError(errorMsg);
            this.commandCompleted();
        }
    }

    private void writeCharacteristic(CommandContext commandContext,
                                     UUID serviceUUID, UUID characteristicUUID, int writeType,
                                     byte[] data) {

        boolean success = true;
        String errorMsg = "";

        BluetoothGattService service = this.gatt.getService(serviceUUID);

        if (service != null) {
            BluetoothGattCharacteristic characteristic = this
                    .findWritableCharacteristic(service, characteristicUUID,
                            writeType);
            if (characteristic != null) {

                characteristic.setValue(data);
                characteristic.setWriteType(writeType);

                if (!this.gatt.writeCharacteristic(characteristic)) {
                    success = false;
                    errorMsg = "write characteristic error";
                }

            } else {
                success = false;
                errorMsg = "no characteristic";
            }
        } else {
            success = false;
            errorMsg = "service is not offered by the remote device";
        }

        if (!success) {
            this.commandError(errorMsg);
            this.commandCompleted();
        }
    }

    public void enableNotifications() {

        BluetoothGattService provisionService = getProvisionService();
        if (provisionService != null) {
            enableNotification(provisionService.getUuid(), UuidInfo.PB_OUT_CHARACTERISTIC_UUID);
        }

        BluetoothGattService proxyService = getProxyService();
        if (proxyService != null) {
            enableNotification(proxyService.getUuid(), UuidInfo.PROXY_OUT_CHARACTERISTIC_UUID);
        }

        enableNotification(UuidInfo.SERVICE_UUID_ONLINE_STATUS, UuidInfo.CHARACTERISTIC_UUID_ONLINE_STATUS);
    }


    public void enableNotification(UUID serviceUUID, UUID characteristicUUID) {

        boolean success = true;
        String errorMsg = "";
//        BluetoothGattService service = getProxyService();
//        BluetoothGattService service = getProvisionService();
        BluetoothGattService service = this.gatt.getService(serviceUUID);

        if (service != null) {
            BluetoothGattCharacteristic characteristic = this
                    .findNotifyCharacteristic(service, characteristicUUID);

            if (characteristic != null) {

                if (!this.gatt.setCharacteristicNotification(characteristic,
                        true)) {
                    success = false;
                    errorMsg = "enable notification error";
                }

            } else {
                success = false;
                errorMsg = "no characteristic";
            }
        } else {
            success = false;
            errorMsg = "service is not offered by the remote device";
        }


        if (!success) {
            log("enable notification error: " + errorMsg + " - " + characteristicUUID);
        } else {
            this.onEnableNotify();
        }
    }


    private void disableNotification(CommandContext commandContext,
                                     UUID serviceUUID, UUID characteristicUUID) {

        boolean success = true;
        String errorMsg = "";

        BluetoothGattService service = this.gatt.getService(serviceUUID);

        if (service != null) {

            BluetoothGattCharacteristic characteristic = this
                    .findNotifyCharacteristic(service, characteristicUUID);

            if (characteristic != null) {

                String key = this.generateHashKey(serviceUUID, characteristic);
                this.mNotificationCallbacks.remove(key);

                if (!this.gatt.setCharacteristicNotification(characteristic,
                        false)) {
                    success = false;
                    errorMsg = "disable notification error";
                }

            } else {
                success = false;
                errorMsg = "no characteristic";
            }

        } else {
            success = false;
            errorMsg = "service is not offered by the remote device";
        }

        if (!success) {
            this.commandError(commandContext, errorMsg);
        } else {
            this.onDisableNotify();
        }

        this.commandCompleted();
    }

    private BluetoothGattCharacteristic findWritableCharacteristic(
            BluetoothGattService service, UUID characteristicUUID, int writeType) {

        BluetoothGattCharacteristic characteristic = null;

        int writeProperty = BluetoothGattCharacteristic.PROPERTY_WRITE;

        if (writeType == BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE) {
            writeProperty = BluetoothGattCharacteristic.PROPERTY_WRITE_NO_RESPONSE;
        }

        List<BluetoothGattCharacteristic> characteristics = service
                .getCharacteristics();

        for (BluetoothGattCharacteristic c : characteristics) {
            if ((c.getProperties() & writeProperty) != 0
                    && characteristicUUID.equals(c.getUuid())) {
                characteristic = c;
                break;
            }
        }

        return characteristic;
    }

    private BluetoothGattCharacteristic findNotifyCharacteristic(
            BluetoothGattService service, UUID characteristicUUID) {

        BluetoothGattCharacteristic characteristic = null;

        List<BluetoothGattCharacteristic> characteristics = service
                .getCharacteristics();

        for (BluetoothGattCharacteristic c : characteristics) {
            if ((c.getProperties() & BluetoothGattCharacteristic.PROPERTY_NOTIFY) != 0
                    && characteristicUUID.equals(c.getUuid())) {
                characteristic = c;
                break;
            }
        }

        if (characteristic != null)
            return characteristic;

        for (BluetoothGattCharacteristic c : characteristics) {
            if ((c.getProperties() & BluetoothGattCharacteristic.PROPERTY_INDICATE) != 0
                    && characteristicUUID.equals(c.getUuid())) {
                characteristic = c;
                break;
            }
        }

        return characteristic;
    }

    private String generateHashKey(BluetoothGattCharacteristic characteristic) {
        return this.generateHashKey(characteristic.getService().getUuid(),
                characteristic);
    }

    protected String generateHashKey(UUID serviceUUID,
                                     BluetoothGattCharacteristic characteristic) {
        return String.valueOf(serviceUUID) + "|" + characteristic.getUuid()
                + "|" + characteristic.getInstanceId();
    }


    @Override
    public void onConnectionStateChange(BluetoothGatt gatt, int status,
                                        int newState) {
        log("onConnectionStateChange  status :" + status + " state : "
                + newState, LogInfo.LogLevel.WARN);

        if (newState == BluetoothGatt.STATE_CONNECTED && status == BluetoothGatt.GATT_SUCCESS) {

            this.mConnState.set(CONN_STATE_CONNECTED);

            if (this.gatt == null || !this.gatt.discoverServices()) {
                log("remote service discovery has been stopped status = "
                        + newState, LogInfo.LogLevel.WARN);
                this.disconnect();
            } else {
//                this.gatt.requestConnectionPriority(BluetoothGatt.CONNECTION_PRIORITY_HIGH);
                this.onConnect();
            }

        } else {

            log("Close");

            if (this.gatt != null) {
                this.gatt.close();
//                this.mConnState.set(CONN_STATE_CLOSED);
            }

            this.clear();
            this.mConnState.set(CONN_STATE_IDLE);
            this.onDisconnect();
        }
    }

    @Override
    public void onCharacteristicChanged(BluetoothGatt gatt,
                                        BluetoothGattCharacteristic characteristic) {
        super.onCharacteristicChanged(gatt, characteristic);

        String key = this.generateHashKey(characteristic);
        this.onNotify(characteristic.getUuid(), characteristic.getValue());
    }

    @Override
    public void onCharacteristicRead(BluetoothGatt gatt,
                                     BluetoothGattCharacteristic characteristic, int status) {
        super.onCharacteristicRead(gatt, characteristic, status);

        this.cancelCommandTimeoutTask();

        if (status == BluetoothGatt.GATT_SUCCESS) {
            byte[] data = characteristic.getValue();
            this.commandSuccess(data);
        } else {
            this.commandError("read characteristic failed");
        }

        this.commandCompleted();
    }

    @Override
    public void onCharacteristicWrite(BluetoothGatt gatt,
                                      BluetoothGattCharacteristic characteristic, int status) {
        super.onCharacteristicWrite(gatt, characteristic, status);

        this.cancelCommandTimeoutTask();

        if (status == BluetoothGatt.GATT_SUCCESS) {
            this.commandSuccess(null);
        } else {
            this.commandError("write characteristic fail");
        }

        log("onCharacteristicWrite newStatus : " + status, LogInfo.LogLevel.VERBOSE);

        this.commandCompleted();
    }

    @Override
    public void onDescriptorRead(BluetoothGatt gatt,
                                 BluetoothGattDescriptor descriptor, int status) {
        super.onDescriptorRead(gatt, descriptor, status);

        this.cancelCommandTimeoutTask();

        if (status == BluetoothGatt.GATT_SUCCESS) {
            byte[] data = descriptor.getValue();
            this.commandSuccess(data);
        } else {
            this.commandError("read description failed");
        }

        this.commandCompleted();
    }

    @Override
    public void onDescriptorWrite(BluetoothGatt gatt,
                                  BluetoothGattDescriptor descriptor, int status) {
        super.onDescriptorWrite(gatt, descriptor, status);

        this.cancelCommandTimeoutTask();

        if (status == BluetoothGatt.GATT_SUCCESS) {
            this.commandSuccess(null);
        } else {
            this.commandError("write description failed");
        }

        this.commandCompleted();
    }

    @Override
    public void onServicesDiscovered(BluetoothGatt gatt, int status) {
        super.onServicesDiscovered(gatt, status);

        if (status == BluetoothGatt.GATT_SUCCESS) {
            List<BluetoothGattService> services = gatt.getServices();
            this.mServices = services;
            this.onServicesDiscovered(services);
        } else {
            log("Service discovery failed", LogInfo.LogLevel.WARN);
            this.disconnect();
        }
    }

    @Override
    public void onReadRemoteRssi(BluetoothGatt gatt, int rssi, int status) {
        super.onReadRemoteRssi(gatt, rssi, status);

        if (status == BluetoothGatt.GATT_SUCCESS) {

            if (rssi != this.rssi) {
                this.rssi = rssi;
                this.onRssiChanged();
            }
        }
    }

    @Override
    public void onMtuChanged(BluetoothGatt gatt, int mtu, int status) {
        super.onMtuChanged(gatt, mtu, status);
        this.mtu = mtu;
        log("mtu changed : " + mtu);
    }

    @Override
    public void onReliableWriteCompleted(BluetoothGatt gatt, int status) {
        super.onReliableWriteCompleted(gatt, status);
    }

    public final class CommandContext {

        public GattCommand gattCommand;

        public CommandContext(GattCommand gattCommand) {
            this.gattCommand = gattCommand;
        }

        public void clear() {
            this.gattCommand = null;
        }
    }


    private final class ConnectionTimeoutRunnable implements Runnable {

        @Override
        public void run() {
            if (!disconnect()) {
                onDisconnect();
            }
        }
    }

    private final class RssiUpdateRunnable implements Runnable {

        @Override
        public void run() {

            if (!monitorRssi)
                return;

            if (!isConnected())
                return;

            if (gatt != null)
                gatt.readRemoteRssi();

            mRssiUpdateHandler.postDelayed(mRssiUpdateRunnable, updateIntervalMill);
        }
    }

    private final class CommandTimeoutRunnable implements Runnable {

        @Override
        public void run() {

            synchronized (mOutputCommandQueue) {

                CommandContext commandContext = mOutputCommandQueue.peek();

                if (commandContext != null) {

                    GattCommand gattCommand = commandContext.gattCommand;

                    boolean retry = commandTimeout(commandContext);

                    if (retry) {
                        commandContext.gattCommand = gattCommand;
                        processCommand(commandContext);
                    } else {
                        mOutputCommandQueue.poll();
                        commandCompleted();
                    }
                }
            }
        }
    }

    private final class CommandDelayRunnable implements Runnable {

        @Override
        public void run() {
            synchronized (mOutputCommandQueue) {

                CommandContext commandContext = mOutputCommandQueue.peek();
                processCommand(commandContext);
            }
        }
    }


    public interface DeviceCallback {
        void onConnected();

        void onDisconnected();

        void onServicesDiscovered(List<BluetoothGattService> services);

        void onNotifyEnable();

        void onNotify(UUID charUUID, byte[] data);

        void onCommandError(String errorMsg);

        void onCommandSuccess(UUID charUUID, byte[] data);

        void onOtaStateChanged(int state);
    }

    private void log(String logInfo) {
        log(logInfo, LogInfo.LogLevel.DEBUG, TAG);
    }

    private void log(String logInfo, LogInfo.LogLevel level) {
        log(logInfo, level, TAG);
    }

    private void log(String logInfo, LogInfo.LogLevel level, String tag) {
        switch (level) {
            case INFO:
                TelinkLog.i(TAG + logInfo);
                break;
            case WARN:
                TelinkLog.w(TAG + logInfo);
                break;
            case DEBUG:
                TelinkLog.d(TAG + logInfo);
                break;
            case VERBOSE:
                TelinkLog.v(TAG + logInfo);
                break;
            case ERROR:
                TelinkLog.e(TAG + logInfo);
                break;
        }

    }
}
