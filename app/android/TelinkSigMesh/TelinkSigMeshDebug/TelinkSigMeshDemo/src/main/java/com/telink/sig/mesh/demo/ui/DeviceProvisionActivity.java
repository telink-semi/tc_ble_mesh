/********************************************************************************************************
 * @file DeviceProvisionActivity.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.ParcelUuid;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.telink.sig.mesh.ble.AdvertisingDevice;
import com.telink.sig.mesh.ble.MeshScanRecord;
import com.telink.sig.mesh.ble.UnprovisionedDevice;
import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.SharedPreferenceHelper;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.model.Mesh;
import com.telink.sig.mesh.demo.model.PrivateDevice;
import com.telink.sig.mesh.demo.model.ProvisioningDevice;
import com.telink.sig.mesh.demo.ui.adapter.DeviceProvisionListAdapter;
import com.telink.sig.mesh.event.Event;
import com.telink.sig.mesh.event.EventListener;
import com.telink.sig.mesh.event.MeshEvent;
import com.telink.sig.mesh.event.NotificationEvent;
import com.telink.sig.mesh.event.ScanEvent;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.light.ProvisionDataGenerator;
import com.telink.sig.mesh.light.PublicationStatusParser;
import com.telink.sig.mesh.light.ScanParameters;
import com.telink.sig.mesh.light.UuidInfo;
import com.telink.sig.mesh.light.parameter.KeyBindParameters;
import com.telink.sig.mesh.light.parameter.ProvisionParameters;
import com.telink.sig.mesh.model.DeviceBindState;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.NodeInfo;
import com.telink.sig.mesh.model.NotificationInfo;
import com.telink.sig.mesh.model.PublishModel;
import com.telink.sig.mesh.model.SigMeshModel;
import com.telink.sig.mesh.model.message.config.PubSetMessage;
import com.telink.sig.mesh.util.TelinkLog;

import java.util.ArrayList;
import java.util.List;

/**
 * auto provision
 * actions:
 * 1.scan -- success ->
 * 2.connect -- success ->
 * 3.provision -- success ->
 * 4.KeyBind -- success ->
 * continue scan...
 * Created by kee on 2017/8/28.
 */

public class DeviceProvisionActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {

    private List<ProvisioningDevice> devices;

    private ProvisioningDevice pubSettingDevice;

    private DeviceProvisionListAdapter mListAdapter;
    private Button btn_back;
    private Mesh mesh;
    private MenuItem refreshItem;

    private UnprovisionedDevice targetDevice;

    private Handler delayedHandler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_device_provision);
        initTitle();
        RecyclerView rv_devices = findViewById(R.id.rv_devices);
        devices = new ArrayList<>();

        mListAdapter = new DeviceProvisionListAdapter(this, devices);
        rv_devices.setLayoutManager(new GridLayoutManager(this, 2));
        rv_devices.setAdapter(mListAdapter);
        btn_back = (Button) findViewById(R.id.btn_back);
        btn_back.setOnClickListener(this);
        findViewById(R.id.tv_log).setOnClickListener(this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_PROVISION_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_PROVISION_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_KEY_BIND_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_KEY_BIND_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(ScanEvent.SCAN_TIMEOUT, this);
        TelinkMeshApplication.getInstance().addEventListener(ScanEvent.DEVICE_FOUND, this);
        mesh = TelinkMeshApplication.getInstance().getMesh();

        if (mesh.pvIndex >= mesh.unicastRange.high) {
            Toast.makeText(getApplicationContext(), "no available address", Toast.LENGTH_SHORT).show();
            finish();
            return;
        }
        startScan();
    }

    private void initTitle() {
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.device_scan);
        setTitle("Device Scan");
//        toolbar.setSubtitle("provision -> bind");
        refreshItem = toolbar.getMenu().findItem(R.id.item_refresh);
        refreshItem.setVisible(false);
        toolbar.setNavigationIcon(null);

        toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                if (item.getItemId() == R.id.item_refresh) {
                    startScan();
                }
                return false;
            }
        });
    }


    private void startScan() {
        enableUI(false);

        ScanParameters parameters = ScanParameters.getDefault(false, true);

        if (devices.size() != 0) {
            String[] excludeMacs = new String[devices.size()];
            for (int i = 0; i < devices.size(); i++) {
                excludeMacs[i] = devices.get(i).macAddress;
            }
            parameters.setExcludeMacs(excludeMacs);
        }

        parameters.setScanTimeout(10 * 1000);
//        parameters.setIncludeMacs(new String[]{"FF:FF:BB:CC:DD:53"});
        MeshService.getInstance().startScan(parameters);
    }

    private void onDeviceFound(AdvertisingDevice device) {
        int address = mesh.pvIndex + 1;

//        int address = 0x6666;
        TelinkLog.d("alloc address: " + address);
        if (address == -1) {
            enableUI(true);
            return;
        }

        ProvisioningDevice pvDevice = new ProvisioningDevice();
        pvDevice.uuid = null;
        pvDevice.macAddress = device.device.getAddress();
        pvDevice.state = ProvisioningDevice.STATE_PROVISIONING;
        pvDevice.unicastAddress = address;
//        pvDevice.elementCnt = remote.elementCnt;
        pvDevice.failReason = null;
        devices.add(pvDevice);
        mListAdapter.notifyDataSetChanged();

        targetDevice = new UnprovisionedDevice(device, address);
//        devices.add(targetDevice);
        byte[] provisionData = ProvisionDataGenerator.getProvisionData(mesh.networkKey, mesh.netKeyIndex, mesh.ivUpdateFlag, mesh.ivIndex, address);
        ProvisionParameters parameters = ProvisionParameters.getDefault(provisionData, targetDevice);
        MeshService.getInstance().startProvision(parameters);
    }

    @Override
    public void finish() {
        super.finish();
        MeshService.getInstance().idle(true);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        delayedHandler.removeCallbacksAndMessages(null);
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_back:
                finish();
                break;
            case R.id.tv_log:
                startActivity(new Intent(this, LogActivity.class));
                break;
        }
    }

    private void enableUI(boolean enable) {
        btn_back.setEnabled(enable);
        refreshItem.setVisible(enable);
    }

    @Override
    public void performed(final Event<String> event) {
        super.performed(event);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {

                if (event.getType().equals(MeshEvent.EVENT_TYPE_PROVISION_SUCCESS)) {
                    onProvisionSuccess((MeshEvent) event);
                } else if (event.getType().equals(ScanEvent.SCAN_TIMEOUT)) {
                    enableUI(true);
                } else if (event.getType().equals(MeshEvent.EVENT_TYPE_PROVISION_FAIL)) {
                    onProvisionFail((MeshEvent) event);
                    startScan();
                } else if (event.getType().equals(MeshEvent.EVENT_TYPE_KEY_BIND_SUCCESS)) {
                    if (onKeyBindSuccess((MeshEvent) event)) {
                        TelinkLog.d("set device time publish");
                        // waiting for publication status
                    } else {
                        startScan();
                    }

                } else if (event.getType().equals(MeshEvent.EVENT_TYPE_KEY_BIND_FAIL)) {
                    onKeyBindFail((MeshEvent) event);
                    startScan();
                } else if (event.getType().equals(ScanEvent.DEVICE_FOUND)) {
                    AdvertisingDevice device = ((ScanEvent) event).advertisingDevice;
                    onDeviceFound(device);
                } else if (event.getType().equals(NotificationEvent.EVENT_TYPE_PUBLICATION_STATUS)) {
                    NotificationInfo notificationInfo = ((NotificationEvent) event).getNotificationInfo();
                    byte[] params = notificationInfo.params;
                    final PublicationStatusParser.StatusInfo statusInfo = PublicationStatusParser.create().parse(params);

                    ProvisioningDevice device = getDeviceByMeshAddress(notificationInfo.srcAdr);
                    if (device != null && statusInfo != null && statusInfo.status == 0) {
                        delayedHandler.removeCallbacks(pubSetTimeoutTask);
                        device.state = ProvisioningDevice.STATE_PUB_SET_SUCCESS;
                        mListAdapter.notifyDataSetChanged();
                        startScan();
                    }
                }
            }
        });

    }

    private Runnable pubSetTimeoutTask = new Runnable() {
        @Override
        public void run() {
            if (pubSettingDevice != null) {
                pubSettingDevice.state = ProvisioningDevice.STATE_PUB_SET_SUCCESS;
                mListAdapter.notifyDataSetChanged();
                startScan();
            }

        }
    };


    private void onProvisionFail(MeshEvent event) {
        DeviceInfo deviceInfo = event.getDeviceInfo();

        ProvisioningDevice pvDevice = getDeviceByMac(deviceInfo.macAddress);
        if (pvDevice == null) return;
        pvDevice.state = ProvisioningDevice.STATE_PROVISION_FAIL;
        pvDevice.unicastAddress = deviceInfo.meshAddress;
//        pvDevice.elementCnt = deviceInfo.elementCnt;
        pvDevice.failReason = null;
        mListAdapter.notifyDataSetChanged();

    }

    private void onProvisionSuccess(MeshEvent event) {

        DeviceInfo remote = event.getDeviceInfo();
        remote.bindState = DeviceBindState.BINDING;

        mesh.insertDevice(remote);
        mesh.pvIndex = remote.meshAddress + remote.elementCnt - 1;
        mesh.saveOrUpdate(DeviceProvisionActivity.this);


        ProvisioningDevice pvDevice = getDeviceByMac(remote.macAddress);
        if (pvDevice == null) return;
        pvDevice.uuid = null;
        pvDevice.macAddress = remote.macAddress;
        pvDevice.state = ProvisioningDevice.STATE_BINDING;
        pvDevice.unicastAddress = remote.meshAddress;
        pvDevice.elementCnt = remote.elementCnt;
        pvDevice.failReason = null;
        mListAdapter.notifyDataSetChanged();

        // check if private mode opened
        final boolean privateMode = SharedPreferenceHelper.isPrivateMode(this);

        // check if device support fast bind
        boolean fastBind = false;
        if (privateMode && targetDevice != null && targetDevice.scanRecord != null) {
            PrivateDevice device = getPrivateDevice(targetDevice.scanRecord);

            if (device != null) {
                TelinkLog.d("private device");
                NodeInfo nodeInfo = new NodeInfo();

                nodeInfo.nodeAdr = remote.meshAddress;
                nodeInfo.elementCnt = remote.elementCnt;
                nodeInfo.deviceKey = remote.deviceKey;
                final byte[] cpsData = device.getCpsData();
                nodeInfo.cpsData = NodeInfo.CompositionData.from(cpsData);
                nodeInfo.cpsDataLen = cpsData.length;
                remote.nodeInfo = nodeInfo;
                fastBind = true;
            } else {
                TelinkLog.d("private device null");
            }
        }

        KeyBindParameters parameters = KeyBindParameters.getDefault(remote,
                mesh.appKey, mesh.appKeyIndex, mesh.netKeyIndex, fastBind);

        MeshService.getInstance().startKeyBind(parameters);

    }


    private PrivateDevice getPrivateDevice(byte[] scanRecord) {
        if (scanRecord == null) return null;
        MeshScanRecord sr = MeshScanRecord.parseFromBytes(scanRecord);
        byte[] serviceData = sr.getServiceData(ParcelUuid.fromString(UuidInfo.PROVISION_SERVICE_UUID.toString()));
        if (serviceData == null) return null;
        return PrivateDevice.filter(serviceData);
    }


    /**
     * @return state saved
     */
    private boolean onKeyBindSuccess(MeshEvent event) {
        DeviceInfo remote = event.getDeviceInfo();
        DeviceInfo local = mesh.getDeviceByMacAddress(remote.macAddress);
        if (local == null) return false;

        ProvisioningDevice deviceInList = getDeviceByMac(remote.macAddress);
        if (deviceInList == null) return false;

        deviceInList.state = ProvisioningDevice.STATE_BIND_SUCCESS;
        deviceInList.nodeInfo = remote.nodeInfo;
        mListAdapter.notifyDataSetChanged();

        local.bindState = DeviceBindState.BOUND;
        local.boundModels = remote.boundModels;
        local.nodeInfo = remote.nodeInfo;
        mesh.saveOrUpdate(DeviceProvisionActivity.this);

        return setPublish(deviceInList, local);

    }

    private boolean setPublish(ProvisioningDevice provisioningDevice, DeviceInfo local) {
        int modelId = SigMeshModel.SIG_MD_TIME_S.modelId;
        int pubEleAdr = local.getTargetEleAdr(modelId);
        if (pubEleAdr != -1) {

            PublishModel pubModel = new PublishModel(pubEleAdr, modelId, 0xFFFF, 30 * 1000);
            final int appKeyIndex = TelinkMeshApplication.getInstance().getMesh().appKeyIndex;
            PubSetMessage pubSetMessage = PubSetMessage.createDefault(local.meshAddress,
                    pubModel.address, appKeyIndex, pubModel.period, pubModel.modelId, true);
            boolean result = MeshService.getInstance().setPublication(local.meshAddress, pubSetMessage, null);
            if (result) {
                pubSettingDevice = provisioningDevice;
                delayedHandler.postDelayed(pubSetTimeoutTask, 5 * 1000);
            }
            return result;
        } else {
            return false;
        }

    }

    private void onKeyBindFail(MeshEvent event) {
        DeviceInfo remote = event.getDeviceInfo();
        DeviceInfo local = mesh.getDeviceByMacAddress(remote.macAddress);
        if (local == null) return;

        ProvisioningDevice deviceInList = getDeviceByMac(remote.macAddress);
        if (deviceInList == null) return;

        deviceInList.state = ProvisioningDevice.STATE_BIND_FAIL;
        mListAdapter.notifyDataSetChanged();

        local.bindState = DeviceBindState.UNBIND;
        local.boundModels = remote.boundModels;
        mesh.saveOrUpdate(DeviceProvisionActivity.this);
    }

    private ProvisioningDevice getDeviceByMac(String mac) {
        if (devices == null) return null;
        for (ProvisioningDevice deviceInfo : devices) {
            if (deviceInfo.macAddress.equals(mac))
                return deviceInfo;
        }
        return null;
    }

    private ProvisioningDevice getDeviceByMeshAddress(int meshAddress) {
        if (devices == null) return null;
        for (ProvisioningDevice deviceInfo : devices) {
            if (deviceInfo.unicastAddress == meshAddress)
                return deviceInfo;
        }
        return null;
    }


}
