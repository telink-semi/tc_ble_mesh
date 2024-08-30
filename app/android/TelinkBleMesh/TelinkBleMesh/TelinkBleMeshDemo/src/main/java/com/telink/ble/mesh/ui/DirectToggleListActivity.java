/********************************************************************************************************
 * @file DirectToggleListActivity.java
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
import android.view.View;
import android.widget.ImageView;

import androidx.annotation.Nullable;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.directforwarding.DirectedControlSetMessage;
import com.telink.ble.mesh.core.message.directforwarding.DirectedControlStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.ReliableMessageProcessEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.ui.adapter.DirectToggleListAdapter;
import com.telink.ble.mesh.util.MeshLogger;

/**
 * network key in target device
 */
public class DirectToggleListActivity extends BaseActivity implements EventListener<String>, View.OnClickListener {

    private MeshInfo meshInfo;
    private Handler handler = new Handler();
    private DirectToggleListAdapter listAdapter;

    private static final int ACTION_ENABLE_DF = 0x01;
    private static final int ACTION_DISABLE_DF = 0x02;

    private static final int ACTION_ENABLE_RELAY = 0x03;
    private static final int ACTION_DISABLE_RELAY = 0x04;

    private static final int ACTION_ENABLE_PROXY = 0x05;
    private static final int ACTION_DISABLE_PROXY = 0x06;

    private static final int ACTION_ENABLE_FRIEND = 0x07;
    private static final int ACTION_DISABLE_FRIEND = 0x08;

    int curIndex = 0;
    int action = 0;
    private boolean isBatchSetting = false;
    private boolean isAllVisible = true;
    private View ll_all_control;
    private ImageView iv_all_visible;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_direct_toggle_list);
        initTitle();

        RecyclerView rv_df = findViewById(R.id.rv_toggle);
        rv_df.setLayoutManager(new LinearLayoutManager(this));

        meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        listAdapter = new DirectToggleListAdapter(this, meshInfo.nodes);
        rv_df.setAdapter(listAdapter);

        findViewById(R.id.btn_enable_df).setOnClickListener(this);
        findViewById(R.id.btn_disable_df).setOnClickListener(this);
        findViewById(R.id.btn_enable_relay).setOnClickListener(this);
        findViewById(R.id.btn_disable_relay).setOnClickListener(this);
        findViewById(R.id.btn_enable_proxy).setOnClickListener(this);
        findViewById(R.id.btn_disable_proxy).setOnClickListener(this);
        findViewById(R.id.btn_enable_friend).setOnClickListener(this);
        findViewById(R.id.btn_disable_friend).setOnClickListener(this);
        iv_all_visible = findViewById(R.id.iv_all_visible);
        iv_all_visible.setOnClickListener(this);
        ll_all_control = findViewById(R.id.ll_all_control);
        TelinkMeshApplication.getInstance().addEventListener(DirectedControlStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(ReliableMessageProcessEvent.EVENT_TYPE_MSG_PROCESS_COMPLETE, this);
    }

    private void initTitle() {
        setTitle("Direct Toggle List");
        enableBackNav(true);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        handler.removeCallbacksAndMessages(null);
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    private Runnable timeoutTask = () -> {
        toastMsg("processing timeout");
        dismissWaitingDialog();
    };


    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(DirectedControlStatusMessage.class.getName())) {
            DirectedControlStatusMessage controlStatusMessage = (DirectedControlStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            if (controlStatusMessage.status != 0) {
                MeshLogger.d("DirectedControlStatusMessage status err");
                return;
            }
            MeshLogger.d(controlStatusMessage.toString());
            int src = ((StatusNotificationEvent) event).getNotificationMessage().getSrc();
            NodeInfo node = meshInfo.getDeviceByMeshAddress(src);
            if (node != null) {
                node.directForwardingEnabled = controlStatusMessage.directedForwarding == 1;
                node.directRelayEnabled = controlStatusMessage.directedRelay == 1;
                node.directProxyEnabled = controlStatusMessage.directedProxy == 1;
                node.directFriendEnabled = controlStatusMessage.directedFriend == 1;
                listAdapter.notifyDataSetChanged();
                node.save();
            }
        } else if (event.getType().equals(ReliableMessageProcessEvent.EVENT_TYPE_MSG_PROCESS_COMPLETE)) {
            ReliableMessageProcessEvent processEvent = (ReliableMessageProcessEvent) event;
            if (isBatchSetting && processEvent.getOpcode() == Opcode.DIRECTED_CONTROL_SET.value) {
                setNext();
            }
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode == RESULT_OK) {
            listAdapter.notifyDataSetChanged();
        }
    }


    private void setAll(int action) {
        showWaitingDialog("Setting Direct...");
        isBatchSetting = true;
        handler.postDelayed(this::dismissWaitingDialog, 800 * meshInfo.nodes.size());
        curIndex = 0;
        this.action = action;
        setNext();
    }

    private void setNext() {
        MeshLogger.d("set next - " + curIndex);
        if (curIndex >= meshInfo.nodes.size()) {
            // all complete
            isBatchSetting = false;
            dismissWaitingDialog();
            return;
        }
        NodeInfo node = meshInfo.nodes.get(curIndex);
        curIndex += 1;
        if (node.isOffline()) {
            setNext();
        }
        int adr = node.meshAddress;

        DirectedControlSetMessage setMessage = new DirectedControlSetMessage(adr);
        switch (action) {
            case ACTION_ENABLE_DF:
                setMessage.directedForwarding = 1; // get current state
                setMessage.directedRelay = (byte) (node.directProxyEnabled ? 1 : 0); //  reverse, changed current state
                setMessage.setDirectedProxy((byte) (node.directProxyEnabled ? 1 : 0));
                setMessage.directedFriend = (byte) (node.directFriendEnabled ? 1 : 0);
                break;

            case ACTION_DISABLE_DF:
                setMessage.directedForwarding = 0; // get current state
                setMessage.directedRelay = (byte) (node.directProxyEnabled ? 1 : 0); //  reverse, changed current state
                setMessage.setDirectedProxy((byte) (node.directProxyEnabled ? 1 : 0));
                setMessage.directedFriend = (byte) (node.directFriendEnabled ? 1 : 0);
                break;

            case ACTION_ENABLE_RELAY:
                setMessage.directedForwarding = (byte) (node.directForwardingEnabled ? 1 : 0); // get current state
                setMessage.directedRelay = 1; //  reverse, changed current state
                setMessage.setDirectedProxy((byte) (node.directProxyEnabled ? 1 : 0));
                setMessage.directedFriend = (byte) (node.directFriendEnabled ? 1 : 0);
                break;

            case ACTION_DISABLE_RELAY:
                setMessage.directedForwarding = (byte) (node.directForwardingEnabled ? 1 : 0); // get current state
                setMessage.directedRelay = 0; //  reverse, changed current state
                setMessage.setDirectedProxy((byte) (node.directProxyEnabled ? 1 : 0));
                setMessage.directedFriend = (byte) (node.directFriendEnabled ? 1 : 0);
                break;

            case ACTION_ENABLE_PROXY:
                setMessage.directedForwarding = (byte) (node.directForwardingEnabled ? 1 : 0); // get current state
                setMessage.directedRelay = (byte) (node.directProxyEnabled ? 1 : 0); //  reverse, changed current state
                setMessage.setDirectedProxy((byte) 1);
                setMessage.directedFriend = (byte) (node.directFriendEnabled ? 1 : 0);
                break;
            case ACTION_DISABLE_PROXY:
                setMessage.directedForwarding = (byte) (node.directForwardingEnabled ? 1 : 0); // get current state
                setMessage.directedRelay = (byte) (node.directProxyEnabled ? 1 : 0); //  reverse, changed current state
                setMessage.setDirectedProxy((byte) 0);
                setMessage.directedFriend = (byte) (node.directFriendEnabled ? 1 : 0);
                break;

            case ACTION_ENABLE_FRIEND:
                setMessage.directedForwarding = (byte) (node.directForwardingEnabled ? 1 : 0); // get current state
                setMessage.directedRelay = (byte) (node.directProxyEnabled ? 1 : 0); //  reverse, changed current state
                setMessage.setDirectedProxy((byte) (node.directProxyEnabled ? 1 : 0));
                setMessage.directedFriend = ((byte) 1);
                break;
            case ACTION_DISABLE_FRIEND:
                setMessage.directedForwarding = (byte) (node.directForwardingEnabled ? 1 : 0); // get current state
                setMessage.directedRelay = (byte) (node.directProxyEnabled ? 1 : 0); //  reverse, changed current state
                setMessage.setDirectedProxy((byte) (node.directProxyEnabled ? 1 : 0));
                setMessage.directedFriend = ((byte) 0);
                break;
        }
        MeshService.getInstance().sendMeshMessage(setMessage);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_enable_df:
                setAll(ACTION_ENABLE_DF);
                break;
            case R.id.btn_disable_df:
                setAll(ACTION_DISABLE_DF);
                break;
            case R.id.btn_enable_relay:
                setAll(ACTION_ENABLE_RELAY);
                break;
            case R.id.btn_disable_relay:
                setAll(ACTION_DISABLE_RELAY);
                break;
            case R.id.btn_enable_proxy:
                setAll(ACTION_ENABLE_PROXY);
                break;
            case R.id.btn_disable_proxy:
                setAll(ACTION_DISABLE_PROXY);
                break;
            case R.id.btn_enable_friend:
                setAll(ACTION_ENABLE_FRIEND);
                break;
            case R.id.btn_disable_friend:
                setAll(ACTION_DISABLE_FRIEND);
                break;

            case R.id.iv_all_visible:
                isAllVisible = !isAllVisible;
                iv_all_visible.setImageResource(isAllVisible ? R.drawable.ic_eye_on : R.drawable.ic_eye_off);
                ll_all_control.setVisibility(isAllVisible ? View.VISIBLE : View.GONE);
                break;

        }
    }
}
