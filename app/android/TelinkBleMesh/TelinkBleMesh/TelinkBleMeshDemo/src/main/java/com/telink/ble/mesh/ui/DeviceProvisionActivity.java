package com.telink.ble.mesh.ui;

import android.content.Intent;
import android.os.Bundle;
import android.os.ParcelUuid;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.ble.MeshScanRecord;
import com.telink.ble.mesh.core.ble.UUIDInfo;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.AdvertisingDevice;
import com.telink.ble.mesh.entity.BindingDevice;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.entity.ProvisioningDevice;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.BindingEvent;
import com.telink.ble.mesh.foundation.event.ProvisioningEvent;
import com.telink.ble.mesh.foundation.event.ScanEvent;
import com.telink.ble.mesh.foundation.parameter.BindingParameters;
import com.telink.ble.mesh.foundation.parameter.ProvisioningParameters;
import com.telink.ble.mesh.foundation.parameter.ScanParameters;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.PrivateDevice;
import com.telink.ble.mesh.ui.adapter.DeviceProvisionListAdapter;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.ArrayList;
import java.util.List;

import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

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

    private List<NodeInfo> devices;
    private DeviceProvisionListAdapter mListAdapter;
    private Button btn_back;
    private MeshInfo mesh;
    private MenuItem refreshItem;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
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
        TelinkMeshApplication.getInstance().addEventListener(ProvisioningEvent.EVENT_TYPE_PROVISION_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(ProvisioningEvent.EVENT_TYPE_PROVISION_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(BindingEvent.EVENT_TYPE_BIND_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(BindingEvent.EVENT_TYPE_BIND_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(ScanEvent.EVENT_TYPE_SCAN_TIMEOUT, this);
        TelinkMeshApplication.getInstance().addEventListener(ScanEvent.EVENT_TYPE_DEVICE_FOUND, this);
        mesh = TelinkMeshApplication.getInstance().getMeshInfo();

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

    private void onDeviceFound(AdvertisingDevice advertisingDevice) {
        int address = mesh.provisionIndex;

        MeshLogger.d("alloc address: " + address);
        if (address == -1) {
            enableUI(true);
            return;
        }

        NodeInfo nodeInfo = new NodeInfo();
        nodeInfo.meshAddress = mesh.provisionIndex;
        nodeInfo.macAddress = advertisingDevice.device.getAddress();
        nodeInfo.state = NodeInfo.STATE_PROVISIONING;

        devices.add(nodeInfo);
        mListAdapter.notifyDataSetChanged();

        ProvisioningDevice provisioningDevice = new ProvisioningDevice(advertisingDevice, address);

        // for static oob test
        /*provisioningDevice.setAuthValue(new byte[]{
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
        });*/
        ProvisioningParameters provisioningParameters = new ProvisioningParameters(provisioningDevice);
        MeshService.getInstance().startProvisioning(provisioningParameters);
    }

    @Override
    public void finish() {
        super.finish();
        MeshService.getInstance().idle(true);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
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
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (event.getType().equals(ProvisioningEvent.EVENT_TYPE_PROVISION_SUCCESS)) {
                    onProvisionSuccess((ProvisioningEvent) event);
                } else if (event.getType().equals(ScanEvent.EVENT_TYPE_SCAN_TIMEOUT)) {
                    enableUI(true);
                } else if (event.getType().equals(ProvisioningEvent.EVENT_TYPE_PROVISION_FAIL)) {
                    onProvisionFail((ProvisioningEvent) event);
                    startScan();
                } else if (event.getType().equals(BindingEvent.EVENT_TYPE_BIND_SUCCESS)) {
                    onKeyBindSuccess((BindingEvent) event);
                    startScan();
                } else if (event.getType().equals(BindingEvent.EVENT_TYPE_BIND_FAIL)) {
                    onKeyBindFail((BindingEvent) event);
                    startScan();
                } else if (event.getType().equals(ScanEvent.EVENT_TYPE_DEVICE_FOUND)) {
                    AdvertisingDevice device = ((ScanEvent) event).getAdvertisingDevice();
                    onDeviceFound(device);
                }
            }
        });

    }


    private void onProvisionFail(ProvisioningEvent event) {
        ProvisioningDevice deviceInfo = event.getProvisioningDevice();

        NodeInfo pvDevice = getNodeByMac(deviceInfo.getAdvertisingDevice().device.getAddress());
        if (pvDevice == null) return;
        pvDevice.state = NodeInfo.STATE_PROVISION_FAIL;
        pvDevice.stateDesc = event.getDesc();
        mListAdapter.notifyDataSetChanged();
    }

    private void onProvisionSuccess(ProvisioningEvent event) {

        ProvisioningDevice remote = event.getProvisioningDevice();

        NodeInfo nodeInfo = getNodeByMac(remote.getAdvertisingDevice().device.getAddress());

        if (nodeInfo == null) return;

        nodeInfo.state = NodeInfo.STATE_BINDING;
        int elementCnt = remote.getDeviceCapability().eleNum;
        nodeInfo.elementCnt = elementCnt;
        nodeInfo.deviceKey = remote.getDeviceKey();
        mesh.insertDevice(nodeInfo);
        mesh.provisionIndex += elementCnt;
        mesh.saveOrUpdate(DeviceProvisionActivity.this);


        // check if private mode opened
        final boolean privateMode = SharedPreferenceHelper.isPrivateMode(this);

        // check if device support fast bind
        boolean defaultBound = false;
        if (privateMode && remote.getAdvertisingDevice().scanRecord != null) {
            PrivateDevice device = getPrivateDevice(remote.getAdvertisingDevice().scanRecord);
            if (device != null) {
                MeshLogger.d("private device");
                final byte[] cpsData = device.getCpsData();
                nodeInfo.compositionData = CompositionData.from(cpsData);
                defaultBound = true;
            } else {
                MeshLogger.d("private device null");
            }
        }

        nodeInfo.setDefaultBind(defaultBound);
        mListAdapter.notifyDataSetChanged();
        int appKeyIndex = mesh.getDefaultAppKeyIndex();
        BindingDevice bindingDevice = new BindingDevice(nodeInfo.meshAddress, nodeInfo.macAddress, appKeyIndex);
        bindingDevice.setDefaultBound(defaultBound);
//        bindingDevice.setDefaultBound(false);
        MeshService.getInstance().startBinding(new BindingParameters(bindingDevice));
    }


    private PrivateDevice getPrivateDevice(byte[] scanRecord) {
        if (scanRecord == null) return null;
        MeshScanRecord sr = MeshScanRecord.parseFromBytes(scanRecord);
        byte[] serviceData = sr.getServiceData(ParcelUuid.fromString(UUIDInfo.PROVISION_SERVICE_UUID.toString()));
        if (serviceData == null) return null;
        return PrivateDevice.filter(serviceData);
    }


    private void onKeyBindSuccess(BindingEvent event) {
        BindingDevice remote = event.getBindingDevice();

        NodeInfo deviceInList = getNodeByMac(remote.getMacAddress());
        if (deviceInList == null) return;

        deviceInList.state = NodeInfo.STATE_BIND_SUCCESS;

        // if is default bound, composition data has been valued ahead of binding action
        if (!remote.isDefaultBound()) {
            deviceInList.compositionData = remote.getCompositionData();
        }

        mListAdapter.notifyDataSetChanged();
        mesh.saveOrUpdate(DeviceProvisionActivity.this);
    }

    private void onKeyBindFail(BindingEvent event) {
        BindingDevice remote = event.getBindingDevice();
        NodeInfo deviceInList = getNodeByMac(remote.getMacAddress());
        if (deviceInList == null) return;

        deviceInList.state = NodeInfo.STATE_BIND_FAIL;
        deviceInList.stateDesc = event.getDesc();
        mListAdapter.notifyDataSetChanged();
        mesh.saveOrUpdate(DeviceProvisionActivity.this);
    }

    private NodeInfo getNodeByMac(String mac) {
        for (NodeInfo nodeInfo :
                this.devices) {
            if (mac.equals(nodeInfo.macAddress)) {
                return nodeInfo;
            }
        }
        return null;
    }
}
