/********************************************************************************************************
 * @file MeshOTAActivity.java
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
package com.telink.ble.mesh.ui;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareUpdateInfoGetMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareUpdateInfoStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.ui.adapter.BaseSelectableListAdapter;
import com.telink.ble.mesh.ui.adapter.FUDeviceSelectAdapter;
import com.telink.ble.mesh.util.Arrays;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


/**
 * select target devices for firmware-update
 */
public class FUDeviceSelectActivity extends BaseActivity implements View.OnClickListener,
        BaseSelectableListAdapter.SelectStatusChangedListener,
        EventListener<String> {

    public static final String KEY_SELECTED_DEVICES = "update-selected-devices";

    /**
     * view adapter
     */
    private FUDeviceSelectAdapter mDeviceAdapter;

    /**
     * local mesh info
     */
    private MeshInfo mesh;

    /**
     * local devices
     */
    private List<NodeInfo> devices;

    /**
     * updating devices
     */
    private ArrayList<MeshUpdatingDevice> updatingDevices;

    /**
     * UIView
     */
    private CheckBox cb_device;

    private Button btn_confirm;
    private RecyclerView rv_device;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_fu_device_select);
        mesh = TelinkMeshApplication.getInstance().getMeshInfo();
        setTitle("Device Select");
        enableBackNav(true);
        initView();
        addEventListeners();
    }

    private void initView() {
        cb_device = findViewById(R.id.cb_device);
        rv_device = findViewById(R.id.rv_device);

        rv_device.setLayoutManager(new LinearLayoutManager(this));
        if (mesh.nodes != null) {
            for (NodeInfo deviceInfo : mesh.nodes) {
                deviceInfo.selected = false;
            }
        }
        devices = mesh.nodes;

        mDeviceAdapter = new FUDeviceSelectAdapter(this, devices);
        mDeviceAdapter.setStatusChangedListener(this);
        rv_device.setAdapter(mDeviceAdapter);
        cb_device.setChecked(mDeviceAdapter.allSelected());
        btn_confirm = findViewById(R.id.btn_confirm);
        btn_confirm.setOnClickListener(this);
        cb_device.setOnClickListener(this);
    }

    private void addEventListeners() {
        TelinkMeshApplication.getInstance().addEventListener(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, this);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {

            case R.id.btn_confirm:
                List<NodeInfo> nodes = getSelectedNodes();
                if (nodes == null) {
                    toastMsg("Pls select at least ONE device");
                    return;
                }
                updatingDevices = new ArrayList<>();
                MeshUpdatingDevice device;
                for (NodeInfo node : nodes) {
                    device = new MeshUpdatingDevice();
                    device.meshAddress = (node.meshAddress);
                    device.updatingEleAddress = (node.getTargetEleAdr(MeshSigModel.SIG_MD_OBJ_TRANSFER_S.modelId));
                    device.isSupported = node.getTargetEleAdr(MeshSigModel.SIG_MD_FW_UPDATE_S.modelId) != -1;
                    device.pidInfo = node.getPidDesc();
                    device.isLpn = node.isLpn();
                    updatingDevices.add(device);
                }

                Intent intent = new Intent();
                intent.putParcelableArrayListExtra(KEY_SELECTED_DEVICES, updatingDevices);
                setResult(RESULT_OK, intent);
                finish();
                break;

            case R.id.cb_device:
                mDeviceAdapter.setAll(!mDeviceAdapter.allSelected());
                break;
        }
    }


    public List<NodeInfo> getSelectedNodes() {
        List<NodeInfo> nodes = null;
        for (NodeInfo deviceInfo : mesh.nodes) {
            if (deviceInfo.selected && deviceInfo.getOnOff() != -1 && deviceInfo.getTargetEleAdr(MeshSigModel.SIG_MD_FW_UPDATE_S.modelId) != -1) {
                if (nodes == null) {
                    nodes = new ArrayList<>();
                }
                nodes.add(deviceInfo);
            }
        }
        return nodes;
    }

    @Override
    public void onSelectStatusChanged(BaseSelectableListAdapter adapter) {
        if (adapter == mDeviceAdapter) {
            cb_device.setChecked(mDeviceAdapter.allSelected());
        }
    }

    /****************************************************************
     * events - start
     ****************************************************************/
    @Override
    public void performed(Event<String> event) {

        final String eventType = event.getType();
        if (eventType.equals(MeshEvent.EVENT_TYPE_DISCONNECTED)
                || eventType.equals(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED)) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    mDeviceAdapter.notifyDataSetChanged();
                }
            });
        }
    }


}
