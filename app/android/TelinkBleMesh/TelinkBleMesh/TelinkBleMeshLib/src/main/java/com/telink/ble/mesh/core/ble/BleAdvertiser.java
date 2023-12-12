/********************************************************************************************************
 * @file BleAdvertiser.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2021
 *
 * @par Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.le.AdvertiseCallback;
import android.bluetooth.le.AdvertiseData;
import android.bluetooth.le.AdvertiseSettings;
import android.bluetooth.le.AdvertisingSetParameters;
import android.bluetooth.le.BluetoothLeAdvertiser;
import android.os.Handler;
import android.os.ParcelUuid;

import com.telink.ble.mesh.util.MeshLogger;

/**
 * send ble broadcast
 */
public class BleAdvertiser {

    public static final int CODE_ADV_START_FAIL = 0x06;

    /**
     * default advertise period, unit: ms
     */
    private static final long MIN_ADV_TIMEOUT = 2 * 1000; // ms

    private AdvertiserCallback advertiserCallback;

    /**
     * is advertising
     */
    private boolean isAdvertising = false;

    private Handler mDelayHandler = new Handler();

    BluetoothLeAdvertiser advertiser;

    AdvertisingSetParameters parameters;

    AdvertiseSettings advertiseSettings;

    private final Runnable ADV_TIMEOUT_TASK = new Runnable() {
        @Override
        public void run() {
            MeshLogger.d("stop by timer ");
            if (advertiserCallback != null) {
                advertiserCallback.onAdvertiseTimeout();
            }
            stopAdvertising();
        }
    };


    public BleAdvertiser() {
        advertiser = BluetoothAdapter.getDefaultAdapter().getBluetoothLeAdvertiser();
        AdvertiseSettings.Builder settingsBuilder = new AdvertiseSettings.Builder();
        settingsBuilder.setAdvertiseMode(AdvertiseSettings.ADVERTISE_MODE_LOW_LATENCY);
        settingsBuilder.setTxPowerLevel(AdvertiseSettings.ADVERTISE_TX_POWER_HIGH);
        settingsBuilder.setConnectable(false);
        settingsBuilder.setTimeout(0);
        advertiseSettings = settingsBuilder.build();
    }

    public void setAdvertiserCallback(AdvertiserCallback advertiserCallback) {
        this.advertiserCallback = advertiserCallback;
    }

    public void resetAdvertiser() {
        advertiser = BluetoothAdapter.getDefaultAdapter().getBluetoothLeAdvertiser();
    }

    public void startAdvertise(ParcelUuid uuid, long timeout) {
        if (advertiser == null) {
            MeshLogger.d("advertiser null");
            return;
        }
        if (timeout <= MIN_ADV_TIMEOUT) {
            timeout = MIN_ADV_TIMEOUT;
        }
//        timeout = 100000;
        mDelayHandler.removeCallbacks(ADV_TIMEOUT_TASK);
        mDelayHandler.postDelayed(ADV_TIMEOUT_TASK, timeout);
        AdvertiseData advData = buildAdvertiseData(uuid);
        advertiser.stopAdvertising(ADV_CB);
        advertiser.startAdvertising(advertiseSettings, advData, ADV_CB);
    }


    public void startAdvertise(ParcelUuid uuid, byte[] data, long timeout) {
        if (advertiser == null) {
            MeshLogger.d("advertiser null");
            return;
        }
        if (timeout <= MIN_ADV_TIMEOUT) {
            timeout = MIN_ADV_TIMEOUT;
        }
        mDelayHandler.removeCallbacks(ADV_TIMEOUT_TASK);
        mDelayHandler.postDelayed(ADV_TIMEOUT_TASK, timeout);
        AdvertiseData advData = buildAdvertiseData(uuid, data);
        advertiser.stopAdvertising(ADV_CB);
        advertiser.startAdvertising(advertiseSettings, advData, ADV_CB);
    }


    private AdvertiseData buildAdvertiseData(ParcelUuid uuid) {
        AdvertiseData.Builder dataBuilder = new AdvertiseData.Builder();
        BluetoothAdapter.getDefaultAdapter().setName("tlk");
        dataBuilder.setIncludeDeviceName(true);
//        dataBuilder.addManufacturerData(APPLE_CID, data);
        dataBuilder.addServiceUuid(uuid);
        return dataBuilder.build();
    }


    private AdvertiseData buildAdvertiseData(ParcelUuid uuid, byte[] data) {
        AdvertiseData.Builder dataBuilder = new AdvertiseData.Builder();
//        BluetoothAdapter.getDefaultAdapter().setName("tlk");
//        dataBuilder.setIncludeDeviceName(true);
//        dataBuilder.addServiceUuid(uuid);
        dataBuilder.addServiceData(uuid, data);
        return dataBuilder.build();
    }

    private final AdvertiseCallback ADV_CB = new AdvertiseCallback() {
        @Override
        public void onStartSuccess(AdvertiseSettings settingsInEffect) {
            super.onStartSuccess(settingsInEffect);
            MeshLogger.d("adv start success");
            if (advertiserCallback != null) {
                advertiserCallback.onStartedAdvertise();
            }
        }

        @Override
        public void onStartFailure(int errorCode) {
            super.onStartFailure(errorCode);
            MeshLogger.d("adv start fail:" + errorCode);
        }
    };

    public void clear() {
        if (advertiser != null) {
            stopAdvertising();
        }
        if (mDelayHandler != null) {
            mDelayHandler.removeCallbacksAndMessages(null);
        }
    }

    public synchronized void stopAdvertising() {

        /*if (!isAdvertising) {
            return;
        }*/
        MeshLogger.d("stop advertising");
        advertiser.stopAdvertising(ADV_CB);
        isAdvertising = false;
        if (advertiserCallback != null) {
            advertiserCallback.onStoppedAdvertise();
        }
    }


    public interface AdvertiserCallback {

        void onAdvertiseFail(int errorCode);

        void onStartedAdvertise();

        void onStoppedAdvertise();

        void onAdvertiseTimeout();
    }
}
