/********************************************************************************************************
 * @file CertDetailActivity.java
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
package com.telink.ble.mesh.ui.cdtp;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattServer;
import android.bluetooth.BluetoothGattServerCallback;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.TextView;

import androidx.annotation.NonNull;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.ble.BleAdvertiser;
import com.telink.ble.mesh.core.ble.UUIDInfo;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.MeshNetKey;
import com.telink.ble.mesh.model.json.MeshStorageService;
import com.telink.ble.mesh.ui.BaseActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;

/**
 * Transfer json files to other phones using CDTP protocol
 */
public class CdtpExportToPhoneActivity extends BaseActivity {

    private byte[] jsonData;
    private BluetoothAdapter bluetoothAdapter;
    private BluetoothGattServer bluetoothGattServer;
    private BleAdvertiser advertiser;
    private TextView tv_log;
    private BluetoothServerSocket serverSocket;
    private BluetoothSocket bleSocket;
    private static final int MSG_APPEND_LOG = 1;

    private static final int MSG_COMPLETE = 2;

    private int psm = 0x25;

    @SuppressLint("HandlerLeak")
    private Handler msgHandler = new Handler() {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            if (msg.what == MSG_APPEND_LOG) {
                tv_log.append((String) msg.obj);
                tv_log.append("\n");
            } else if (msg.what == MSG_COMPLETE) {
                showTipDialog("success", "Mesh JSON send complete", (dialog, which) -> {
                    dialog.dismiss();
                    finish();
                });
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_cdtp_export_to_phone);
        setTitle("CDTP Export", "To Phone");
        enableBackNav(true);
        tv_log = findViewById(R.id.tv_log);
        initService();
        appendLog("init complete");
        MeshService.getInstance().idle(false);
        startAdvertising();
        initData();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        advertiser.stopAdvertising();
        msgHandler.removeCallbacksAndMessages(null);
        List<BluetoothDevice> devices;
        try {
            devices = bluetoothGattServer.getConnectedDevices();
        } catch (UnsupportedOperationException e) {
            BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
            devices = bluetoothManager.getConnectedDevices(BluetoothProfile.GATT_SERVER);
        }

        if (bluetoothGattServer != null) {
            for (BluetoothDevice device : devices) {
                bluetoothGattServer.cancelConnection(device);
            }
            bluetoothGattServer.close();
        }
        if (serverSocket != null) {
            try {
                serverSocket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private void initData() {
        int[] selectedIndexes = getIntent().getIntArrayExtra("selectedIndexes");
        if (selectedIndexes == null) return;
        MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();

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
    }

    private void initService() {
        advertiser = new BleAdvertiser();
        initBluetooth();
        createGattService();
        createDeviceSocket();
    }

    private void appendLog(String log) {
        MeshLogger.d(log);
        msgHandler.obtainMessage(MSG_APPEND_LOG, log).sendToTarget();
    }


    // 初始化蓝牙适配器和 GATT 服务器
    private void initBluetooth() {
        BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        bluetoothAdapter = bluetoothManager.getAdapter();
        try {
            Method localMethod = bluetoothManager.getClass().
                    getMethod("openGattServer", new Class[]{Context.class, BluetoothGattServerCallback.class, int.class});
            bluetoothGattServer = (BluetoothGattServer) localMethod.invoke(bluetoothManager, this, gattServerCallback, BluetoothDevice.TRANSPORT_LE);
            appendLog("init gatt server complete");
        } catch (NoSuchMethodException | IllegalAccessException | InvocationTargetException e) {
            e.printStackTrace();
            appendLog("init gatt server err");
            bluetoothGattServer = bluetoothManager.openGattServer(this, gattServerCallback);
        }
    }


    // 在 GATT 服务器中创建 Service 和 Characteristic
    private void createGattService() {
        BluetoothGattService service = new BluetoothGattService(UUIDInfo.SERVICE_OTS, BluetoothGattService.SERVICE_TYPE_PRIMARY);

        // ots feature
        BluetoothGattCharacteristic ftChar = new BluetoothGattCharacteristic(UUIDInfo.CHARACTERISTIC_OTS_FEATURE,
                BluetoothGattCharacteristic.PROPERTY_READ | BluetoothGattCharacteristic.PROPERTY_WRITE,
                BluetoothGattCharacteristic.PERMISSION_READ | BluetoothGattCharacteristic.PERMISSION_WRITE);
        service.addCharacteristic(ftChar);


        BluetoothGattCharacteristic oacpChar = new BluetoothGattCharacteristic(UUIDInfo.CHARACTERISTIC_OACP,
                BluetoothGattCharacteristic.PROPERTY_READ | BluetoothGattCharacteristic.PROPERTY_WRITE
                        | BluetoothGattCharacteristic.PROPERTY_NOTIFY,
                BluetoothGattCharacteristic.PERMISSION_READ_ENCRYPTED | BluetoothGattCharacteristic.PERMISSION_WRITE_ENCRYPTED);

        BluetoothGattDescriptor cccDesc = new BluetoothGattDescriptor(UUIDInfo.DESCRIPTOR_CFG_UUID,
                BluetoothGattDescriptor.PERMISSION_WRITE_ENCRYPTED | BluetoothGattDescriptor.PERMISSION_READ_ENCRYPTED);
        oacpChar.addDescriptor(cccDesc);
        service.addCharacteristic(oacpChar);


        BluetoothGattCharacteristic objectSizeChar = new BluetoothGattCharacteristic(UUIDInfo.CHARACTERISTIC_OBJECT_SIZE,
                BluetoothGattCharacteristic.PROPERTY_READ | BluetoothGattCharacteristic.PROPERTY_WRITE,
                BluetoothGattCharacteristic.PERMISSION_READ_ENCRYPTED | BluetoothGattCharacteristic.PERMISSION_WRITE_ENCRYPTED);
        service.addCharacteristic(objectSizeChar);

        BluetoothGattCharacteristic psmChar = new BluetoothGattCharacteristic(UUIDInfo.CHARACTERISTIC_IOS_PSM,
                BluetoothGattCharacteristic.PROPERTY_READ, BluetoothGattCharacteristic.PERMISSION_READ);
        service.addCharacteristic(psmChar);


        // 将 Service 添加到 GATT 服务器中
        bluetoothGattServer.addService(service);
    }


    private void createDeviceSocket() {
        new Thread(() -> {
            try {
                serverSocket = bluetoothAdapter.listenUsingL2capChannel();
                psm = serverSocket.getPsm();
                appendLog("psm : " + psm);
                this.bleSocket = serverSocket.accept();
                appendLog("socket establish success");
            } catch (IOException e) {
                e.printStackTrace();
            }

//            try {
//
//                ContextUtil.skipReflectWarning();
//                Constructor<BluetoothServerSocket> construct = BluetoothServerSocket.class.getDeclaredConstructor(
//                        int.class,
//                        boolean.class,
//                        boolean.class, int.class);
//                construct.setAccessible(true);
//                // 0x25
//                serverSocket = construct.newInstance(4, true, true, AppSettings.PSM); // BluetoothSocket.TYPE_L2CAP_LE=4
//
//                // int errno = socket.mSocket.bindListen();
//                Field targetField = BluetoothServerSocket.class.getDeclaredField("mSocket");
//                targetField.setAccessible(true);
//                BluetoothSocket bluetoothSocket = (BluetoothSocket) targetField.get(serverSocket);
//                MeshLogger.d("bluetoothSocket - " + bluetoothSocket);
//
//                @SuppressLint({"DiscouragedPrivateApi", "SoonBlockedPrivateApi"})
//                Method bindListenM = BluetoothSocket.class.getDeclaredMethod("bindListen");
//                bindListenM.setAccessible(true);
//                bindListenM.invoke(bluetoothSocket);
//                appendLog(String.format(Locale.getDefault(), "socket listening (psm: %d)... ", serverSocket.getPsm()));
//                this.bleSocket = serverSocket.accept();
//                appendLog("socket establish success");
////                outputStream.close();
//                /*
//                socket.close();
//                socket = null;*/
//            } catch (IOException | NoSuchMethodError | InstantiationException | InvocationTargetException | IllegalAccessException | NoSuchFieldException | NoSuchMethodException e) {
//                e.printStackTrace();
//            }
        }).start();

    }

    private void writeData() {
        if (bleSocket != null) {
            try {
                OutputStream outputStream = bleSocket.getOutputStream();
                outputStream.write(jsonData);
                outputStream.flush();
                appendLog("data write over socket complete");
                msgHandler.obtainMessage(MSG_COMPLETE).sendToTarget();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }


    private BluetoothGattServerCallback gattServerCallback = new BluetoothGattServerCallback() {
        @Override
        public void onConnectionStateChange(BluetoothDevice device, int status, int newState) {
            // 处理连接状态变化
            MeshLogger.d("gattServerCallback#onConnectionStateChange#" + device.getName() + " -- " + device.getAddress());
            if (newState == BluetoothProfile.STATE_CONNECTED) {
//                advertiser.stopAdvertising();
//                handler.postDelayed(() -> device.createBond(), 2000);
                appendLog("gatt connect success - " + device.getAddress());
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                appendLog("gatt disconnected - " + device.getAddress());
            }
        }

        @Override
        public void onCharacteristicReadRequest(BluetoothDevice device, int requestId, int offset,
                                                BluetoothGattCharacteristic characteristic) {
            MeshLogger.d("gattServerCallback#onCharacteristicReadRequest - " + characteristic.getUuid().toString());
            if (characteristic.getUuid().equals(UUIDInfo.CHARACTERISTIC_OBJECT_SIZE)) {
                appendLog("receive read request - Object Size - " + device.getAddress());
                byte[] size = ByteBuffer.allocate(8).order(ByteOrder.LITTLE_ENDIAN).putInt(jsonData.length).putInt(10 * 1024 * 1024).array();
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, 0, size);
            } else if (characteristic.getUuid().equals(UUIDInfo.CHARACTERISTIC_OTS_FEATURE)) {
                int oacpFeature = 0b00111111; // Create, Delete, Calculate Checksum, Execute, Read, Write
                // olcp not suppport
                int olcpFeature = 0;
                byte[] featuresData = ByteBuffer.allocate(8).order(ByteOrder.LITTLE_ENDIAN).putInt(oacpFeature).putInt(olcpFeature).array();
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, 0, featuresData);
            } else if (characteristic.getUuid().equals(UUIDInfo.CHARACTERISTIC_IOS_PSM)) {
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, 0, MeshUtils.integer2Bytes(psm, 2, ByteOrder.LITTLE_ENDIAN));
            } else {
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, 0, new byte[]{0});
            }
        }

        @Override
        public void onCharacteristicWriteRequest(BluetoothDevice device, int requestId,
                                                 BluetoothGattCharacteristic characteristic,
                                                 boolean preparedWrite, boolean responseNeeded,
                                                 int offset, byte[] value) {
            MeshLogger.d("gattServerCallback#onCharacteristicWriteRequest");
            if (responseNeeded) {
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, 0, null);
            }
            if (characteristic.getUuid().equals(UUIDInfo.CHARACTERISTIC_OACP)) {
                byte op = value[0];
                appendLog("receive oacp request : " + op);
                if (op == OacpOpcode.CALCULATE_CHECKSUM.code) {
                    appendLog("CALCULATE_CHECKSUM");
                    int crcOffset = MeshUtils.bytes2Integer(value, 1, 4, ByteOrder.LITTLE_ENDIAN);
                    int crcLen = MeshUtils.bytes2Integer(value, 5, 4, ByteOrder.LITTLE_ENDIAN);
                    int crc = MeshUtils.crc32(jsonData, crcOffset, crcLen);
                    byte[] crcData = MeshUtils.integer2Bytes(crc, 4, ByteOrder.LITTLE_ENDIAN);
                    byte result = OacpResultCode.SUCCESS.code;
                    characteristic.setValue(generateOacpResult(op, result, crcData));
                    bluetoothGattServer.notifyCharacteristicChanged(device, characteristic, false);
                } else if (op == OacpOpcode.READ.code) {
//                    byte[] lenData = MeshUtils.integer2Bytes(jsonData.length, 4, ByteOrder.LITTLE_ENDIAN);
                    byte result = OacpResultCode.SUCCESS.code;
                    characteristic.setValue(generateOacpResult(op, result, null));
                    bluetoothGattServer.notifyCharacteristicChanged(device, characteristic, false);
                    writeData();
                }
            }
        }

        @Override
        public void onDescriptorWriteRequest(BluetoothDevice device, int requestId, BluetoothGattDescriptor descriptor, boolean preparedWrite, boolean responseNeeded, int offset, byte[] value) {
            super.onDescriptorWriteRequest(device, requestId, descriptor, preparedWrite, responseNeeded, offset, value);
            MeshLogger.d("gattServerCallback#onDescriptorWriteRequest - " + Arrays.bytesToHexString(value));
            appendLog("onDescriptorWriteRequest - " + Arrays.bytesToHexString(value) + " responseNeeded - " + responseNeeded);
            advertiser.stopAdvertising();
            if (responseNeeded) {
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, 0, null);
            }

        }
    };

    private byte[] generateOacpResult(byte requestOp, byte resultCode, byte[] data) {
        if (data == null) {
            return new byte[]{
                    OacpOpcode.RESPONSE.code, requestOp, resultCode,
            };
        }
        return ByteBuffer.allocate(3 + data.length).put(OacpOpcode.RESPONSE.code)
                .put(requestOp).put(resultCode).put(data).array();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    private void startAdvertising() {
        appendLog("start advertising - service uuid - " + UUIDInfo.SERVICE_OTS.toString());
        advertiser.startAdvertise(MeshUtils.OTS_UUID, 600 * 1000);
    }

}
