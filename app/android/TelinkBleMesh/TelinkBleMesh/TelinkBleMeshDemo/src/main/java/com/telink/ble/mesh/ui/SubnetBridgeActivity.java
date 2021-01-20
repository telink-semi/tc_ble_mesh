/********************************************************************************************************
 * @file CompositionDataActivity.java
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

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.Switch;
import android.widget.Toast;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.config.BridgingTableRemoveMessage;
import com.telink.ble.mesh.core.message.config.BridgingTableStatusMessage;
import com.telink.ble.mesh.core.message.config.SubnetBridgeGetMessage;
import com.telink.ble.mesh.core.message.config.SubnetBridgeSetMessage;
import com.telink.ble.mesh.core.message.config.SubnetBridgeStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.model.BridgingTable;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.ui.adapter.BaseRecyclerViewAdapter;
import com.telink.ble.mesh.ui.adapter.BridgingTableAdapter;

import androidx.annotation.Nullable;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * network key in target device
 */
public class SubnetBridgeActivity extends BaseActivity implements EventListener<String>, View.OnClickListener {

    private BridgingTableAdapter adapter;
    private NodeInfo nodeInfo;
    private int meshAddress;
    private Handler handler = new Handler();
    private Switch sw_bridge;
    private int removeIndex = -1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_subnet_bridge);
        setTitle("Subnet Bridge");
        enableBackNav(true);

        Intent intent = getIntent();
        if (intent.hasExtra("meshAddress")) {
            meshAddress = intent.getIntExtra("meshAddress", -1);
            nodeInfo = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(meshAddress);
        } else {
            Toast.makeText(getApplicationContext(), "subnet -> params err", Toast.LENGTH_SHORT).show();
            finish();
            return;
        }

        initView();
        adapter = new BridgingTableAdapter(this, nodeInfo.bridgingTableList);
        adapter.setOnItemLongClickListener(new BaseRecyclerViewAdapter.OnItemLongClickListener() {
            @Override
            public boolean onLongClick(int position) {
                removeIndex = position;
                showRemoveDialog();
                return false;
            }
        });
        RecyclerView recyclerView = findViewById(R.id.rv_bridging_table);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        recyclerView.setAdapter(adapter);
        TelinkMeshApplication.getInstance().addEventListener(SubnetBridgeStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(BridgingTableStatusMessage.class.getName(), this);

    }

    private void showRemoveDialog() {
        showConfirmDialog("Remove Bridging Table ?", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                BridgingTableRemoveMessage removeMessage = new BridgingTableRemoveMessage(meshAddress);
                final BridgingTable table = nodeInfo.bridgingTableList.get(removeIndex);
                if (table == null) return;
                removeMessage.netKeyIndex1 = table.netKeyIndex1;
                removeMessage.netKeyIndex2 = table.netKeyIndex2;
                removeMessage.address1 = table.address1;
                removeMessage.address2 = table.address2;
                showWaitingDialog("removing table...");
                handler.postDelayed(timeoutTask, 3 * 1000);
                MeshService.getInstance().sendMeshMessage(removeMessage);
            }
        });
    }

    private void initView() {
        findViewById(R.id.btn_add_table).setOnClickListener(this);
        sw_bridge = findViewById(R.id.sw_bridge);
        findViewById(R.id.rl_switch).setOnClickListener(this);
        updateState();

    }

    private void updateState() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                dismissWaitingDialog();
                sw_bridge.setChecked(nodeInfo.subnetBridgeEnabled);
            }
        });

    }

    private void updateTable() {
        adapter.notifyDataSetChanged();
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                dismissWaitingDialog();
            }
        });
    }

    private void getSubnetBridge() {
        SubnetBridgeGetMessage getMessage = new SubnetBridgeGetMessage(this.meshAddress);
        MeshService.getInstance().sendMeshMessage(getMessage);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        handler.removeCallbacksAndMessages(null);
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }


    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(SubnetBridgeStatusMessage.class.getName())) {
            SubnetBridgeStatusMessage bridgeStatusMessage = (SubnetBridgeStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            int state = bridgeStatusMessage.getSubnetBridgeState();
            nodeInfo.subnetBridgeEnabled = state == 1;
            TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate(this);
            handler.removeCallbacksAndMessages(null);
            updateState();

        } else if (event.getType().equals(BridgingTableStatusMessage.class.getName())) {
            BridgingTableStatusMessage statusMessage = (BridgingTableStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            if (statusMessage.getStatus() == 0) {
                handler.removeCallbacksAndMessages(null);
                if (removeIndex != -1) {
                    nodeInfo.bridgingTableList.remove(removeIndex);
                    TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate(this);
                    removeIndex = -1;
                }
                updateTable();
            }
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode == RESULT_OK) {
            updateTable();
        }
    }

    private Runnable timeoutTask = new Runnable() {
        @Override
        public void run() {
            toastMsg("processing timeout");
            removeIndex = -1;
            dismissWaitingDialog();
        }
    };

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.rl_switch:
                byte newState = (byte) (nodeInfo.subnetBridgeEnabled ? 0 : 1);
                SubnetBridgeSetMessage setMessage = new SubnetBridgeSetMessage(meshAddress, newState);
                showWaitingDialog((newState == 0 ? "disabling" : "enabling") + " subnet bridge...");
                handler.removeCallbacksAndMessages(null);
                handler.postDelayed(timeoutTask, 3 * 1000);
                MeshService.getInstance().sendMeshMessage(setMessage);
                break;

            case R.id.btn_add_table:
                startActivityForResult(new Intent(SubnetBridgeActivity.this, BridgingTableAddActivity.class)
                        .putExtra("meshAddress", nodeInfo.meshAddress), 1);
                break;

        }
    }
}
