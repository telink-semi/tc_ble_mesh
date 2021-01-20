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

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.MenuItem;
import android.widget.Toast;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.access.BindingBearer;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.config.ConfigStatus;
import com.telink.ble.mesh.core.message.config.NetKeyAddMessage;
import com.telink.ble.mesh.core.message.config.NetKeyDeleteMessage;
import com.telink.ble.mesh.core.message.config.NetKeyStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.BindingDevice;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.BindingEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.foundation.parameter.BindingParameters;
import com.telink.ble.mesh.model.MeshAppKey;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.MeshNetKey;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.ui.adapter.BaseRecyclerViewAdapter;
import com.telink.ble.mesh.ui.adapter.NodeMeshKeyAdapter;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * network key in target device
 */
public class NodeNetKeyActivity extends BaseActivity implements EventListener<String> {

    public static final int ACTION_IDLE = 0;
    public static final int ACTION_ADD = 1;

    public static final int ACTION_DELETE = 2;

    private int action = ACTION_IDLE;

    private int processingIndex = -1;

    private NodeMeshKeyAdapter<MeshNetKey> adapter;
    private NodeInfo nodeInfo;
    private int meshAddress;
    private List<MeshNetKey> netKeyList = new ArrayList<>();
    private List<MeshNetKey> excludeNetKeyList = new ArrayList<>();
    private Handler handler = new Handler();
    AlertDialog dialog;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_network_key_setting);
        setTitle("Network Keys");
        enableBackNav(true);
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.net_key);
        MenuItem menuItem = toolbar.getMenu().findItem(R.id.item_add);
        menuItem.setVisible(true);
        toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                if (item.getItemId() == R.id.item_add) {
                    showAddDialog();
                }
                return false;
            }
        });


        Intent intent = getIntent();
        if (intent.hasExtra("meshAddress")) {
            meshAddress = intent.getIntExtra("meshAddress", -1);
            nodeInfo = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(meshAddress);
        } else {
            Toast.makeText(getApplicationContext(), "net key -> params err", Toast.LENGTH_SHORT).show();
            finish();
            return;
        }

        adapter = new NodeMeshKeyAdapter(this, netKeyList, true);
        RecyclerView recyclerView = findViewById(R.id.rv_net_key);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        recyclerView.setAdapter(adapter);
        updateKeyList();
        TelinkMeshApplication.getInstance().addEventListener(NetKeyStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(BindingEvent.EVENT_TYPE_BIND_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(BindingEvent.EVENT_TYPE_BIND_SUCCESS, this);
    }

    private void updateKeyList() {
//        if (this.netKeyList.size() != 0) return;
        netKeyList.clear();
        MeshLogger.d("update key List -> node net key count: " + nodeInfo.netKeyIndexes.size());
        MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        for (MeshNetKey netKey :
                meshInfo.meshNetKeyList) {
            boolean exist = false;
            for (int index : nodeInfo.netKeyIndexes) {
                if (netKey.index == index) {
                    exist = true;
                    this.netKeyList.add(netKey);
                }
            }
            if (!exist) {
                this.excludeNetKeyList.add(netKey);
            }
        }
        adapter.notifyDataSetChanged();
    }


    private void showAddDialog() {
        if (nodeInfo.netKeyIndexes.size() >= 2) {
            toastMsg("more than 2 net keys is not supported");
            return;
        }
        if (excludeNetKeyList.size() == 0) {
            toastMsg("not found available net key");
            return;
        }
        String[] keyInfoList = new String[excludeNetKeyList.size()];
        MeshNetKey netKey;
        for (int i = 0; i < keyInfoList.length; i++) {
            netKey = excludeNetKeyList.get(i);
            keyInfoList[i] = String.format("name: %s\nindex: %02X\nkey: %s",
                    netKey.name,
                    netKey.index,
                    Arrays.bytesToHexString(netKey.key));
        }
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        RecyclerView recyclerView = new RecyclerView(this);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        NodeMeshKeyAdapter<MeshNetKey> adapter = new NodeMeshKeyAdapter<>(this, excludeNetKeyList, false);
        adapter.setOnItemClickListener(new BaseRecyclerViewAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(int position) {
                if (dialog != null) {
                    dialog.dismiss();
                }
                onNetKeySelect(excludeNetKeyList.get(position), ACTION_ADD);
            }
        });
        recyclerView.setAdapter(adapter);
        builder.setView(recyclerView);
        builder.setTitle("Select Net Key");
        dialog = builder.show();
    }

    public void onNetKeySelect(MeshNetKey netKey, int action) {
        MeshLogger.d("on key selected : " + netKey.index);
        MeshMessage meshMessage = null;
        this.action = action;
        this.processingIndex = netKey.index;
        if (action == ACTION_ADD) {
            meshMessage = new NetKeyAddMessage(nodeInfo.meshAddress, netKey.index,
                    netKey.key);
        } else if (action == ACTION_DELETE) {
            meshMessage = new NetKeyDeleteMessage(nodeInfo.meshAddress, netKey.index);
        }

        showWaitingDialog("net key adding...");
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                toastMsg("net key add timeout");
                dismissWaitingDialog();
            }
        }, 3 * 1000);
        MeshService.getInstance().sendMeshMessage(meshMessage);

    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        handler.removeCallbacksAndMessages(null);
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }


    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(NetKeyStatusMessage.class.getName())) {
            NetKeyStatusMessage netKeyStatusMessage = (NetKeyStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            MeshLogger.d("net key status received");
            handler.removeCallbacksAndMessages(null);
            final boolean success = netKeyStatusMessage.getStatus() == ConfigStatus.SUCCESS.code;
            if (success) {
                onNetKeyStatus();
            } else {
                MeshLogger.d("net key status error");
                onActionComplete(false);
            }

        } else if (event.getType().equals(BindingEvent.EVENT_TYPE_BIND_FAIL)) {
            onActionComplete(false);
        } else if (event.getType().equals(BindingEvent.EVENT_TYPE_BIND_SUCCESS)) {
            onActionComplete(true);
        }
    }

    private void onActionComplete(final boolean success) {
        if (success){
            nodeInfo.netKeyIndexes.add(processingIndex);
            TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate(this);
            updateKeyList();
        }
        processingIndex = -1;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                dismissWaitingDialog();
                Toast.makeText(NodeNetKeyActivity.this, success ? "add net key success" : "add net key failed", Toast.LENGTH_SHORT).show();
            }
        });
    }

    public void onNetKeyStatus() {
        if (action == ACTION_IDLE || processingIndex == -1) {
            return;
        }
        if (action == ACTION_ADD) {
            onNetKeyAddSuccess(processingIndex);
        } else if (action == ACTION_DELETE) {
            onNetKeyDeleteSuccess(processingIndex);
        }

    }

    public void onNetKeyAddSuccess(int keyIndex) {
        for (int keyIdx : nodeInfo.netKeyIndexes) {
            if (keyIndex == keyIdx) {
                MeshLogger.d("net key already exists");
                return;
            }
        }
        MeshLogger.d("net key add success");
        startBinding(keyIndex);

    }

    private void startBinding(int keyIndex) {
        //        find binding app key
        final List<MeshAppKey> appKeys = TelinkMeshApplication.getInstance().getMeshInfo().appKeyList;
        MeshAppKey meshAppKey = null;
        for (MeshAppKey appKey : appKeys) {
            if (appKey.boundNetKeyIndex == keyIndex) {
                meshAppKey = appKey;
                break;
            }
        }
        if (meshAppKey == null) {
            MeshLogger.w("meshAppKey not found");
            return;
        }
        CompositionData compositionData = nodeInfo.compositionData;

        BindingDevice bindingDevice = new BindingDevice(nodeInfo.meshAddress, nodeInfo.deviceUUID,
                meshAppKey.index);
        bindingDevice.setNetKeyIndex(meshAppKey.boundNetKeyIndex);
        bindingDevice.setCompositionData(compositionData);

        bindingDevice.setBearer(BindingBearer.Any);
        MeshService.getInstance().startBinding(new BindingParameters(bindingDevice));
    }

    public void onNetKeyDeleteSuccess(int keyIndex) {
        Iterator<Integer> netKeyIt = nodeInfo.netKeyIndexes.iterator();
        while (netKeyIt.hasNext()) {
            if (netKeyIt.next() == keyIndex) {
                netKeyIt.remove();
            }
        }
        MeshLogger.d("net key add success");
        updateKeyList();
    }
}
