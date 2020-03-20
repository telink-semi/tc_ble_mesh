package com.telink.sig.mesh.demo.ui;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.model.Mesh;
import com.telink.sig.mesh.demo.model.PrivateDevice;
import com.telink.sig.mesh.demo.ui.adapter.FastProvisionAdapter;
import com.telink.sig.mesh.event.Event;
import com.telink.sig.mesh.event.EventListener;
import com.telink.sig.mesh.event.MeshEvent;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.light.parameter.FastProvisionParameters;
import com.telink.sig.mesh.model.DeviceBindState;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.NodeInfo;
import com.telink.sig.mesh.util.TelinkLog;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by kee on 2019/9/17.
 */

public class FastProvisionActivity extends BaseActivity implements EventListener<String>, View.OnClickListener {
    private Mesh mesh;
    private FastProvisionAdapter adapter;
    private List<DeviceInfo> devices;
    private Button btn_back;
    private PrivateDevice TARGET_DEVICE = PrivateDevice.CT;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_device_provision);
        mesh = TelinkMeshApplication.getInstance().getMesh();
        addEventListener();
        initTitle();

        RecyclerView rv_devices = findViewById(R.id.rv_devices);
        devices = new ArrayList<>();

        adapter = new FastProvisionAdapter(this, devices);
        rv_devices.setLayoutManager(new GridLayoutManager(this, 2));
        rv_devices.setAdapter(adapter);
        btn_back = (Button) findViewById(R.id.btn_back);
        btn_back.setOnClickListener(this);
        findViewById(R.id.tv_log).setOnClickListener(this);

        start();
    }

    private void initTitle() {
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.device_scan);
        setTitle("Device Scan(Fast)");

        MenuItem refreshItem = toolbar.getMenu().findItem(R.id.item_refresh);
        refreshItem.setVisible(false);
        toolbar.setNavigationIcon(null);
    }

    private void addEventListener() {
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_FAST_PROVISION_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_FAST_PROVISION_DEVICE_ADDRESS_SET, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_FAST_PROVISION_COMPLETE, this);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    private void start() {
        int pvIndex = mesh.pvIndex + 1;
        TelinkLog.d("fast provision pv : " + pvIndex);
        NodeInfo.CompositionData compositionData = NodeInfo.CompositionData.from(TARGET_DEVICE.getCpsData());

        // for more pid customer should add device composition data in PrivateDevice
        FastProvisionParameters parameters = FastProvisionParameters.getDefault(pvIndex, TARGET_DEVICE.getPid(), compositionData.elements.size());
        MeshService.getInstance().startFastProvision(parameters);
    }

    private void enableUI(boolean enable) {
        btn_back.setEnabled(enable);
    }

    @Override
    public void performed(final Event<String> event) {
        super.performed(event);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                switch (event.getType()) {
                    case MeshEvent.EVENT_TYPE_FAST_PROVISION_FAIL:
                        onProvisionComplete(false);
                        break;

                    case MeshEvent.EVENT_TYPE_FAST_PROVISION_DEVICE_ADDRESS_SET:
                        DeviceInfo deviceInfo = ((MeshEvent) event).getDeviceInfo();
                        deviceInfo.bindState = DeviceBindState.BINDING;
//                        mesh.pvIndex = deviceInfo.meshAddress + deviceInfo.elementCnt - 1;
                        mesh.pvIndex += deviceInfo.elementCnt;
                        TelinkLog.d("fast provision pv update: " + mesh.pvIndex + " -- " + deviceInfo.elementCnt);
                        mesh.saveOrUpdate(FastProvisionActivity.this);
                        devices.add(deviceInfo);
                        adapter.notifyDataSetChanged();
                        break;

                    case MeshEvent.EVENT_TYPE_FAST_PROVISION_COMPLETE:
                        onProvisionComplete(true);
                        break;
                }
            }
        });

    }

    private void onProvisionComplete(boolean success) {
        for (DeviceInfo device : devices) {
            if (success) {
                device.bindState = DeviceBindState.BOUND;
                NodeInfo nodeInfo = new NodeInfo();
                nodeInfo.nodeAdr = device.meshAddress;
                nodeInfo.elementCnt = device.elementCnt;
                nodeInfo.deviceKey = device.deviceKey;
                byte[] cpsData = TARGET_DEVICE.getCpsData();
                nodeInfo.cpsData = NodeInfo.CompositionData.from(cpsData);
                nodeInfo.cpsDataLen = cpsData.length;
                device.nodeInfo = nodeInfo;
                mesh.insertDevice(device);
            } else {
                device.bindState = DeviceBindState.UNBIND;
            }
        }
        adapter.notifyDataSetChanged();
        enableUI(true);

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
}
