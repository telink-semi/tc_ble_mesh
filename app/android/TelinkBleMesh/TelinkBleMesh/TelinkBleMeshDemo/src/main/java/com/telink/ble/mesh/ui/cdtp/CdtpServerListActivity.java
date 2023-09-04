package com.telink.ble.mesh.ui.cdtp;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.os.Bundle;
import android.os.Handler;
import android.view.MenuItem;

import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.ble.MeshScanRecord;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.AdvertisingDevice;
import com.telink.ble.mesh.ui.BaseActivity;
import com.telink.ble.mesh.ui.adapter.CdtpServerListAdapter;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.ArrayList;
import java.util.List;

public class CdtpServerListActivity extends BaseActivity {
    private CdtpServerListAdapter listAdapter;
    private List<AdvertisingDevice> deviceList = new ArrayList<>();
    private final Handler mScanHandler = new Handler();
    private BluetoothAdapter mBluetoothAdapter;
    private final static long SCAN_PERIOD = 10 * 1000;
    private boolean mScanning = false;
    private MenuItem refreshItem;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cdtp_server_list);
        RecyclerView rv_servers = findViewById(R.id.rv_servers);
        Toolbar toolbar = findViewById(R.id.title_bar);
        enableBackNav(true);
        setTitle("Server List", "OTS");
        toolbar.inflateMenu(R.menu.menu_server_scan);
        refreshItem = toolbar.getMenu().findItem(R.id.item_refresh);
        toolbar.setOnMenuItemClickListener(item -> {
            scanToggle(!mScanning);
            return true;
        });
        deviceList = new ArrayList<>();
        listAdapter = new CdtpServerListAdapter(this, deviceList);
        rv_servers.setLayoutManager(new LinearLayoutManager(this));
        rv_servers.setAdapter(listAdapter);
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        scanToggle(true);
    }

    private void scanToggle(final boolean enable) {
        mScanHandler.removeCallbacks(scanTask);
        if (enable) {
            MeshLogger.i("ADV#startScan");
            mScanning = true;
            deviceList.clear();
            listAdapter.notifyDataSetChanged();

            mBluetoothAdapter.startLeScan(scanCallback);
            mScanHandler.postDelayed(scanTask, SCAN_PERIOD);
        } else {
            MeshLogger.i("ADV#stopScan");
            mScanning = false;
            mBluetoothAdapter.stopLeScan(scanCallback);
        }
        refreshItem.setIcon(mScanning ? R.drawable.ic_stop : R.drawable.ic_refresh);
    }


    @Override
    public void onDestroy() {
        super.onDestroy();
        if (mScanning && mBluetoothAdapter != null && mBluetoothAdapter.isEnabled()) {
            mBluetoothAdapter.stopLeScan(scanCallback);
        }
    }

    private BluetoothAdapter.LeScanCallback scanCallback = new BluetoothAdapter.LeScanCallback() {
        @Override
        public void onLeScan(final BluetoothDevice device, final int rssi, final byte[] scanRecord) {
            MeshLogger.w("scan:" + device.getName() + " mac:" + device.getAddress() + " rssi:" + rssi + " record:  " + Arrays.bytesToHexString(scanRecord, ":"));
            MeshScanRecord meshScanRecord = MeshScanRecord.parseFromBytes(scanRecord);
            if (meshScanRecord == null || meshScanRecord.getServiceUuids() == null || !meshScanRecord.getServiceUuids().contains(MeshUtils.OTS_UUID)) {
                return;
            }
//            if (!device.getAddress().toUpperCase().contains("FF:FF:BB:CC:DD")) return;
//            if (device.getName() == null || !device.getName().contains("k")) return;
            for (AdvertisingDevice advDevice : deviceList) {
                if (device.getAddress().equals(advDevice.device.getAddress())) {
                    return;
                }
            }

            deviceList.add(new AdvertisingDevice(device, rssi, scanRecord));
            listAdapter.notifyItemInserted(deviceList.size() - 1);
        }
    };

    private Runnable scanTask = new Runnable() {
        @Override
        public void run() {
            if (mBluetoothAdapter != null && mBluetoothAdapter.isEnabled()) {
                scanToggle(false);
            }
        }
    };
}
