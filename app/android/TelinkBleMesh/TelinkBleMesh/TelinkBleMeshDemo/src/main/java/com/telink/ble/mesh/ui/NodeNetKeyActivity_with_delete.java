/********************************************************************************************************
 * @file NodeNetKeyActivity_with_delete.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2017
 *
 * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/
package com.telink.ble.mesh.ui;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.MenuItem;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.config.ConfigStatus;
import com.telink.ble.mesh.core.message.config.NetKeyAddMessage;
import com.telink.ble.mesh.core.message.config.NetKeyDeleteMessage;
import com.telink.ble.mesh.core.message.config.NetKeyStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.MeshNetKey;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.db.MeshInfoService;
import com.telink.ble.mesh.ui.adapter.BaseRecyclerViewAdapter;
import com.telink.ble.mesh.ui.adapter.NodeMeshKeyAdapter;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 * network key in target device
 */
public class NodeNetKeyActivity_with_delete extends BaseActivity implements EventListener<String> {

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
    }

    private void updateKeyList() {
//        if (this.netKeyList.size() != 0) return;
        netKeyList.clear();
        MeshLogger.d("update key List -> node net key count: " + nodeInfo.netKeyIndexes.size());
        MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        for (MeshNetKey netKey :
                meshInfo.meshNetKeyList) {
            boolean exist = false;
            for (String index : nodeInfo.netKeyIndexes) {
                if (netKey.index == MeshUtils.hexToIntB(index)) {
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
            }


            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    dismissWaitingDialog();
                    Toast.makeText(NodeNetKeyActivity_with_delete.this, success ? "add net key success" : "add net key failed", Toast.LENGTH_SHORT).show();
                }
            });

        }
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
        nodeInfo.save();
    }

    public void onNetKeyAddSuccess(int keyIndex) {
        for (String keyIdx : nodeInfo.netKeyIndexes) {
            if (keyIndex == MeshUtils.hexToIntB(keyIdx)) {
                MeshLogger.d("net key already exists");
                return;
            }
        }
        MeshLogger.d("net key add success");
        nodeInfo.netKeyIndexes.add(MeshUtils.intToHex2(keyIndex));
        updateKeyList();
    }


    public void onNetKeyDeleteSuccess(int keyIndex) {
        Iterator<String> netKeyIt = nodeInfo.netKeyIndexes.iterator();
        while (netKeyIt.hasNext()) {
            if (MeshUtils.hexToIntB(netKeyIt.next()) == keyIndex) {
                netKeyIt.remove();
            }
        }
        MeshLogger.d("net key add success");
        updateKeyList();
    }
}
