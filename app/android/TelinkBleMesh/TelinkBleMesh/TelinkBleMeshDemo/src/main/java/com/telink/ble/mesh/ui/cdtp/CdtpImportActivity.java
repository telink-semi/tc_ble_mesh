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
import com.telink.ble.mesh.model.json.MeshStorageService;
import com.telink.ble.mesh.ui.BaseActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.ContextUtil;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Constructor;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.List;
import java.util.UUID;

public class CdtpImportActivity extends BaseActivity {

    private static final int TAG_READ_OBJ_SIZE = 1;

    private static final int TAG_READ_PSM = 2;
    private static final int TAG_READ_OACP = 3;

    private static final int REQUEST_CODE_SELECT_DEVICE = 1;
    private GattConnection device;
    private static final int MSG_APPEND_LOG = 1;
    private static final int MSG_CONNECTION = 2;
    private static final int MSG_COMPLETE = 3;
    private BluetoothDevice bluetoothDevice = null;
    private TextView tv_log, tv_device_info, tv_device_name;
    private BluetoothSocket socket;
    // cardview
    private View cv_connection;
    int objSize = 0;
    private Button btn_start, btn_select_device, btn_connect_gatt;
    private Handler socketHandler;
    private HandlerThread socketThread;

    // data received from other phone
    private byte[] jsonData;
    private boolean transferComplete = false;
    int psm = 0x25;

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
                byte[] decompressedData = CdtpEncoder.decompress(jsonData);
//                appendLog("data decompressed: " + Arrays.bytesToHexString(decompressedData));
                showCompleteDialog(new String(decompressedData));
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cdtp_import);
        initView();
        initDevice();
    }

    private void initView() {
        tv_log = findViewById(R.id.tv_log);
        cv_connection = findViewById(R.id.cv_connection);
        cv_connection.setVisibility(View.GONE);
        tv_device_name = findViewById(R.id.tv_device_name);
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
                start();
            } else {
                toastMsg("please connect to the server");
            }
        });
        enableBackNav(true);
        setTitle("CDTP Import");
    }

    private void initDevice() {
        device = new GattConnection(this, null);
        device.setConnectionCallback(mDeviceStateCallback);
    }

    private void onDeviceSelect() {
        cv_connection.setVisibility(View.VISIBLE);
        btn_select_device.setVisibility(View.GONE);
        tv_device_info.setText(bluetoothDevice.getAddress());
        tv_device_name.setText("" + bluetoothDevice.getName());

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
        transferComplete = true;
        msgHandler.removeCallbacks(flowTimeout);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setCancelable(false);
        builder.setTitle("Warning");
        builder.setMessage("Mesh JSON receive complete, import data?");
        builder.setPositiveButton("Confirm", (dialog, which) -> {

            if (MeshStorageService.getInstance().importExternal(jsonData, this)) {
                dialog.dismiss();
                setResult(RESULT_OK);
                finish();
            }

            /*MeshInfo localMesh = TelinkMeshApplication.getInstance().getMeshInfo();
            MeshInfo newMesh = null;
            try {
                newMesh = MeshStorageService.getInstance().importExternal(jsonData, localMesh);
            } catch (Exception e) {
                e.printStackTrace();
            }
            if (newMesh == null) {
                toastMsg("import fail");
                appendLog("import json fail");
                return;
            }
            newMesh.saveOrUpdate();
            MeshService.getInstance().idle(true);
            TelinkMeshApplication.getInstance().setupMesh(newMesh);
            MeshService.getInstance().setupMeshNetwork(newMesh.convertToConfiguration());
            appendLog("Mesh storage import success, back to home page to reconnect\n");
            toastMsg("import success");
            dialog.dismiss();
            setResult(RESULT_OK);
            finish();*/
        });
        builder.setNegativeButton("Cancel", (dialog, which) -> dialog.dismiss());
        builder.show();
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
                socket = construct.newInstance(4, -1, true, true, bluetoothDevice, psm, null);
                socket.connect();
                appendLog("socket establish success");
                while (true) {
                    InputStream inputStream = socket.getInputStream();
                    int a = inputStream.available();
                    if (a != 0) {
                        MeshLogger.d("input stream available: " + a);
                        byte[] buffer = new byte[a];
                        inputStream.read(buffer);
                        MeshLogger.d("input stream buffer: " + Arrays.bytesToHexString(buffer, ":"));
                        appendLog("data received: " + Arrays.bytesToHexString(buffer, ""));
                        appendData(buffer);
                        appendLog("json data - " + jsonData.length + " -- " + objSize);
                        if (jsonData.length == objSize) {
                            onJsonDataReceiveComplete();
                            transferComplete = true;
                        }
//                        jsonData = buffer.clone();
//                        byte[] decompressedData = CdtpEncoder.decompress(buffer);
//                        appendLog("data decompressed: " + Arrays.bytesToHexString(decompressedData));
//                        msgHandler.obtainMessage(MSG_COMPLETE, new String(decompressedData)).sendToTarget();

                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
                if (!transferComplete) {
                    onError("socket error: " + e.getMessage());
                }
            }
        }).start();
    }

    private void appendData(byte[] data) {
        if (jsonData == null) {
            jsonData = data.clone();
            return;
        }
        jsonData = ByteBuffer.allocate(jsonData.length + data.length).put(jsonData).put(data).array();
    }

    private void onJsonDataReceiveComplete() {
        msgHandler.postDelayed(this::sendChecksum, 2 * 1000);
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
        device.sendRequest(command);
    }

    private void start() {
//        1. enable notify
//        2. read object size
//        3. open socket
        msgHandler.postDelayed(flowTimeout, 60 * 1000);
        enableOACP();
//        msgHandler.postDelayed(this::createSocket, 500);
        msgHandler.postDelayed(this::readPsm, 2 * 1000);
    }

    private void enableOACP() {
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
        command.tag = 1;
        command.callback = GATT_REQUEST_CB;
        device.sendRequest(command);
    }

    private void readOacp(int size) {
        GattRequest command = new GattRequest();
        command.serviceUUID = UUIDInfo.SERVICE_OTS;
        command.characteristicUUID = UUIDInfo.CHARACTERISTIC_OACP;
        command.type = GattRequest.RequestType.WRITE;
        int offset = 0;
        command.data = ByteBuffer.allocate(9).order(ByteOrder.LITTLE_ENDIAN).put(OacpOpcode.READ.code).putInt(offset)
                .putInt(size).array();
        command.tag = TAG_READ_OACP;
        command.callback = GATT_REQUEST_CB;
        device.sendRequest(command);
    }

    private void readPsm() {
        if (device.isPsmUuidSupport()) {
            appendLog("psm uuid supported");
            GattRequest command = new GattRequest();
            command.serviceUUID = UUIDInfo.SERVICE_OTS;
            command.characteristicUUID = UUIDInfo.CHARACTERISTIC_IOS_PSM;
            command.type = GattRequest.RequestType.READ;
            command.tag = TAG_READ_PSM;
            command.callback = GATT_REQUEST_CB;
            device.sendRequest(command);
        } else {
            appendLog("psm uuid not supported");
            onPsmConfirmed();
        }
    }

    private void onPsmConfirmed() {
        createSocket();
        readObjectSize();
    }

    private Runnable flowTimeout = () -> onError("flow timeout");


    @Override
    protected void onDestroy() {
        super.onDestroy();
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
                            onError("checksum error - " + checksum + " - " + local);
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
        appendLog("ERROR: " + info);
        msgHandler.removeCallbacks(flowTimeout);
        if (!transferComplete) {
            transferComplete = true;
            runOnUiThread(() -> showTipDialog("ERROR", "transfer error: " + info, (dialog, which) -> finish()));
        }
    }

    private final GattRequest.Callback GATT_REQUEST_CB = new GattRequest.Callback() {

        @Override
        public void success(GattRequest request, Object obj) {
            byte[] data = (byte[]) obj;
            if (request.tag == null) return;
            if (request.tag.equals(TAG_READ_OBJ_SIZE)) {
                // read size
                objSize = MeshUtils.bytes2Integer(data, ByteOrder.LITTLE_ENDIAN);
                appendLog("obj size received: " + objSize);
                readOacp(objSize);
            } else if (request.tag.equals(TAG_READ_PSM)) {
                psm = MeshUtils.bytes2Integer(data, ByteOrder.LITTLE_ENDIAN);
                appendLog("use psm - " + psm);
                onPsmConfirmed();
            } else if (request.tag.equals(TAG_READ_OACP)) {
                // do nothing
            }
        }

        @Override
        public void error(GattRequest request, String errorMsg) {
            onError("read object size error");
        }

        @Override
        public boolean timeout(GattRequest request) {
            onError("gatt request timeout - " + request.tag);
            return false;
        }
    };

}
