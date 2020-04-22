package com.telink.ble.mesh.ui;

import android.os.Bundle;
import android.os.Handler;
import android.util.SparseIntArray;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.entity.FastProvisioningConfiguration;
import com.telink.ble.mesh.entity.FastProvisioningDevice;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.FastProvisioningEvent;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.parameter.FastProvisioningParameters;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.PrivateDevice;
import com.telink.ble.mesh.ui.adapter.DeviceProvisionListAdapter;
import com.telink.ble.mesh.util.Arrays;

import java.util.ArrayList;
import java.util.List;

import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * fast provision
 */

public class FastProvisionActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {

    private MeshInfo meshInfo;

    /**
     * ui devices
     */
    private List<NodeInfo> devices = new ArrayList<>();

    private DeviceProvisionListAdapter mListAdapter;

    private Handler delayHandler = new Handler();

    private Button btn_back;

    private PrivateDevice[] targetDevices = PrivateDevice.values();

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
        btn_back = findViewById(R.id.btn_back);
        btn_back.setOnClickListener(this);

        meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);

        TelinkMeshApplication.getInstance().addEventListener(FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_ADDRESS_SET, this);
        TelinkMeshApplication.getInstance().addEventListener(FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_SUCCESS, this);

        actionStart();
    }

    private void initTitle() {
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.device_scan);
        setTitle("Device Scan(Fast)");

        MenuItem refreshItem = toolbar.getMenu().findItem(R.id.item_refresh);
        refreshItem.setVisible(false);
        toolbar.setNavigationIcon(null);
    }

    private void actionStart() {
        enableUI(false);

        int provisionIndex = meshInfo.provisionIndex;
        SparseIntArray targetDevicePid = new SparseIntArray(targetDevices.length);

        CompositionData compositionData;
        for (PrivateDevice privateDevice : targetDevices) {
            compositionData = CompositionData.from(privateDevice.getCpsData());
            targetDevicePid.put(privateDevice.getPid(), compositionData.elements.size());
        }
        MeshService.getInstance().startFastProvision(new FastProvisioningParameters(FastProvisioningConfiguration.getDefault(
                provisionIndex,
                targetDevicePid
        )));

    }

    private void enableUI(boolean enable) {
        btn_back.setEnabled(enable);
    }


    private void onDeviceFound(FastProvisioningDevice fastProvisioningDevice) {
        NodeInfo nodeInfo = new NodeInfo();
        nodeInfo.meshAddress = fastProvisioningDevice.getNewAddress();

        nodeInfo.deviceUUID = new byte[16];
        System.arraycopy(fastProvisioningDevice.getMac(), 0, nodeInfo.deviceUUID, 0, 6);

        nodeInfo.deviceKey = fastProvisioningDevice.getDeviceKey();
        nodeInfo.elementCnt = fastProvisioningDevice.getElementCount();
        nodeInfo.compositionData = getCompositionData(fastProvisioningDevice.getPid());
        nodeInfo.state = NodeInfo.STATE_PROVISIONING;
        devices.add(nodeInfo);
        mListAdapter.notifyDataSetChanged();

        meshInfo.provisionIndex = fastProvisioningDevice.getNewAddress() + fastProvisioningDevice.getElementCount();
        meshInfo.saveOrUpdate(this);
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
        delayHandler.removeCallbacksAndMessages(null);
    }


    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_back) {
            finish();
        }
    }

    @Override
    public void performed(final Event<String> event) {
        String eventType = event.getType();

        if (eventType.equals(FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_ADDRESS_SET)) {
            onDeviceFound(((FastProvisioningEvent) event).getFastProvisioningDevice());
        } else if (eventType.equals(FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_FAIL)) {
            onFastProvisionComplete(false);
        } else if (eventType.equals(FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_SUCCESS)) {
            onFastProvisionComplete(true);
        }
    }

    private void onFastProvisionComplete(boolean success) {
        for (NodeInfo nodeInfo : devices) {
            if (success) {
                nodeInfo.state = NodeInfo.STATE_BIND_SUCCESS;
                meshInfo.insertDevice(nodeInfo);
            } else {
                nodeInfo.state = NodeInfo.STATE_PROVISION_FAIL;
            }
        }
        if (success) {
            meshInfo.saveOrUpdate(this);
        }
        mListAdapter.notifyDataSetChanged();
        enableUI(true);
    }

    private CompositionData getCompositionData(int pid) {
        for (PrivateDevice privateDevice : targetDevices) {
            if (pid == privateDevice.getPid())
                return CompositionData.from(privateDevice.getCpsData());

        }
        return null;
    }
}
