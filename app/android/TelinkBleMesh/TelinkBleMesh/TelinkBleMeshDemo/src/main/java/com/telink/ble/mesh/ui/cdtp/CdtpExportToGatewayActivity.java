package com.telink.ble.mesh.ui.cdtp;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.os.ParcelUuid;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.ble.GattConnection;
import com.telink.ble.mesh.core.ble.GattRequest;
import com.telink.ble.mesh.core.ble.UUIDInfo;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.MeshNetKey;
import com.telink.ble.mesh.model.db.MeshInfoService;
import com.telink.ble.mesh.model.json.MeshStorageService;
import com.telink.ble.mesh.ui.BaseActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.ContextUtil;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.Constructor;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

/**
 * export json to gateway using cdtp protocol
 */
public class CdtpExportToGatewayActivity extends BaseActivity {

    private static final int TAG_READ_OBJ_SIZE = 1;

    private static final int TAG_WRITE_OACP = 2;

    private static final int TAG_CHECKSUM = 3;

    private static final int REQUEST_CODE_SELECT_DEVICE = 1;
    private byte[] jsonData;
    private GattConnection device;
    private static final int MSG_APPEND_LOG = 1;
    private static final int MSG_CONNECTION = 2;
    private static final int MSG_COMPLETE = 3;
    private BluetoothDevice bluetoothDevice = null;
    private TextView tv_log, tv_device_info;
    private BluetoothSocket socket;
    // cardview
    private View cv_connection;
    private Button btn_start, btn_select_device, btn_connect_gatt;
    private Handler socketHandler;
    private HandlerThread socketThread;
    private MeshInfo meshInfo;
    private boolean transferComplete = false;

    @SuppressLint("HandlerLeak")
    private Handler msgHandler = new Handler() {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            if (msg.what == MSG_APPEND_LOG) {
                tv_log.append((String) msg.obj);
                tv_log.append("\n");
            } else if (msg.what == MSG_CONNECTION) {
                int connSt = (int) msg.obj;
                boolean isConnected = connSt == BluetoothGatt.STATE_CONNECTED;
                btn_start.setEnabled(isConnected);
                if (isConnected) {
                    appendLog("connect success");
                } else if (connSt == BluetoothGatt.STATE_DISCONNECTED) {
                    appendLog("device disconnected");
                }

                String desc = MeshUtils.getGattConnectionDesc(connSt);
                btn_connect_gatt.setText(desc);
            } else if (msg.what == MSG_COMPLETE) {
                showCompleteDialog((String) msg.obj);
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cdtp_import);
        initView();
        initDevice();
        initData();
    }

    private void initView() {
        tv_log = findViewById(R.id.tv_log);
        cv_connection = findViewById(R.id.cv_connection);
        cv_connection.setVisibility(View.GONE);
        tv_device_info = findViewById(R.id.tv_device_info);
        btn_connect_gatt = findViewById(R.id.btn_connect_gatt);
        btn_connect_gatt.setOnClickListener(v -> {
            if (bluetoothDevice == null) {
                toastMsg("no device selected");
                return;
            }
            if (device.isConnected()) {
                updateConnectionState(BluetoothGatt.STATE_DISCONNECTING);
                device.disconnect();
            } else if (device.isDisconnected()) {
                updateConnectionState(BluetoothGatt.STATE_CONNECTING);
                device.connect(bluetoothDevice);
            }
        });
        btn_select_device = findViewById(R.id.btn_select_device);
        btn_select_device.setOnClickListener(v -> {
            startActivityForResult(new Intent(this, CdtpServerListActivity.class), REQUEST_CODE_SELECT_DEVICE);
        });
        btn_start = findViewById(R.id.btn_start);
        btn_start.setOnClickListener(v -> {
            if (device.isConnected()) {
//                start();
                checkDeviceBondState();
            } else {
                toastMsg("please connect to the server");
            }
        });
        enableBackNav(true);
        setTitle("CDTP Export", "To Gateway");
    }

    private void initData() {
        int[] selectedIndexes = getIntent().getIntArrayExtra("selectedIndexes");
        if (selectedIndexes == null) return;
        long meshId = getIntent().getLongExtra("MeshInfoId", 0);
        meshInfo = MeshInfoService.getInstance().getById(meshId);
        List<MeshNetKey> meshNetKeyList = new ArrayList<>();
        outer:
        for (MeshNetKey netKey : meshInfo.meshNetKeyList) {
            for (int idx : selectedIndexes) {
                if (idx == netKey.index) {
                    meshNetKeyList.add(netKey);
                    continue outer;
                }
            }
        }
        String jsonStr = MeshStorageService.getInstance().meshToJsonString(meshInfo, meshNetKeyList);
//        MeshLogger.d("json string: " + jsonStr);
        byte[] jsonRaw = jsonStr.getBytes();
        MeshLogger.d("json raw - " + Arrays.bytesToHexString(jsonRaw));
        MeshLogger.d("json raw length - " + jsonRaw.length);
        jsonData = CdtpEncoder.compress(jsonRaw, false);
        MeshLogger.d("json data length compressed - " + jsonData.length);

        addTestData();

        appendLog("json data length: " + jsonData.length);
    }

    private void addTestData() {
        ByteBuffer bf = ByteBuffer.allocate(jsonData.length * 100 + 33);
        for (int i = 0; i < 100; i++) {
            bf.put(jsonData);
        }
        jsonData = bf.array();
        MeshLogger.d("json data length compressed - (*100) - " + jsonData.length);
    }


    private void initDevice() {
        device = new GattConnection(this, null);
        device.setConnectionCallback(mDeviceStateCallback);
    }

    /**
     * device(server) selected from {@link CdtpServerListActivity}
     */
    private void onDeviceSelect() {
        cv_connection.setVisibility(View.VISIBLE);
        btn_select_device.setVisibility(View.GONE);
        tv_device_info.setText(bluetoothDevice.getAddress());
        updateConnectionState(BluetoothGatt.STATE_CONNECTING);
        device.connect(bluetoothDevice);
    }


    private void updateConnectionState(int newState) {
        msgHandler.obtainMessage(MSG_CONNECTION, newState).sendToTarget();
    }

    private void appendLog(String log) {
        MeshLogger.d(log);
        msgHandler.obtainMessage(MSG_APPEND_LOG, log).sendToTarget();
    }

    private void showCompleteDialog(String jsonData) {
        msgHandler.removeCallbacks(flowTimeout);
        showTipDialog("success", "export to gateway complete", (dialog, which) -> finish());
    }

    // 反射调用
    private void createSocket() {
        new Thread(() -> {
            try {
                ContextUtil.skipReflectWarning();
                Constructor<BluetoothSocket> construct = BluetoothSocket.class.getDeclaredConstructor(
                        int.class,
                        int.class, boolean.class,
                        boolean.class, BluetoothDevice.class, int.class, ParcelUuid.class);
                construct.setAccessible(true);
                // BluetoothSocket.TYPE_L2CAP_LE 4
                socket = construct.newInstance(4, -1, true, true, bluetoothDevice, 0x25, null);
                socket.connect();
                appendLog("socket establish success");
                OutputStream outputStream = socket.getOutputStream();


                int offset = 0;
                byte[] data;
                int round = (jsonData.length + 99) / 100;
                MeshLogger.d("length : " + jsonData.length);
                MeshLogger.d("round : " + round);
                for (int i = 0; i < round; i++) {
                    if (i == round - 1) {
                        data = ByteBuffer.allocate(jsonData.length - offset).put(jsonData, offset, jsonData.length - offset).array();
                    } else {
                        data = ByteBuffer.allocate(100).put(jsonData, offset, 100).array();
                    }
                    MeshLogger.d("write data: " + Arrays.bytesToHexString(data) + " -- len: " + data.length);
                    outputStream.write(data);
//                    outputStream.flush();
                    offset += data.length;
                    MeshLogger.d("offset: " + offset);
                    Thread.sleep(30);
                }

//                outputStream.write(jsonData);
//                outputStream.flush();
                onJsonDataSendComplete();
            } catch (Exception e) {
                e.printStackTrace();
                if (!transferComplete) {
                    onError("socket error: " + e.getMessage());
                }
            }
        }).start();
    }


    private void checkDeviceBondState() {
        boolean bond = bluetoothDevice.getBondState() == BluetoothDevice.BOND_BONDED;
        if (!bond) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("Warning");
            builder.setMessage("Device Not Bond, please check device state; click [start] to start transfer, click [bond] to cancel action");
            builder.setPositiveButton("start", (dialog, which) -> start());
            builder.setNegativeButton("bond", (dialog, which) -> bluetoothDevice.createBond());
            builder.show();
        } else {
            start();
        }
    }

    private void start() {
//        1. enable notify
//        2. read object size
//        3. open socket
        btn_start.setEnabled(false);
        msgHandler.postDelayed(flowTimeout, 5 * 60 * 1000);
        enableOacpCcc();
        msgHandler.postDelayed(this::readObjectSize, 2 * 1000);
    }

    private void enableOacpCcc() {
        MeshLogger.d("enabel oacp ccc");
        UUID serviceUUID = UUIDInfo.SERVICE_OTS;
        UUID characteristicUUID = UUIDInfo.CHARACTERISTIC_OACP;
        GattRequest cmd = GattRequest.newInstance();
        cmd.serviceUUID = serviceUUID;
        cmd.characteristicUUID = characteristicUUID;
        cmd.type = GattRequest.RequestType.ENABLE_NOTIFY;
        device.sendRequest(cmd);

        GattRequest cccCmd = GattRequest.newInstance();
        cccCmd.serviceUUID = serviceUUID;
        cccCmd.characteristicUUID = characteristicUUID;
        cccCmd.descriptorUUID = UUIDInfo.DESCRIPTOR_CFG_UUID;
        cccCmd.data = new byte[]{0x02, 0x00};
        cccCmd.type = GattRequest.RequestType.WRITE_DESCRIPTOR;
        device.sendRequest(cccCmd);
    }

    // read object size
    private void readObjectSize() {
        GattRequest command = new GattRequest();
        command.serviceUUID = UUIDInfo.SERVICE_OTS;
        command.characteristicUUID = UUIDInfo.CHARACTERISTIC_OBJECT_SIZE;
        command.type = GattRequest.RequestType.READ;
        command.tag = TAG_READ_OBJ_SIZE;
        command.callback = GATT_REQUEST_CB;
        device.sendRequest(command);
    }

    private void oacpWrite() {
        GattRequest command = new GattRequest();
        command.serviceUUID = UUIDInfo.SERVICE_OTS;
        command.characteristicUUID = UUIDInfo.CHARACTERISTIC_OACP;
        command.type = GattRequest.RequestType.WRITE;
        int offset = 0;
        byte mode = 0;
        command.data = ByteBuffer.allocate(10).order(ByteOrder.LITTLE_ENDIAN)
                .put(OacpOpcode.WRITE.code)
                .putInt(offset)
                .putInt(jsonData.length).put(mode).array();
        command.tag = TAG_WRITE_OACP;
        command.callback = GATT_REQUEST_CB;
        device.sendRequest(command);
    }


    private void sendChecksum() {
        GattRequest command = new GattRequest();
        command.serviceUUID = UUIDInfo.SERVICE_OTS;
        command.characteristicUUID = UUIDInfo.CHARACTERISTIC_OACP;
        command.type = GattRequest.RequestType.WRITE;
        int crc = MeshUtils.crc32(jsonData);
        command.data = ByteBuffer.allocate(13).order(ByteOrder.LITTLE_ENDIAN).put(OacpOpcode.CALCULATE_CHECKSUM.code)
                .putInt(0)
                .putInt(jsonData.length)
                .putInt(crc)
                .array();
        command.tag = TAG_CHECKSUM;
        command.callback = GATT_REQUEST_CB;
        device.sendRequest(command);
    }

    private void onJsonDataSendComplete() {
        msgHandler.postDelayed(this::sendChecksum, 2 * 1000);
    }

    private Runnable flowTimeout = () -> onError("flow timeout");

    @Override
    protected void onDestroy() {
        super.onDestroy();
        transferComplete = true;
        if (device != null) {
            device.disconnect();
        }

        if (socketHandler != null) {
            socketHandler.removeCallbacksAndMessages(null);
            socketHandler = null;
        }

        if (socketThread != null) {
            socketThread.quitSafely();
            socketThread = null;
        }

        if (socket != null) {
            try {
                socket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public GattConnection.ConnectionCallback mDeviceStateCallback = new GattConnection.ConnectionCallback() {

        @Override
        public void onConnected() {

        }

        @Override
        public void onDisconnected() {
            updateConnectionState(BluetoothGatt.STATE_DISCONNECTED);
        }

        @Override
        public void onServicesDiscovered(List<BluetoothGattService> services) {
            updateConnectionState(BluetoothGatt.STATE_CONNECTED);
        }

        @Override
        public void onNotify(UUID serviceUUID, UUID charUUID, byte[] data) {
            appendLog("notify - " + Arrays.bytesToHexString(data) + " - " + charUUID.toString());
            if (charUUID.equals(UUIDInfo.CHARACTERISTIC_OACP)) {
                byte opcode = data[0];
                if (opcode != OacpOpcode.RESPONSE.code) {
                    return;
                }
                byte requestCode = data[1];
                if (requestCode == OacpOpcode.CALCULATE_CHECKSUM.code) {
                    byte resultCode = data[2];
                    appendLog("CALCULATE_CHECKSUM response: " + resultCode);
                    if (resultCode == OacpResultCode.SUCCESS.code) {
                        int checksum = MeshUtils.bytes2Integer(data, 3, 4, ByteOrder.LITTLE_ENDIAN);
                        int local = MeshUtils.crc32(jsonData);
                        if (checksum == local) {
                            msgHandler.obtainMessage(MSG_COMPLETE).sendToTarget();
                        } else {
                            onError("checksum error");
                        }
                    }
                }

            }
        }
    };

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (REQUEST_CODE_SELECT_DEVICE == requestCode && resultCode == RESULT_OK && data != null) {
            bluetoothDevice = data.getParcelableExtra("device");
            onDeviceSelect();
        }
    }

    private void onError(String info) {
        if (transferComplete) return;
        appendLog("ERROR: " + info);
        msgHandler.removeCallbacks(flowTimeout);
        transferComplete = true;
        runOnUiThread(() -> showTipDialog("ERROR", "transfer error: " + info, (dialog, which) -> finish()));
    }

    private final GattRequest.Callback GATT_REQUEST_CB = new GattRequest.Callback() {

        @Override
        public void success(GattRequest request, Object obj) {
            byte[] data = (byte[]) obj;
            if (request.tag == null) return;
            if (request.tag.equals(TAG_READ_OBJ_SIZE)) {
                // read size, Current Size[4] + Allocated Size[4]
                int allocObjSize = MeshUtils.bytes2Integer(data, 4, 4, ByteOrder.LITTLE_ENDIAN);
                appendLog("obj size received: " + allocObjSize);
                if (allocObjSize < jsonData.length) {
                    onError("gateway object size is not enough");
                    return;
                }
                oacpWrite();
            } else if (request.tag.equals(TAG_WRITE_OACP)) {
                createSocket();
            }
        }

        @Override
        public void error(GattRequest request, String errorMsg) {
            onError("request error: " + request.tag);
        }

        @Override
        public boolean timeout(GattRequest request) {
            onError("request timeout: " + request.tag);
            return false;
        }
    };

}
