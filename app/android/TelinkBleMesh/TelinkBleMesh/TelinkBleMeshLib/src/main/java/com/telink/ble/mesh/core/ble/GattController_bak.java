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
import android.os.Looper;

import com.telink.ble.mesh.entity.AdvertisingDevice;
import com.telink.ble.mesh.util.Arrays;
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

public class GattController_bak extends BluetoothGattCallback {

    public static final int STATE_SUCCESS = 1;
    public static final int STATE_FAILURE = 0;
    public static final int STATE_PROGRESS = 2;

    private static final int CONN_STATE_IDLE = 1;
    private static final int CONN_STATE_CONNECTING = 2;
    private static final int CONN_STATE_CONNECTED = 4;
    private static final int CONN_STATE_DISCONNECTING = 8;
    private static final int CONN_STATE_CLOSED = 16;

    private static final int RSSI_UPDATE_TIME_INTERVAL = 2000;

    protected final Queue<CommandContext> mInputCommandQueue = new ConcurrentLinkedQueue<>();

    protected final Queue<CommandContext> mOutputCommandQueue = new ConcurrentLinkedQueue<>();
    protected final Map<String, CommandContext> mNotificationCallbacks = new ConcurrentHashMap<>();

    protected final Handler mTimeoutHandler = new Handler(Looper.getMainLooper());
    protected final Handler mRssiUpdateHandler = new Handler(Looper.getMainLooper());
    protected final Handler mDelayHandler = new Handler(Looper.getMainLooper());
    protected final Runnable mRssiUpdateRunnable = new RssiUpdateRunnable();
    protected final Runnable mCommandTimeoutRunnable = new CommandTimeoutRunnable();
    protected final Runnable mCommandDelayRunnable = new CommandDelayRunnable();

    protected BluetoothDevice device;
    protected BluetoothGatt gatt;
    protected int rssi;
    //    protected String mac;
//    protected int type;
    protected List<BluetoothGattService> mServices;

    protected AtomicBoolean processing = new AtomicBoolean(false);

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

    public GattController_bak(Context context) {
        this.mContext = context;
    }

    /*public GattController(BluetoothDevice device, int rssi) {
        this.device = device;
        this.rssi = rssi;
    }*/

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


    public boolean isConnected() {
        return this.mConnState.get() == CONN_STATE_CONNECTED;
    }

    public void connect(AdvertisingDevice advertisingDevice){
        
    }


    public void connect() {
        this.lastTime = 0;
        if (this.device == null) return;
        if (this.mConnState.get() == CONN_STATE_IDLE) {
            TelinkLog.w("connect "
                    + this.getMacAddress());
            this.mConnState.set(CONN_STATE_CONNECTING);
            this.gatt = this.device.connectGatt(mContext, false, this);
            if (this.gatt == null) {
                this.disconnect();
                this.mConnState.set(CONN_STATE_IDLE);
                this.onDisconnect();
            }
        }
    }

    /**
     * @return if disconnect invoke
     * true onDisconnect will callback
     * false no callback
     */
    public boolean disconnect() {

        TelinkLog.w("disconnect " + this.getMacAddress() + " -- " + mConnState.get());
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
     * disconnect backup
     */
    /*public void disconnect() {

        if (this.mConnState.get() != CONN_STATE_CONNECTING && this.mConnState.get() != CONN_STATE_CONNECTED)
            return;

        TelinkLog.w("disconnect " + this.getDeviceName() + " -- "
                + this.getMacAddress());

        this.clear();

        if (this.gatt != null) {
            int connState = this.mConnState.get();
            if (connState == CONN_STATE_CONNECTED) {
                this.gatt.disconnect();
                this.mConnState.set(CONN_STATE_DISCONNECTING);
            } else {
                this.gatt.disconnect();
                this.gatt.close();
                this.mConnState.set(CONN_STATE_CLOSED);
            }
        } else {
            this.mConnState.set(CONN_STATE_IDLE);
        }
    }*/
    public void sendProvisionCmd(byte[] data) {
        GattCommand cmd = GattCommand.newInstance();
        cmd.characteristicUUID = UuidInfo.PB_IN_CHARACTERISTIC_UUID;
        cmd.serviceUUID = UuidInfo.PROVISION_SERVICE_UUID;
        cmd.data = data.clone();
        cmd.type = GattCommand.CommandType.WRITE_NO_RESPONSE;
        sendCommand(cmd);
    }

    public void sendControlCmd(byte[] data) {
        GattCommand cmd = GattCommand.newInstance();
        cmd.characteristicUUID = UuidInfo.PROXY_IN_CHARACTERISTIC_UUID;
        cmd.serviceUUID = UuidInfo.PROXY_SERVICE_UUID;
        cmd.data = data.clone();
        cmd.type = GattCommand.CommandType.WRITE_NO_RESPONSE;
//        cmd.delay = 20;
        sendCommand(cmd);
    }

    // 27 18
    public void writeCCCForPv() {
        GattCommand cmd = GattCommand.newInstance();
        cmd.serviceUUID = UuidInfo.PROVISION_SERVICE_UUID;
        cmd.characteristicUUID = UuidInfo.PB_OUT_CHARACTERISTIC_UUID;
        cmd.descriptorUUID = UuidInfo.CFG_DESCRIPTOR_UUID;
        cmd.data = new byte[]{0x01, 0x00};
        cmd.type = GattCommand.CommandType.WRITE_DESCRIPTOR;
        sendCommand(cmd);
    }


    // 28 18
    public void writeCCCForPx() {
        GattCommand cmd = GattCommand.newInstance();
        cmd.serviceUUID = UuidInfo.PROXY_SERVICE_UUID;
        cmd.characteristicUUID = UuidInfo.PROXY_OUT_CHARACTERISTIC_UUID;
        cmd.descriptorUUID = UuidInfo.CFG_DESCRIPTOR_UUID;
        cmd.data = new byte[]{0x01, 0x00};
        cmd.type = GattCommand.CommandType.WRITE_DESCRIPTOR;
        sendCommand(cmd);
    }

    /*public void writeCCCForPv() {
        GattCommand cmd = GattCommand.newInstance();
        cmd.serviceUUID = USER_DEFINE_SERVICE_UUID;
        cmd.characteristicUUID = USER_DEFINE_CHARACTERISTIC_UUID;
        cmd.data = new byte[]{0x01, 0x00};
        cmd.typeValue = GattCommand.CommandType.WRITE_NO_RESPONSE;
        sendCommand(cmd);
    }*/

    private void clear() {
        this.processing.set(false);
        this.stopMonitoringRssi();
        this.cancelCommandTimeoutTask();
        this.mInputCommandQueue.clear();
        this.mOutputCommandQueue.clear();
        this.mNotificationCallbacks.clear();
        this.mDelayHandler.removeCallbacksAndMessages(null);
    }

    public boolean refreshCache() {
        TelinkLog.e("GattController#refreshCache");
        /*if (gatt != null) {
            gatt.discoverServices();
            return true;
        }
        return false;*/
        if (gatt == null) {
            TelinkLog.e("refresh error: gatt null");
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
            TelinkLog.e("An exception occured while refreshing device");
        }
        return false;
    }

    public void setCallback(DeviceCallback deviceCallback) {
        this.callback = deviceCallback;
    }


    protected void onNotify(UUID charUUID, byte[] data) {

        TelinkLog.w("onNotify -->" + Arrays.bytesToHexString(data, ":"));

        this.callback.onNotify(charUUID, data);
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

//        TelinkLog.w("postCommand");

        this.mInputCommandQueue.add(commandContext);
        synchronized (this.mProcessLock) {
            if (!this.processing.get()) {
                this.processCommand();
            }
        }
        if (!this.processing.get()) {
            this.processCommand();
        }
    }

    private void processCommand() {

//        TelinkLog.w("processing : " + this.processing);

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
            /*this.mOutputCommandQueue.add(commandContext);

            if (!this.processing.get())
                this.processing.set(true);*/

            this.processing.set(true);
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
        /*int delay = commandContext.gattCommand.delay;
        if (delay > 0) {
            this.mDelayHandler.postDelayed(this.mCommandDelayRunnable, delay);
        } else {
            this.processCommand(commandContext);
        }*/
    }

    synchronized private void processCommand(CommandContext commandContext) {
        GattCommand gattCommand = commandContext.gattCommand;
        GattCommand.CommandType commandType = gattCommand.type;

        TelinkLog.v("processCommand : " + gattCommand.toString());

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
                this.writeCCCForPv(commandContext, gattCommand.serviceUUID, gattCommand.characteristicUUID, gattCommand.descriptorUUID, gattCommand.data);
                break;
            case ENABLE_NOTIFY:
                this.enableNotification();
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
        if (this.callback != null)
            this.callback.onConnected();
    }

    protected void onDisconnect() {
        serviceRefreshed = false;
        if (this.callback != null)
            this.callback.onDisconnected();
    }


    protected void onServicesDiscovered(List<BluetoothGattService> services) {

        if (serviceRefreshed) {
            enableNotification();
            enableProxyNotification();
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

        TelinkLog.w("Start OTA");
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
            callback.onOtaStateChanged(STATE_SUCCESS);
        }
    }

    protected void onOtaFailure() {
        if (callback != null) {
            callback.onOtaStateChanged(STATE_FAILURE);
        }
    }

    protected void onOtaProgress() {
        if (callback != null) {
            callback.onOtaStateChanged(STATE_PROGRESS);
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

    // OTA 开始时发送的命令
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
         * 发送read指令
         */
        int sectionSize = 16 * 8;
        int sendTotal = this.mOtaParser.getNextPacketIndex() * 16;
        TelinkLog.i("ota onCommandSampled byte length : " + sendTotal);
        if (sendTotal > 0 && sendTotal % sectionSize == 0) {
            TelinkLog.i("onCommandSampled ota read packet " + mOtaParser.getNextPacketIndex());
            GattCommand cmd = GattCommand.newInstance();
            cmd.serviceUUID = UuidInfo.SERVICE_UUID_OTA;
            cmd.characteristicUUID = UuidInfo.CHARACTERISTIC_UUID_OTA;
            cmd.type = GattCommand.CommandType.READ;
            cmd.tag = TAG_OTA_READ;
            cmd.flag = FLAG_OTA;
            readCnt++;
            TelinkLog.d("cur read count: " + readCnt);
            this.sendCommand(cmd);
            return true;
        }
        return false;
    }

    /********************************************************************************
     * OTA API end
     *******************************************************************************/


    private void commandCompleted() {

        TelinkLog.w("commandCompleted");


        synchronized (this.mProcessLock) {
            if (this.processing.get())
                this.processing.set(false);
            this.processCommand();
        }

    }

    private void commandSuccess(CommandContext commandContext, byte[] data) {
        TelinkLog.v("commandSuccess");
        this.lastTime = System.currentTimeMillis();
        if (commandContext != null) {
            GattCommand gattCommand = commandContext.gattCommand;
            if (this.callback != null) {
                this.callback.onCommandSuccess(gattCommand.characteristicUUID, data);
            }
            if (gattCommand.flag == FLAG_OTA) {
                onOTACmdSuccess(gattCommand, data);
            }
//            TelinkLog.w("commandSuccess -- " + Arrays.bytesToHexString(gattCommand.data, ":"));
            commandContext.clear();
        }
    }

    private void onOTACmdSuccess(GattCommand gattCommand, Object data) {

        if (gattCommand.tag.equals(TAG_OTA_PRE_READ)) {
            TelinkLog.w("read =========> " + Arrays.bytesToHexString((byte[]) data, "-"));
        } else if (gattCommand.tag.equals(TAG_OTA_PREPARE)) {
            sendOtaStartCommand();
        } else if (gattCommand.tag.equals(TAG_OTA_START)) {
            sendNextOtaPacketCommand(0);
        } else if (gattCommand.tag.equals(TAG_OTA_END)) {
            setOtaProgressChanged();
            resetOta();
            onOtaSuccess();
        } else if (gattCommand.tag.equals(TAG_OTA_LAST)) {
            sendOtaEndCommand();
        } else if (gattCommand.tag.equals(TAG_OTA_WRITE)) {

            if (!validateOta()) {
                sendNextOtaPacketCommand(0);
            }
            setOtaProgressChanged();
        } else if (gattCommand.tag.equals(TAG_OTA_READ)) {
            sendNextOtaPacketCommand(0);
        }


    }

    private void commandSuccess(byte[] data) {
        CommandContext commandContext;
        commandContext = this.mOutputCommandQueue.poll();
        this.commandSuccess(commandContext, data);
    }

    private void commandError(CommandContext commandContext, String errorMsg) {

        TelinkLog.w("commandError:" + errorMsg);
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
        TelinkLog.w("commandTimeout");
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


    private void writeCCCForPv(CommandContext commandContext,
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

    public void enableProxyNotification() {

        boolean success = true;
        String errorMsg = "";

        BluetoothGattService service = this.gatt.getService(UuidInfo.PROXY_SERVICE_UUID);

        if (service != null) {

            BluetoothGattCharacteristic characteristic = this
                    .findNotifyCharacteristic(service, UuidInfo.PROXY_OUT_CHARACTERISTIC_UUID);

            if (characteristic != null) {

                if (!this.gatt.setCharacteristicNotification(characteristic,
                        true)) {
                    success = false;
                    errorMsg = "enable notification error";
                } else {
                    String key = this.generateHashKey(UuidInfo.PROXY_SERVICE_UUID,
                            characteristic);
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
//            this.commandError(commandContext, errorMsg);
        } else {
            this.onEnableNotify();
        }

//        this.commandCompleted();
    }

    public void enableNotification() {

        boolean success = true;
        String errorMsg = "";

        BluetoothGattService service = this.gatt.getService(UuidInfo.PROVISION_SERVICE_UUID);

        if (service != null) {

            BluetoothGattCharacteristic characteristic = this
                    .findNotifyCharacteristic(service, UuidInfo.PB_OUT_CHARACTERISTIC_UUID);

            if (characteristic != null) {

                if (!this.gatt.setCharacteristicNotification(characteristic,
                        true)) {
                    success = false;
                    errorMsg = "enable notification error";
                } else {
                    String key = this.generateHashKey(UuidInfo.PROVISION_SERVICE_UUID,
                            characteristic);
//                    this.mNotificationCallbacks.put(key, commandContext);
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
//            this.commandError(commandContext, errorMsg);
        } else {
            this.onEnableNotify();
        }

//        this.commandCompleted();
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
        TelinkLog.w("onConnectionStateChange  status :" + status + " state : "
                + newState);

        if (newState == BluetoothGatt.STATE_CONNECTED && status == BluetoothGatt.GATT_SUCCESS) {

            this.mConnState.set(CONN_STATE_CONNECTED);

            if (this.gatt == null || !this.gatt.discoverServices()) {
                TelinkLog.w("remote service discovery has been stopped status = "
                        + newState);
                this.disconnect();
            } else {
//                this.gatt.requestConnectionPriority(BluetoothGatt.CONNECTION_PRIORITY_HIGH);
                this.onConnect();
            }

        } else {

            TelinkLog.w("Close");

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

        TelinkLog.v("onCharacteristicWrite newStatus : " + status);

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
            TelinkLog.w("Service discovery failed");
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

        TelinkLog.w("mtu changed : " + mtu);
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
}
