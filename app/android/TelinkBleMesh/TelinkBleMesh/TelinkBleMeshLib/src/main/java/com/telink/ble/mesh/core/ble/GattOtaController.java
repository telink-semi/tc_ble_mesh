/********************************************************************************************************
 * @file GattOtaController.java
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
package com.telink.ble.mesh.core.ble;

import android.os.Build;
import android.os.Handler;

import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.util.OtaPacketParser;

/**
 * Update direct connected device for update,
 * based on gatt connection {@link GattConnection}
 */
public class GattOtaController {

    private final String LOG_TAG = "GATT-OTA";

    public static final int OTA_STATE_FAILURE = 0;
    public static final int OTA_STATE_SUCCESS = 1;
    public static final int OTA_STATE_PROGRESS = 2;

    protected Handler mTimeoutHandler;

    private static final int OTA_PREPARE = 0xFF00;
    private static final int OTA_START = 0xFF01;
    private static final int OTA_END = 0xFF02;

    private static final int TAG_OTA_WRITE = 1;
    private static final int TAG_OTA_READ = 2;
    private static final int TAG_OTA_LAST = 3;

    private static final int TAG_OTA_PREPARE = 6; // prepare
    private static final int TAG_OTA_START = 7; // start
    private static final int TAG_OTA_END = 8; // end

    private GattConnection mConnection;

    private GattOtaCallback mCallback;

    private final OtaPacketParser mOtaParser = new OtaPacketParser();

    private int readCnt = 0;


    private static final int DEFAULT_READ_INTERVAL = 8;

    private int readInterval = DEFAULT_READ_INTERVAL;


    public GattOtaController(GattConnection gattConnection) {
        mTimeoutHandler = new Handler();
        mConnection = gattConnection;
    }

    public void setCallback(GattOtaCallback callback) {
        this.mCallback = callback;
    }

    public void begin(byte[] firmware) {
        begin(firmware, DEFAULT_READ_INTERVAL);
    }


    public void begin(byte[] firmware, int readInterval) {
        log("Start OTA");
        this.clear();
        this.mOtaParser.set(firmware);
        this.readInterval = readInterval;
        this.sendOTAPrepareCommand();
    }

    private void clear() {
        this.readCnt = 0;
        this.mOtaParser.clear();
    }

    private void sendRequest(GattRequest request) {
        if (mConnection != null) {
            mConnection.sendRequest(request);
        }
    }


    public void otaWriteData(byte[] data, int tag) {
        GattRequest cmd = GattRequest.newInstance();
        cmd.characteristicUUID = UUIDInfo.CHARACTERISTIC_UUID_OTA;
        cmd.serviceUUID = UUIDInfo.SERVICE_UUID_OTA;
        cmd.data = data.clone();
        cmd.tag = tag;
        cmd.type = GattRequest.RequestType.WRITE_NO_RESPONSE;
        cmd.callback = this.mOtaRequestCallback;
        sendRequest(cmd);
    }


    public int getOtaProgress() {
        return this.mOtaParser.getProgress();
    }


    private void setOtaProgressChanged() {
        if (this.mOtaParser.invalidateProgress()) {
            onOtaProgress();
        }
    }

    private void onOtaSuccess() {
        if (mCallback != null) {
            mCallback.onOtaStateChanged(OTA_STATE_SUCCESS);
        }
    }

    private void onOtaFailure() {
        if (mCallback != null) {
            mCallback.onOtaStateChanged(OTA_STATE_FAILURE);
        }
    }

    private void onOtaProgress() {
        if (mCallback != null) {
            mCallback.onOtaStateChanged(OTA_STATE_PROGRESS);
        }
    }

    private void sendOTAPrepareCommand() {
        otaWriteData(new byte[]{OTA_PREPARE & 0xFF, (byte) (OTA_PREPARE >> 8 & 0xFF)}, TAG_OTA_PREPARE);
    }

    // send ota start command
    private void sendOtaStartCommand() {
        otaWriteData(new byte[]{OTA_START & 0xFF, (byte) (OTA_START >> 8 & 0xFF)}, TAG_OTA_START);
    }

    private void sendOtaEndCommand() {
        int index = mOtaParser.getIndex();
        byte[] data = new byte[6];
        data[0] = OTA_END & 0xFF;
        data[1] = (byte) ((OTA_END >> 8) & 0xFF);
        data[2] = (byte) (index & 0xFF);
        data[3] = (byte) (index >> 8 & 0xFF);
        data[4] = (byte) (~index & 0xFF);
        data[5] = (byte) (~index >> 8 & 0xFF);
        otaWriteData(data, TAG_OTA_END);
    }


    private void sendNextOtaPacketCommand() {
        if (this.mOtaParser.hasNextPacket()) {
            byte[] data = this.mOtaParser.getNextPacket();
            int tag = this.mOtaParser.isLast() ? TAG_OTA_LAST : TAG_OTA_WRITE;
            otaWriteData(data, tag);

        }
    }

    private boolean validateOta() {

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) return false;

        if (readInterval <= 0) return false;

        /*
         * read
         */
        int sectionSize = 16 * readInterval;
        int sendTotal = this.mOtaParser.getNextPacketIndex() * 16;
//        logMessage("ota onCommandSampled byte length : " + sendTotal);
        if (sendTotal > 0 && sendTotal % sectionSize == 0) {
            log("onCommandSampled ota read packet " + mOtaParser.getNextPacketIndex(), MeshLogger.LEVEL_VERBOSE);
            GattRequest cmd = GattRequest.newInstance();
            cmd.serviceUUID = UUIDInfo.SERVICE_UUID_OTA;
            cmd.characteristicUUID = UUIDInfo.CHARACTERISTIC_UUID_OTA;
            cmd.type = GattRequest.RequestType.READ;
            cmd.tag = TAG_OTA_READ;
            cmd.callback = mOtaRequestCallback;
            readCnt++;
            log("cur read count: " + readCnt);
            this.sendRequest(cmd);
            return true;
        }
        return false;
    }


    private void onOTACmdSuccess(GattRequest command, Object data) {

        if (command.tag.equals(TAG_OTA_PREPARE)) {
            sendOtaStartCommand();
        } else if (command.tag.equals(TAG_OTA_START)) {
            sendNextOtaPacketCommand();
        } else if (command.tag.equals(TAG_OTA_END)) {
            setOtaProgressChanged();
            clear();
            onOtaSuccess();
        } else if (command.tag.equals(TAG_OTA_LAST)) {
            sendOtaEndCommand();
        } else if (command.tag.equals(TAG_OTA_WRITE)) {
            if (!validateOta()) {
                sendNextOtaPacketCommand();
            }
            setOtaProgressChanged();
        } else if (command.tag.equals(TAG_OTA_READ)) {
            sendNextOtaPacketCommand();
        }


    }

    private void onOtaCmdError(GattRequest request) {
        if (request.tag.equals(TAG_OTA_END)) {
            // ota success
            setOtaProgressChanged();
            clear();
            onOtaSuccess();
        } else {
            clear();
            onOtaFailure();
        }
    }

    private void onOtaCmdTimeout(GattRequest request) {
        if (request.tag.equals(TAG_OTA_END)) {
            // ota success
            setOtaProgressChanged();
            clear();
            onOtaSuccess();
        } else {
            clear();
            onOtaFailure();
        }
    }

    private GattRequest.Callback mOtaRequestCallback = new GattRequest.Callback() {
        @Override
        public void success(GattRequest request, Object obj) {
            onOTACmdSuccess(request, obj);
        }

        @Override
        public void error(GattRequest request, String errorMsg) {
            onOtaCmdError(request);
        }

        @Override
        public boolean timeout(GattRequest request) {
            onOtaCmdTimeout(request);
            return false;
        }
    };


    public interface GattOtaCallback {
        void onOtaStateChanged(int state);
    }

    private void log(String logMessage) {
        log(logMessage, MeshLogger.LEVEL_DEBUG);
    }

    private void log(String logMessage, int level) {
        MeshLogger.log(logMessage, LOG_TAG, level);
    }

}
