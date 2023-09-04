/********************************************************************************************************
 * @file MeshInfoActivity.java
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
import android.text.TextUtils;
import android.widget.TextView;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.textfield.TextInputEditText;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.db.MeshInfoService;
import com.telink.ble.mesh.ui.adapter.AppKeyInfoAdapter;
import com.telink.ble.mesh.ui.adapter.NetKeyInfoAdapter;

/**
 * Created by kee on 2021/01/13.
 */
public class MeshInfoActivity extends BaseActivity {
    private MeshInfo mesh;
    private TextView tv_mesh_name;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_mesh_info);
        initData();
        initView();
        updateUI();
        enableBackNav(true);
    }

    private void initView() {
        setTitle("Mesh Info", mesh.meshName);
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.menu_single);
        toolbar.getMenu().findItem(R.id.item_add).setIcon(R.drawable.ic_edit);
        toolbar.setOnMenuItemClickListener(item -> {
            showEditNameDialog();
            return false;
        });
    }

    private void initData() {
        Intent intent = getIntent();
        long id = intent.getLongExtra("MeshInfoId", 0);
        MeshInfo curMesh = TelinkMeshApplication.getInstance().getMeshInfo();
        if (id != 0) {
            mesh = MeshInfoService.getInstance().getById(id);
            if (mesh.id == curMesh.id) {
                mesh = curMesh;
            }
        } else {
            mesh = curMesh;
        }
    }


    TextInputEditText et_single_input;

    private void showEditNameDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Update Mesh Name")
                .setView(R.layout.dialog_single_input)
                .setPositiveButton("Confirm", (dialog, which) -> updateMeshName(et_single_input.getText().toString()))
                .setNegativeButton("Cancel", (dialog, which) -> dialog.cancel());
        AlertDialog dialog = builder.show();
        et_single_input = dialog.findViewById(R.id.et_single_input);
        et_single_input.setText(mesh.meshName);
        et_single_input.setHint("please input new mesh name");
    }

    private void updateMeshName(String name) {
        if (TextUtils.isEmpty(name)) {
            toastMsg("mesh name can not by null!");
            return;
        }
        mesh.meshName = name;
        mesh.saveOrUpdate();
        setSubTitle(name);
        tv_mesh_name.setText(name);
        toastMsg("update name success");
    }

    private void updateUI() {

        TextView
                tv_mesh_uuid,
                tv_iv_index,
                tv_sno,
                tv_local_adr;
        tv_mesh_name = findViewById(R.id.tv_mesh_name);
        tv_mesh_name.setText(mesh.meshName);

        tv_mesh_uuid = findViewById(R.id.tv_mesh_uuid);
        tv_mesh_uuid.setText(mesh.meshUUID);

        tv_iv_index = findViewById(R.id.tv_iv_index);
        tv_iv_index.setText(String.format("0x%08X", mesh.ivIndex));
        tv_sno = findViewById(R.id.tv_sno);
        tv_sno.setText(String.format("0x%06X", mesh.sequenceNumber));
        tv_local_adr = findViewById(R.id.tv_local_adr);
        tv_local_adr.setText(String.format("0x%04X", mesh.localAddress));
        NetKeyInfoAdapter netKeyAdapter = new NetKeyInfoAdapter(this, mesh.meshNetKeyList);
        AppKeyInfoAdapter appKeyAdapter = new AppKeyInfoAdapter(this, mesh.appKeyList);

        RecyclerView rv_net_key = findViewById(R.id.rv_net_key);
//        rv_net_key.setNestedScrollingEnabled(false);
        rv_net_key.setLayoutManager(new LinearLayoutManager(this));
        rv_net_key.setAdapter(netKeyAdapter);
        RecyclerView rv_app_key = findViewById(R.id.rv_app_key);
        rv_app_key.setNestedScrollingEnabled(false);
        rv_app_key.setLayoutManager(new LinearLayoutManager(this));
        rv_app_key.setAdapter(appKeyAdapter);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }


}
