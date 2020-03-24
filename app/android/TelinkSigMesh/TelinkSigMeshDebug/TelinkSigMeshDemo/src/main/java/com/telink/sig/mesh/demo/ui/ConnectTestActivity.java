/********************************************************************************************************
 * @file     ConnectTestActivity.java 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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
package com.telink.sig.mesh.demo.ui;

import android.bluetooth.BluetoothGattService;
import android.os.Bundle;
import android.view.View;

import com.telink.sig.mesh.ble.AdvertisingDevice;
import com.telink.sig.mesh.ble.Device;
import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.util.TelinkLog;

import java.util.List;
import java.util.UUID;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Created by kee on 2018/6/6.
 */

public class ConnectTestActivity extends BaseActivity {
    Device device;
    private static final String TAG = "CONNECT_TEST: ";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_connect_test);
        AdvertisingDevice advertisingDevice = (AdvertisingDevice) getIntent().getParcelableExtra("device");
//        device = new Device(advertisingDevice.device,advertisingDevice.scanRecord, advertisingDevice.rssi);
        device.setCallback(new Device.DeviceCallback() {
            @Override
            public void onConnected() {
                TelinkLog.d(TAG + "onConnected");
            }

            @Override
            public void onDisconnected() {
                TelinkLog.d(TAG + "onDisconnected");

            }

            @Override
            public void onServicesDiscovered(List<BluetoothGattService> services) {
                TelinkLog.d(TAG + "onServicesDiscovered");
            }

            @Override
            public void onNotifyEnable() {
            }

            @Override
            public void onNotify(UUID charUUID, byte[] data) {

            }

            @Override
            public void onCommandError(String errorMsg) {

            }

            @Override
            public void onOtaStateChanged(int state) {

            }
        });
    }


    public void connect(View view) {
//        device.connect(this);
    }

    public void disconnect(View view) {
        device.disconnect();
    }

    private AtomicInteger cnt = new AtomicInteger(0);
    public void cmdTest(View view){
        new Thread(new Runnable() {
            @Override
            public void run() {
                while (true){
                    try {
                        Thread.sleep(5);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    TelinkLog.d("cmd cnt: " + cnt.incrementAndGet());
                    device.writeCCCForPx();
                }
            }
        }, "thread_____1").start();

        new Thread(new Runnable() {
            @Override
            public void run() {
                while (true){
                    try {
                        Thread.sleep(5);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    TelinkLog.d("cmd cnt: " + cnt.incrementAndGet());
                    device.writeCCCForPx();
                }
            }
        }, "thread_____2").start();
    }
}
