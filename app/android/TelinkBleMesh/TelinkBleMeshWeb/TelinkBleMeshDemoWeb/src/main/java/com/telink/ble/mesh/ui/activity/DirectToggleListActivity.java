///********************************************************************************************************
// * @file DirectToggleListActivity.java
// *
// * @brief for TLSR chips
// *
// * @author telink
// * @date Sep. 30, 2017
// *
// * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
// *
// *          Licensed under the Apache License, Version 2.0 (the "License");
// *          you may not use this file except in compliance with the License.
// *          You may obtain a copy of the License at
// *
// *              http://www.apache.org/licenses/LICENSE-2.0
// *
// *          Unless required by applicable law or agreed to in writing, software
// *          distributed under the License is distributed on an "AS IS" BASIS,
// *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// *          See the License for the specific language governing permissions and
// *          limitations under the License.
// *******************************************************************************************************/
//package com.telink.ble.mesh.ui;
//
//import android.content.Intent;
//import android.os.Bundle;
//import android.os.Handler;
//
//import androidx.annotation.Nullable;
//import androidx.recyclerview.widget.LinearLayoutManager;
//import androidx.recyclerview.widget.RecyclerView;
//
//import com.telink.ble.mesh.TelinkMeshApplication;
//import com.telink.ble.mesh.core.message.directforwarding.DirectedControlStatusMessage;
//import com.telink.ble.mesh.demo.R;
//import com.telink.ble.mesh.foundation.Event;
//import com.telink.ble.mesh.foundation.EventListener;
//import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
//import com.telink.ble.mesh.model.MeshInfo;
//import com.telink.ble.mesh.model.NodeInfo;
//import com.telink.ble.mesh.ui.adapter.DirectToggleListAdapter;
//import com.telink.ble.mesh.util.MeshLogger;
//
///**
// * network key in target device
// */
//public class DirectToggleListActivity extends BaseActivity implements EventListener<String> {
//
//    private MeshInfo meshInfo;
//    private Handler handler = new Handler();
//    private DirectToggleListAdapter listAdapter;
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        if (!validateNormalStart(savedInstanceState)) {
//            return;
//        }
//        setContentView(R.layout.activity_direct_toggle_list);
//        initTitle();
//
//        RecyclerView rv_df = findViewById(R.id.rv_toggle);
//        rv_df.setLayoutManager(new LinearLayoutManager(this));
//
//        meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
//        listAdapter = new DirectToggleListAdapter(this, meshInfo.nodes);
//        rv_df.setAdapter(listAdapter);
//
//        TelinkMeshApplication.getInstance().addEventListener(DirectedControlStatusMessage.class.getName(), this);
//    }
//
//    private void initTitle() {
//        setTitle("Direct Toggle List");
//        enableBackNav(true);
//    }
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
//        if (event.getType().equals(DirectedControlStatusMessage.class.getName())) {
//            DirectedControlStatusMessage controlStatusMessage = (DirectedControlStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
//            if (controlStatusMessage.status != 0) {
//                MeshLogger.d("DirectedControlStatusMessage status err");
//                return;
//            }
//            MeshLogger.d(controlStatusMessage.toString());
//            int src = ((StatusNotificationEvent) event).getNotificationMessage().getSrc();
//            NodeInfo node = meshInfo.getDeviceByMeshAddress(src);
//            if (node != null) {
//                node.directForwardingEnabled = controlStatusMessage.directedForwarding == 1;
//                node.directRelay = controlStatusMessage.directedRelay == 1;
//                node.directProxyEnabled = controlStatusMessage.directedProxy == 1;
//                node.directFriend = controlStatusMessage.directedFriend == 1;
//                listAdapter.notifyDataSetChanged();
//                meshInfo.saveOrUpdate(this);
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
//}
