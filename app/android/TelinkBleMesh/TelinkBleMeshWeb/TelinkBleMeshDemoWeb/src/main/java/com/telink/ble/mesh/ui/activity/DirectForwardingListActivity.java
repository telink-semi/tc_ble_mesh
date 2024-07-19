/********************************************************************************************************
 * @file DirectForwardingListActivity.java
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
//package com.telink.ble.mesh.ui;
//
//import android.content.Intent;
//import android.os.Bundle;
//import android.os.Handler;
//import android.view.View;
//
//import androidx.annotation.Nullable;
//import androidx.recyclerview.widget.LinearLayoutManager;
//import androidx.recyclerview.widget.RecyclerView;
//
//import com.telink.ble.mesh.TelinkMeshApplication;
//import com.telink.ble.mesh.core.message.directforwarding.DirectedControlSetMessage;
//import com.telink.ble.mesh.core.message.directforwarding.ForwardingTableDeleteMessage;
//import com.telink.ble.mesh.core.message.directforwarding.ForwardingTableStatusMessage;
//import com.telink.ble.mesh.demo.R;
//import com.telink.ble.mesh.foundation.Event;
//import com.telink.ble.mesh.foundation.EventListener;
//import com.telink.ble.mesh.foundation.MeshService;
//import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
//import com.telink.ble.mesh.model.DirectForwardingInfo;
//import com.telink.ble.mesh.model.DirectForwardingInfoService;
//import com.telink.ble.mesh.model.MeshInfo;
//import com.telink.ble.mesh.model.NodeInfo;
//import com.telink.ble.mesh.ui.adapter.DirectForwardingListAdapter;
//import com.telink.ble.mesh.util.MeshLogger;
//
//import java.util.List;
//
///**
// * network key in target device
// */
//public class DirectForwardingListActivity extends BaseActivity implements EventListener<String>, View.OnClickListener {
//
//
//    private MeshInfo meshInfo;
//    private NodeInfo nodeInfo;
//    private int meshAddress;
//    private Handler handler = new Handler();
//    private DirectForwardingListAdapter listAdapter;
//    private List<DirectForwardingInfo> infoList;
//    private DirectForwardingInfo selectInfo;
//    private int deleteIndex = 0;
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        if (!validateNormalStart(savedInstanceState)) {
//            return;
//        }
//        setContentView(R.layout.activity_df_list);
//        initTitle();
//
//        findViewById(R.id.view_toggle).setOnClickListener(this);
//        findViewById(R.id.btn_add).setOnClickListener(this);
//        RecyclerView rv_df = findViewById(R.id.rv_df);
//        rv_df.setLayoutManager(new LinearLayoutManager(this));
//        DirectForwardingInfoService.getInstance().load(this);
//        infoList = DirectForwardingInfoService.getInstance().get();
//        listAdapter = new DirectForwardingListAdapter(this, infoList);
//        listAdapter.setOnItemLongClickListener(position -> {
//            showDeleteDialog(position);
//            return false;
//        });
//        rv_df.setAdapter(listAdapter);
//
//        meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
////        TelinkMeshApplication.getInstance().addEventListener(DirectedControlStatusMessage.class.getName(), this);
//        TelinkMeshApplication.getInstance().addEventListener(ForwardingTableStatusMessage.class.getName(), this);
//    }
//
//    @Override
//    protected void onResume() {
//        super.onResume();
//        listAdapter.notifyDataSetChanged();
//    }
//
//    private void initTitle() {
//        setTitle("Direct Forwarding");
//        enableBackNav(true);
//        /*Toolbar toolbar = findViewById(R.id.title_bar);
//        toolbar.inflateMenu(R.menu.common_list);
//        toolbar.setOnMenuItemClickListener(item -> {
//            if (item.getItemId() == R.id.item_add) {
//                startActivity(new Intent(DirectForwardingListActivity.this, DirectForwardingActivity.class)
//                );
//            }
//            return false;
//        });*/
//    }
//
//    private void showDeleteDialog(int position) {
//        showConfirmDialog("Confirm to delete table ?", (dialog, which) -> {
//            selectInfo = infoList.get(position);
//            deleteIndex = -2;
//            showWaitingDialog("deleting table...");
//            deleteNext();
//        });
//    }
//
//    private void deleteNext() {
//
//        if (deleteIndex >= selectInfo.nodesOnRoute.size()) {
//            dismissWaitingDialog();
//            DirectForwardingInfoService.getInstance().removeItem(selectInfo);
//            listAdapter.notifyDataSetChanged();
//            MeshLogger.d("delete complete");
//            handler.removeCallbacksAndMessages(null);
//            selectInfo = null;
//        } else {
//
//            int address;
//            if(deleteIndex == -2){
//                address = selectInfo.originAdr;
//            } else if (deleteIndex == -1) {
//                address = selectInfo.target;
//            }else{
//                address = selectInfo.nodesOnRoute.get(deleteIndex);
//            }
//
//            ForwardingTableDeleteMessage deleteMessage = new ForwardingTableDeleteMessage(address);
//            deleteMessage.netKeyIndex = meshInfo.getDefaultNetKey().index;
//            deleteMessage.pathOrigin = selectInfo.originAdr;
//            deleteMessage.destination = selectInfo.target;
//            MeshService.getInstance().sendMeshMessage(deleteMessage);
//            handler.postDelayed(DELETE_TIMEOUT_TASK, 3 * 1000);
//        }
//    }
//
//    private Runnable DELETE_TIMEOUT_TASK = () -> {
//        deleteIndex++;
//        deleteNext();
//    };
//
//
//    @Override
//    protected void onDestroy() {
//        super.onDestroy();
//        handler.removeCallbacksAndMessages(null);
//        TelinkMeshApplication.getInstance().removeEventListener(this);
//    }
//
//    private Runnable timeoutTask = () -> {
//        toastMsg("processing timeout");
//        dismissWaitingDialog();
//    };
//
//
//    @Override
//    public void performed(Event<String> event) {
//        if (event.getType().equals(ForwardingTableStatusMessage.class.getName())) {
//            if (selectInfo == null) return;
//            ForwardingTableStatusMessage tableStatusMessage = (ForwardingTableStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
//            MeshLogger.d("tableStatusMessage - " + tableStatusMessage.toString());
//            if (tableStatusMessage.status == 0) {
//                deleteIndex++;
//                deleteNext();
//            }
//        }
//    }
//
//    @Override
//    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
//        super.onActivityResult(requestCode, resultCode, data);
//        if (resultCode == RESULT_OK) {
//            listAdapter.notifyDataSetChanged();
//        }
//    }
//
//    @Override
//    public void onClick(View v) {
//        switch (v.getId()) {
//            case R.id.rl_switch:
//                DirectedControlSetMessage setMessage = DirectedControlSetMessage.getDFEnable(
//                        meshAddress,
//                        meshInfo.getDefaultNetKey().index,
//                        !nodeInfo.directForwardingEnabled
//                );
//
//                showWaitingDialog((nodeInfo.directForwardingEnabled ? "disabling" : "enabling") + " subnet bridge...");
//                handler.removeCallbacksAndMessages(null);
//                handler.postDelayed(timeoutTask, 3 * 1000);
//                MeshService.getInstance().sendMeshMessage(setMessage);
//                break;
//
//            case R.id.view_toggle:
//                startActivity(new Intent(this, DirectToggleListActivity.class));
//                break;
//
//            case R.id.btn_add:
//                startActivity(new Intent(DirectForwardingListActivity.this, DirectForwardingActivity.class));
//                break;
//        }
//    }
//}
