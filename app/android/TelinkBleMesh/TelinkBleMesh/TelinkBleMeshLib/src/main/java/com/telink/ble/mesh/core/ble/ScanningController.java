package com.telink.ble.mesh.core.ble;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.os.Handler;
import android.os.HandlerThread;

import java.util.concurrent.atomic.AtomicBoolean;

/**
 * Created by kee on 2019/9/9.
 */

public class ScanningController {

    enum ScannerVersion {
        DEFAULT,
        LOLLIPOP,

    }

    public static final int SCAN_FAILED_FEATURE_UNSUPPORTED = 4;
    private Context mContext;
    private Handler mDelayHandler = new Handler();
    private BluetoothAdapter mAdapter = null;
    private ScanCallback mScanCallback = null;
    private AtomicBoolean isScanning = new AtomicBoolean(false);

    private BluetoothAdapter.LeScanCallback mLeScanCallback;
    private LeScanFilter mScanFilter;
    private LeScanSetting mScanSetting;

    private long lastScanStartTime = 0;

    public ScanningController(Context mContext, HandlerThread handlerThread) {
        if (mDelayHandler == null) {
            mDelayHandler = new Handler(handlerThread.getLooper());
        }
        this.mContext = mContext;
        this.mAdapter = BluetoothAdapter.getDefaultAdapter();
        this.mLeScanCallback = new BluetoothAdapter.LeScanCallback() {

            @Override
            public void onLeScan(BluetoothDevice device, int rssi, byte[] scanRecord) {
                if (mScanCallback != null) {
                    if (mScanFilter != null) {
                        LeScanFilter filter = mScanFilter;
                        if (filter.macExclude != null && filter.macExclude.length != 0) {
                            for (String mac : filter.macExclude) {
                                if (mac.toUpperCase().equals(device.getAddress().toUpperCase())) {
                                    return;
                                }
                            }
                        }

                        boolean isTarget = true;
                        if (filter.macInclude != null && filter.macInclude.length != 0) {
                            isTarget = false;
                            for (String mac : filter.macInclude) {
                                if (mac.toUpperCase().equals(device.getAddress().toUpperCase())) {
                                    isTarget = true;
                                    break;
                                }
                            }
                        }
                        if (isTarget) {
                            mScanCallback.onLeScan(device, rssi, scanRecord);
                        }
                    } else {
                        mScanCallback.onLeScan(device, rssi, scanRecord);
                    }

                }

            }
        };
    }

    public interface ScanCallback {
        void onLeScan(BluetoothDevice device, int rssi, byte[] scanRecord);

        void onScanFail(int errorCode);

        void onStartedScan();

        void onStoppedScan();

        void onScanTimeout();
    }


    public void setScanCallback(ScanCallback mScanCallback) {
        this.mScanCallback = mScanCallback;
    }

    public void startScan(LeScanFilter leScanFilter, LeScanSetting leScanSetting) {
        mDelayHandler.removeCallbacksAndMessages(null);
        if (!isEnabled()) {
            return;
        }
        mScanFilter = leScanFilter;
        mScanSetting = leScanSetting;
        if (isScanning.get()) {
            mAdapter.stopLeScan(mLeScanCallback);
        }

        long scanDelay = 0;

        long scanSpacing = 0;
        if (leScanSetting != null) {
            scanSpacing = leScanSetting.spacing;
        }
        if (scanSpacing > 0 && System.currentTimeMillis() - lastScanStartTime < scanSpacing) {
            scanDelay = scanSpacing - (System.currentTimeMillis() - lastScanStartTime);
            if (scanDelay > scanSpacing) scanDelay = scanSpacing;
        }
        lastScanStartTime = System.currentTimeMillis();
//        mDelayHandler.removeCallbacks(startScanTask);
        mDelayHandler.postDelayed(startScanTask, scanDelay);

    }


    private Runnable startScanTask = new Runnable() {
        @Override
        public void run() {
            if (isEnabled()) {
                boolean scanResult = mAdapter.startLeScan(mScanFilter == null ? null : mScanFilter.uuidInclude, mLeScanCallback);
//                boolean scanResult = mAdapter.startLeScan(mLeScanCallback);
                if (scanResult) {
                    isScanning.set(true);
                    onScanStarted();
                } else {
                    isScanning.set(false);
                    mAdapter.stopLeScan(mLeScanCallback);
                    if (mScanCallback != null)
                        mScanCallback.onScanFail(SCAN_FAILED_FEATURE_UNSUPPORTED);
                }
            }
        }
    };

    private Runnable scanTimeoutTask = new Runnable() {
        @Override
        public void run() {
            stopScan();
            onScanTimeout();
        }
    };

    private void onScanStarted() {
        if (mScanCallback != null)
            mScanCallback.onStartedScan();

        if (mScanSetting != null && mScanSetting.during > 0) {
            mDelayHandler.postDelayed(scanTimeoutTask, mScanSetting.during);
        }
    }

    private void onScanTimeout() {
        if (mScanCallback != null) {
            mScanCallback.onScanTimeout();
        }
    }

    public void stopScan() {
        mDelayHandler.removeCallbacksAndMessages(null);
        if (!isEnabled()) {
            return;
        }
        if (isScanning.get()) {
            isScanning.set(false);
            mAdapter.stopLeScan(mLeScanCallback);
            if (mScanCallback != null) {
                mScanCallback.onStoppedScan();
            }
        }
    }

    public boolean isEnabled() {
        return this.mAdapter != null
                && this.mAdapter.isEnabled();
    }

    public void enableBluetooth() {
        if (this.mAdapter != null)
            this.mAdapter.enable();
    }

}
