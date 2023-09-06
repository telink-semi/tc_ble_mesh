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

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.view.View;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.bottomsheet.BottomSheetDialog;
import com.google.android.material.textfield.TextInputEditText;
import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.db.MeshInfoService;
import com.telink.ble.mesh.ui.adapter.NetworkListAdapter;

/**
 * network key in target device
 */
public class NetworkListActivity extends BaseActivity implements View.OnClickListener {

    private static final int REQUEST_CODE_IMPORT = 1;
    private Handler handler = new Handler();
    private NetworkListAdapter listAdapter;
    private int selectedPosition = -1;

    private BottomSheetDialog networkActionDialog;

    /**
     * actions in bottom dialog
     */
    private TextView tv_dialog_title;

    /**
     * text input in network create dialog
     *
     * @see #showCreateNetworkDialog()
     */
    private TextInputEditText et_single_input = null;

    private long lastMeshId;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        lastMeshId = TelinkMeshApplication.getInstance().getMeshInfo().id;
        setContentView(R.layout.activity_network_list);
        initView();
    }


    private void initView() {
        initTitle();
        RecyclerView rv_df = findViewById(R.id.rv_network);
        rv_df.setLayoutManager(new LinearLayoutManager(this));
        findViewById(R.id.fab_import).setOnClickListener(this);

        listAdapter = new NetworkListAdapter(this, this::showBottomDialog);
        rv_df.setAdapter(listAdapter);
        updateListData();
        initBottomDialog();
        findViewById(R.id.btn_remove_all).setOnClickListener(this);
    }

    private void updateListData() {
        listAdapter.resetData(MeshInfoService.getInstance().getAll());
    }

    private void initBottomDialog() {
        networkActionDialog = new BottomSheetDialog(this, R.style.BottomSheetDialog);
        networkActionDialog.setCancelable(true);
        networkActionDialog.setContentView(R.layout.dialog_bottom_network);
        tv_dialog_title = networkActionDialog.findViewById(R.id.tv_dialog_title);
        TextView tv_share = networkActionDialog.findViewById(R.id.tv_share);
        TextView tv_switch = networkActionDialog.findViewById(R.id.tv_switch);
        TextView tv_delete = networkActionDialog.findViewById(R.id.tv_delete);
        networkActionDialog.findViewById(R.id.iv_close).setOnClickListener(this);
        networkActionDialog.findViewById(R.id.tv_show_detail).setOnClickListener(this);
        tv_share.setOnClickListener(this);
        tv_switch.setOnClickListener(this);
        tv_delete.setOnClickListener(this);
    }

    private void showBottomDialog(int position) {
        this.selectedPosition = position;
        String title = "select action for : " + listAdapter.get(position).meshName;
        tv_dialog_title.setText(title);
        networkActionDialog.show();
    }


    private void initTitle() {
        setTitle("Network List");
        enableBackNav(true);
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.menu_single);
        toolbar.setOnMenuItemClickListener(item -> {
            showCreateNetworkDialog();
            return false;
        });
    }

    private void showCreateNetworkDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Create New Mesh Network")
                .setView(R.layout.dialog_single_input)
//                .setMessage("Input Network Name")
                .setPositiveButton("Confirm", (dialog, which) -> createNetwork(et_single_input.getText().toString()))
                .setNegativeButton("Cancel", (dialog, which) -> dialog.cancel());
        AlertDialog dialog = builder.show();
        et_single_input = dialog.findViewById(R.id.et_single_input);
        et_single_input.setHint("please input new name");
    }

    private void createNetwork(String name) {
        if (TextUtils.isEmpty(name)) {
            toastMsg("pls input network name");
            return;
        }
        MeshInfo meshInfo = MeshInfo.createNewMesh(this, name);
        MeshInfoService.getInstance().addMeshInfo(meshInfo);
        updateListData();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        handler.removeCallbacksAndMessages(null);
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    @Override
    protected void onResume() {
        super.onResume();
        listAdapter.resetCurMeshId();
        updateListData();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.fab_import:
                startActivityForResult(new Intent(this, ShareImportActivity.class), REQUEST_CODE_IMPORT);
                break;
            case R.id.iv_close:
                networkActionDialog.dismiss();
                break;

            case R.id.tv_show_detail:
                showNetworkDetail();
                networkActionDialog.dismiss();
                break;
            case R.id.tv_share:
                startActivity(new Intent(this, ShareExportActivity.class).putExtra("MeshInfoId", listAdapter.get(selectedPosition).id));
                networkActionDialog.dismiss();
                break;
            case R.id.tv_switch:
                switchToNetwork();
                networkActionDialog.dismiss();
                break;
            case R.id.tv_delete:
                if (listAdapter.isCurrentMesh(selectedPosition)) {
                    toastMsg("can not delete current mesh network");
                    return;
                }
                showDeleteDialog();
                networkActionDialog.dismiss();
                break;
            case R.id.btn_remove_all:
                showConfirmDialog("remove all and create a new mesh? ", (dialog, which) -> removeAllMesh());
                break;
        }
    }

    private void removeAllMesh() {
        MeshInfoService.getInstance().removeAllMesh();
        MeshInfo meshInfo = MeshInfo.createNewMesh(this, "Default Mesh");
        MeshInfoService.getInstance().addMeshInfo(meshInfo);
        TelinkMeshApplication.getInstance().setupMesh(meshInfo);
        listAdapter.resetData(MeshInfoService.getInstance().getAll());
        listAdapter.resetCurMeshId();
    }

    private void showNetworkDetail() {
        startActivity(new Intent(this, MeshInfoActivity.class)
                .putExtra("MeshInfoId", listAdapter.get(selectedPosition).id));
    }

    private void showDeleteDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Warning")
                .setMessage("Delete Mesh Network?")
                .setPositiveButton("Confirm", (dialog, which) -> deleteNetwork())
                .setNegativeButton("Cancel", (dialog, which) -> dialog.cancel());
        builder.show();
    }

    private void deleteNetwork() {
        MeshInfo meshNetwork = listAdapter.get(selectedPosition);
        MeshInfoService.getInstance().removeMeshInfo(meshNetwork);
        listAdapter.remove(selectedPosition);
    }

    private void switchToNetwork() {
        MeshInfo meshNetwork = listAdapter.get(selectedPosition);
        TelinkMeshApplication.getInstance().setupMesh(meshNetwork);
        toastMsg("switch success");
        listAdapter.resetCurMeshId();
        // keep
        /*Intent intent = new Intent(this, MainActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
        startActivity(intent);
        finish();*/
    }

    @Override
    public void finish() {
        super.finish();
        if (listAdapter != null) {
            MeshService.getInstance().idle(lastMeshId != listAdapter.getCurMeshId());
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (data == null) return;
        if (requestCode == REQUEST_CODE_IMPORT && resultCode == RESULT_OK) {
            long importedNetworkId = data.getLongExtra(ShareImportActivity.EXTRA_NETWORK_ID, 0);
            long curMeshId = TelinkMeshApplication.getInstance().getMeshInfo().id;
            if (importedNetworkId != curMeshId) {
                if (SharedPreferenceHelper.getShareImportCompleteAction(this) == SharedPreferenceHelper.IMPORT_COMPLETE_ACTION_DEFAULT) {
                    showConfirmDialog("share import success, switch to the new mesh?", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            switchToNetworkById(importedNetworkId);
                        }
                    });
                } else {
                    switchToNetworkById(importedNetworkId);
                }
            }
        }
    }

    private void switchToNetworkById(long networkId) {
        MeshInfo meshNetwork = MeshInfoService.getInstance().getById(networkId);
        TelinkMeshApplication.getInstance().setupMesh(meshNetwork);
        listAdapter.resetCurMeshId();
        toastMsg("switch to network success: " + meshNetwork.meshName);
    }
}
