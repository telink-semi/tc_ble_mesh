/********************************************************************************************************
 * @file     AdvDeviceListActivity.java 
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

import android.app.ActionBar;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.Toast;

import com.telink.sig.mesh.ble.AdvertisingDevice;
import com.telink.sig.mesh.ble.Device;
import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.ui.adapter.DeviceListAdapter;
import com.telink.sig.mesh.util.Arrays;
import com.telink.sig.mesh.util.TelinkLog;

import java.util.ArrayList;
import java.util.List;

/**
 * 广播设备列表页面
 * Created by Administrator on 2017/2/20.
 */
public class AdvDeviceListActivity extends BaseActivity {

    private ListView lv_devices;
    private DeviceListAdapter mListAdapter;
    private List<AdvertisingDevice> mDeviceList = new ArrayList<>();
    private final Handler mScanHandler = new Handler();
    private BluetoothAdapter mBluetoothAdapter;
    private final static long SCAN_PERIOD = 10 * 1000;
    private boolean mScanning = false;
    private Device device;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_adv_device_list);
        ActionBar actionBar = getActionBar();
        if (actionBar != null) {
            actionBar.setTitle("AdvDevices");
        }
        lv_devices = (ListView) findViewById(R.id.lv_devices);
        mListAdapter = new DeviceListAdapter(this, mDeviceList);
        lv_devices.setAdapter(mListAdapter);
        lv_devices.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {

                startActivity(new Intent(AdvDeviceListActivity.this, ConnectTestActivity.class)
                        .putExtra("device", mDeviceList.get(position)));
                if (mScanning) {
                    mScanning = false;
                    mBluetoothAdapter.stopLeScan(scanCallback);
                    invalidateOptionsMenu();
                }
            }
        });


        if (!isSupport(getApplicationContext())) {
            Toast.makeText(this, "ble not support", Toast.LENGTH_SHORT).show();
            this.finish();
            return;
        }

//        scanToggle(true);
    }

    private BluetoothAdapter.LeScanCallback scanCallback = new BluetoothAdapter.LeScanCallback() {
        @Override
        public void onLeScan(final BluetoothDevice device, final int rssi, final byte[] scanRecord) {
            TelinkLog.w("scan:" + device.getName() + " record:" + Arrays.bytesToHexString(scanRecord, ":"));

//            if (scanRecord.length < 7 || (scanRecord[5] != (byte) 0xd1 && scanRecord[5] != (byte) 0xd2) || scanRecord[6] != (byte) 0x7f)
//                return;

            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    for (AdvertisingDevice advertisingDevice : mDeviceList) {
                        if (device.getAddress().equals(advertisingDevice.device.getAddress())) return;
                    }
                    mDeviceList.add(new AdvertisingDevice(device, rssi, scanRecord));
                    mListAdapter.notifyDataSetChanged();
                }
            });
        }
    };


    @Override
    protected void onResume() {
        super.onResume();
        checkBleState();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mScanning && mBluetoothAdapter != null && mBluetoothAdapter.isEnabled()) {
            mBluetoothAdapter.stopLeScan(scanCallback);
        }
    }

    private void checkBleState() {
        if (this.mBluetoothAdapter != null
                && !this.mBluetoothAdapter.isEnabled()) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setMessage("Open Bluetooth!");
            builder.setCancelable(false);
            builder.setNeutralButton("cancel", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    finish();
                }
            });
            builder.setNegativeButton("enable", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    enable(getApplicationContext());
                }
            });
            builder.show();
        }
    }

    public boolean isSupport(Context context) {
        return this.getAdapter(context) != null;
    }

    public boolean enable(Context context) {
        BluetoothAdapter mAdapter = getAdapter(context);
        if (mAdapter == null)
            return false;
        if (mAdapter.isEnabled())
            return true;
        return mBluetoothAdapter.enable();
    }

    public BluetoothAdapter getAdapter(Context context) {
        synchronized (this) {
            if (mBluetoothAdapter == null) {
                BluetoothManager manager = (BluetoothManager) context
                        .getSystemService(Context.BLUETOOTH_SERVICE);
                this.mBluetoothAdapter = manager.getAdapter();
            }
        }

        return this.mBluetoothAdapter;
    }

    private Runnable scanTask = new Runnable() {
        @Override
        public void run() {
            if (mBluetoothAdapter != null && mBluetoothAdapter.isEnabled()) {
                TelinkLog.i("ADV#stopScan");
                mScanning = false;
                mBluetoothAdapter.stopLeScan(scanCallback);
                invalidateOptionsMenu();
            }
        }
    };

    private int scanDelay = 0 * 1000;


    public int compareVersion(String lightVersion, String newVersion) {
        if (lightVersion == null || newVersion == null) {
            return 0;
        }
        int compareResult = newVersion.compareTo(lightVersion);
        return compareResult > 1 ? 1 : compareResult;
    }

    private void scanToggle(final boolean enable) {

//        int i = compareVersion("V1.0", "V1.9");
//        TelinkLog.d("compare:" + i);
//        startActivity(new Intent(this, MainActivity.class));

        /*lastClock = meshLib.clockTime();

        TelinkLog.d("0x lastClock:" + Integer.toHexString(lastClock));

        mScanHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                int thisClock = meshLib.clockTime();
                TelinkLog.d("0x thisClock:" + Integer.toHexString(thisClock));
                int tmp = thisClock - lastClock;
                TelinkLog.d("clock tmp 100ms:" + (Integer.toHexString(tmp)));
                int per = ((tmp) /32)*1000;
                TelinkLog.d("clock period 100ms:" + ((per)));

            }
        }, 100);
*/

        /*TelinkLog.w("millTime:" + System.currentTimeMillis() + "  nanoTime:" + System.nanoTime());
//Integer.MAX_VALUE
        TelinkLog.w("clock:" + meshLib.getClock());*/
        mScanHandler.removeCallbacks(scanTask);
        if (enable) {
            TelinkLog.i("ADV#startScan");
            mScanning = true;
            mDeviceList.clear();
            mListAdapter.notifyDataSetChanged();
            mScanHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    mBluetoothAdapter.startLeScan(scanCallback);
                    mScanHandler.postDelayed(scanTask, SCAN_PERIOD);
                }
            }, scanDelay);
        } else {
            TelinkLog.i("ADV#scanToggle#stopScan");
            mScanning = false;
            mBluetoothAdapter.stopLeScan(scanCallback);
        }
        invalidateOptionsMenu();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.adv, menu);
        if (!mScanning) {
            menu.findItem(R.id.menu_state).setTitle(R.string.menu_scan);
            menu.findItem(R.id.menu_refresh).setActionView(null);
//            menu.findItem(R.id.menu_refresh).setVisible(false);
        } else {
            menu.findItem(R.id.menu_state).setTitle(R.string.menu_stop);
//            menu.findItem(R.id.menu_refresh).setVisible(true);
            menu.findItem(R.id.menu_refresh).setActionView(
                    R.layout.actionbar_indeterminate_progress);

        }
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.menu_state:
                scanToggle(!mScanning);

                break;

        }
        return super.onOptionsItemSelected(item);
    }

    public void refresh(View view) {

        scanToggle(true);
        /*BluetoothManager manager = (BluetoothManager) this
                .getSystemService(Context.BLUETOOTH_SERVICE);
        List<BluetoothDevice> devices = manager.getConnectedDevices(BluetoothProfile.GATT);
        Toast.makeText(this, "cur connected devices:" + devices.size(), Toast.LENGTH_SHORT).show();*/

    }
}
