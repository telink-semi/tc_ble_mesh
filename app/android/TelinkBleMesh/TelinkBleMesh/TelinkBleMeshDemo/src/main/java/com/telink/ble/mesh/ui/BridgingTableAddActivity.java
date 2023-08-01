/********************************************************************************************************
 * @file BridgingTableAddActivity.java
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

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.config.BridgingTableAddMessage;
import com.telink.ble.mesh.core.message.config.BridgingTableStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.model.BridgingTable;
import com.telink.ble.mesh.model.MeshNetKey;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.db.MeshInfoService;

import java.util.List;

/**
 * network key in target device
 */
public class BridgingTableAddActivity extends BaseActivity implements EventListener<String>, View.OnClickListener {

    private NodeInfo nodeInfo;
    private EditText et_direction, et_index_1, et_index_2, et_adr_1, et_adr_2;

    private static final String[] DIRECTIONS = new String[]{
            BridgingTable.Direction.UNIDIRECTIONAL.desc,
            BridgingTable.Direction.BIDIRECTIONAL.desc
    };


    private BridgingTable.Direction selectedDirection = BridgingTable.Direction.BIDIRECTIONAL;
    /**
     * selected net keys
     */
    private MeshNetKey selectedKey1, selectedKey2;
    List<MeshNetKey> netKeyList;
    private String[] keys;
    private Handler handler = new Handler();
    private BridgingTable table;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_bridging_table_add);
        setTitle("Add Bridging Table");
        enableBackNav(true);

        Intent intent = getIntent();
        if (intent.hasExtra("meshAddress")) {
            int meshAddress = intent.getIntExtra("meshAddress", -1);
            nodeInfo = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(meshAddress);
        } else {
            Toast.makeText(getApplicationContext(), "subnet -> params err", Toast.LENGTH_SHORT).show();
            finish();
            return;
        }
        initView();
        initKeys();
        TelinkMeshApplication.getInstance().addEventListener(BridgingTableStatusMessage.class.getName(), this);
    }

    private void initKeys() {
        netKeyList = TelinkMeshApplication.getInstance().getMeshInfo().meshNetKeyList;
        if (netKeyList.size() < 2) {
            showErrorDialog();
            return;
        }
        keys = new String[netKeyList.size()];
        for (int i = 0; i < keys.length; i++) {
//            keys[i] = String.format(Locale.getDefault(), "%s(%02d)", netKeyList.get(i).name, netKeyList.get(i).index);
            keys[i] = netKeyList.get(i).name;
        }
        selectedKey1 = netKeyList.get(1);
        selectedKey2 = netKeyList.get(0);
        et_index_1.setText(keys[1]);
        et_index_2.setText(keys[0]);
    }

    /**
     * @param index
     */
    private void showNetKeySelectDialog(final int index) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Select Net Key")
//                .setMessage("Less than two net keys are not supported")
                .setItems(keys, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        if (index == 0) {
                            et_index_1.setText(keys[which]);
                            selectedKey1 = netKeyList.get(which);
                        } else {
                            et_index_2.setText(keys[which]);
                            selectedKey2 = netKeyList.get(which);
                        }
                    }
                });
        builder.show();
    }

    private void showErrorDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Warning")
                .setMessage("Less than two net keys are not supported")
                .setCancelable(false)
                .setPositiveButton("Confirm", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        finish();
                    }
                });
        builder.show();
    }

    private void initView() {
        findViewById(R.id.btn_add).setOnClickListener(this);
        et_direction = findViewById(R.id.et_direction);
        et_index_1 = findViewById(R.id.et_index_1);
        et_index_1.setOnClickListener(this);
        et_index_2 = findViewById(R.id.et_index_2);
        et_index_2.setOnClickListener(this);
        et_adr_1 = findViewById(R.id.et_adr_1);
        et_adr_2 = findViewById(R.id.et_adr_2);
    }

    private void checkAndSendAddMessage() {
        String directionInput = et_direction.getText().toString().trim();
        if (TextUtils.isEmpty(directionInput)) {
            toastMsg("pls input direction");
            return;
        }
        int direction = selectedDirection.value;
//        int direction = Integer.parseInt(directionInput, 16);

        /*String idx1Input =  et_index_1.getText().toString().trim();
        if (TextUtils.isEmpty(idx1Input)) {
            toastMsg("pls input key index 1");
            return;
        }
        int idx1 = Integer.parseInt(idx1Input, 16);*/
        int idx1 = selectedKey1.index;

        /*String idx2Input = et_index_2.getText().toString().trim();
        if (TextUtils.isEmpty(idx2Input)) {
            toastMsg("pls input key index 2");
            return;
        }
        int idx2 = Integer.parseInt(idx2Input, 16);*/
        int idx2 = selectedKey2.index;

        String adr1Input = et_adr_1.getText().toString().trim();
        if (TextUtils.isEmpty(adr1Input)) {
            toastMsg("pls input address 1");
            return;
        }
        int adr1 = Integer.parseInt(adr1Input, 16);

        String adr2Input = et_adr_2.getText().toString().trim();
        if (TextUtils.isEmpty(adr2Input)) {
            toastMsg("pls input address 1");
            return;
        }
        int adr2 = Integer.parseInt(adr2Input, 16);

        BridgingTableAddMessage addMessage = new BridgingTableAddMessage(nodeInfo.meshAddress);
        addMessage.directions = (byte) direction;
        addMessage.netKeyIndex1 = idx1;
        addMessage.netKeyIndex2 = idx2;
        addMessage.address1 = adr1;
        addMessage.address2 = adr2;

        BridgingTable table = new BridgingTable();
        table.directions = (byte) direction;
        table.netKeyIndex1 = idx1;
        table.netKeyIndex2 = idx2;
        table.address1 = adr1;
        table.address2 = adr2;
        this.table = table;
        showWaitingDialog("adding bridging table...");
        handler.removeCallbacksAndMessages(null);
        handler.postDelayed(timeoutTask, 3 * 1000);
        MeshService.getInstance().sendMeshMessage(addMessage);
    }

    private Runnable timeoutTask = new Runnable() {
        @Override
        public void run() {
            toastMsg("add bridging table timeout");
            dismissWaitingDialog();
        }
    };


    @Override
    protected void onDestroy() {
        super.onDestroy();
        handler.removeCallbacksAndMessages(null);
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }


    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(BridgingTableStatusMessage.class.getName())) {
            BridgingTableStatusMessage statusMessage = (BridgingTableStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            if (table != null) {
                nodeInfo.bridgingTableList.add(table);
                nodeInfo.save();
            }
            handler.removeCallbacksAndMessages(null);
            if (statusMessage.getStatus() == 0) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        dismissWaitingDialog();
                        setResult(RESULT_OK);
                        finish();
                    }
                });
            } else {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        toastMsg("add bridging table failed");
                    }
                });
            }
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.et_index_1:
                showNetKeySelectDialog(0);
                break;
            case R.id.et_index_2:
                showNetKeySelectDialog(1);
                break;
            case R.id.btn_add:
                checkAndSendAddMessage();
                break;
        }
    }
}
