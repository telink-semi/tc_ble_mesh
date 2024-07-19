/********************************************************************************************************
 * @file DeviceSelectActivity.java
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
//import android.view.View;
//import android.widget.Button;
//
//import androidx.recyclerview.widget.LinearLayoutManager;
//import androidx.recyclerview.widget.RecyclerView;
//
//import com.telink.ble.mesh.TelinkMeshApplication;
//import com.telink.ble.mesh.demo.R;
//import com.telink.ble.mesh.foundation.Event;
//import com.telink.ble.mesh.foundation.EventListener;
//import com.telink.ble.mesh.foundation.event.MeshEvent;
//import com.telink.ble.mesh.model.MeshInfo;
//import com.telink.ble.mesh.model.NodeInfo;
//import com.telink.ble.mesh.model.NodeStatusChangedEvent;
//import com.telink.ble.mesh.ui.adapter.BaseSelectableListAdapter;
//import com.telink.ble.mesh.ui.adapter.DeviceSelectAdapter;
//
//import java.util.ArrayList;
//
//
///**
// * select device
// */
//public class DeviceSelectActivity extends BaseActivity implements View.OnClickListener,
//        BaseSelectableListAdapter.SelectStatusChangedListener,
//        EventListener<String> {
//
//    public static final String KEY_TITLE = "select-device-title";
//
//    public static final String KEY_SELECTED_DEVICES = "selected-devices";
//
//    public static final String KEY_SELECT_MULTI = "select-multi";
//
//    public static final String KEY_SPECIFIED_MODEL = "specified-model";
//    //  && deviceInfo.getTargetEleAdr(MeshSigModel.SIG_MD_SCENE_S.modelId) != -1
//
//
//    private DeviceSelectAdapter deviceSelectAdapter;
//
//    /**
//     * local mesh info
//     */
//    private MeshInfo mesh;
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        if (!validateNormalStart(savedInstanceState)) {
//            return;
//        }
//        setContentView(R.layout.activity_device_select);
//        mesh = TelinkMeshApplication.getInstance().getMeshInfo();
//        Intent intent = getIntent();
//        if(intent.hasExtra(KEY_TITLE)){
//            String title = intent.getStringExtra(KEY_TITLE);
//            setTitle(title);
//        }else{
//            setTitle("Device Select");
//        }
//
//        enableBackNav(true);
//        initView();
//        addEventListeners();
//    }
//
//    private void initView() {
//
//        RecyclerView rv_device = findViewById(R.id.rv_device);
//        rv_device.setLayoutManager(new LinearLayoutManager(this));
//
//        for (NodeInfo node :
//                mesh.nodes) {
//            node.selected = false;
//        }
//
//        deviceSelectAdapter = new DeviceSelectAdapter(this, mesh.nodes);
//        deviceSelectAdapter.setOnItemClickListener(position -> {
//
//        });
//
//        rv_device.setAdapter(deviceSelectAdapter);
//
//        Button btn_confirm = findViewById(R.id.btn_confirm);
//        btn_confirm.setOnClickListener(this);
//    }
//
//    private void addEventListeners() {
//        TelinkMeshApplication.getInstance().addEventListener(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, this);
//    }
//
//    @Override
//    protected void onDestroy() {
//        super.onDestroy();
//        TelinkMeshApplication.getInstance().removeEventListener(this);
//    }
//
//    @Override
//    public void onClick(View v) {
//        switch (v.getId()) {
//
//            case R.id.btn_confirm:
//                ArrayList<Integer> nodes = getSelectedNodes();
//                if (nodes == null) {
//                    toastMsg("Pls select at least ONE device");
//                    return;
//                }
//
//                Intent intent = new Intent();
//                intent.putIntegerArrayListExtra(KEY_SELECTED_DEVICES, nodes);
//                setResult(RESULT_OK, intent);
//                finish();
//                break;
//
//        }
//    }
//
//
//    public ArrayList<Integer> getSelectedNodes() {
//        ArrayList<Integer> nodes = null;
//
//        for (NodeInfo nodeInfo : mesh.nodes) {
//            // deviceInfo.getOnOff() != -1 &&
//            if (nodeInfo.selected) {
//                if (nodes == null) {
//                    nodes = new ArrayList<>();
//                }
//                nodes.add(nodeInfo.meshAddress);
//            }
//        }
//
//        return nodes;
//    }
//
//    @Override
//    public void onSelectStatusChanged(BaseSelectableListAdapter adapter) {
//
//    }
//
//    /****************************************************************
//     * events - start
//     ****************************************************************/
//    @Override
//    public void performed(Event<String> event) {
//
//        final String eventType = event.getType();
//        if (eventType.equals(MeshEvent.EVENT_TYPE_DISCONNECTED)
//                || eventType.equals(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED)) {
//            runOnUiThread(() -> deviceSelectAdapter.notifyDataSetChanged());
//        }
//    }
//
//
//}
