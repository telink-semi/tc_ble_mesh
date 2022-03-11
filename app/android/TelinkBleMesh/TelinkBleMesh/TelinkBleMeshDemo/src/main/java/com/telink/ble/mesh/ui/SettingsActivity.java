/********************************************************************************************************
 * @file SettingsActivity.java
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

import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.TextView;

import androidx.appcompat.app.AlertDialog;

import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.networking.ExtendBearerMode;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.model.FUCacheService;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

/**
 * Created by kee on 2018/9/18.
 */
public class SettingsActivity extends BaseActivity implements View.OnClickListener {

    private Switch switch_log, switch_private, switch_remote_prov, switch_fast_prov, switch_no_oob, switch_auto_provision;
    private EditText et_extend, et_net_key, et_app_key;
    private MeshInfo mesh;
    private TextView tv_online_status;
    private AlertDialog extendDialog;
    private final String[] EXTEND_TYPES = new String[]{
            "No Extend",
            "Extend GATT Only",
            "Extend GATT & ADV",};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_settings);
        setTitle("Settings");
        switch_log = findViewById(R.id.switch_log);
        switch_log.setChecked(SharedPreferenceHelper.isLogEnable(this));
        switch_log.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                SharedPreferenceHelper.setLogEnable(SettingsActivity.this, isChecked);
                MeshLogger.enableRecord(isChecked);
            }
        });

        switch_private = findViewById(R.id.switch_private_mode);
        switch_private.setChecked(SharedPreferenceHelper.isPrivateMode(this));
        switch_private.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                SharedPreferenceHelper.setPrivateMode(SettingsActivity.this, isChecked);
            }
        });

        switch_remote_prov = findViewById(R.id.switch_remote_prov);

        if (AppSettings.DRAFT_FEATURES_ENABLE) {
            findViewById(R.id.ll_rp).setVisibility(View.VISIBLE);
            findViewById(R.id.ll_cert).setVisibility(View.VISIBLE);
        } else {
            findViewById(R.id.ll_rp).setVisibility(View.GONE);
            findViewById(R.id.ll_cert).setVisibility(View.GONE);
        }

        switch_remote_prov.setChecked(SharedPreferenceHelper.isRemoteProvisionEnable(this));
        switch_remote_prov.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                SharedPreferenceHelper.setRemoteProvisionEnable(SettingsActivity.this, isChecked);
                if (isChecked && SharedPreferenceHelper.isRemoteProvisionEnable(SettingsActivity.this)) {
                    switch_fast_prov.setChecked(false);
                }
            }
        });

        switch_fast_prov = findViewById(R.id.switch_fast_prov);
        switch_fast_prov.setChecked(SharedPreferenceHelper.isFastProvisionEnable(this));
        switch_fast_prov.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                SharedPreferenceHelper.setFastProvisionEnable(SettingsActivity.this, isChecked);
                if (isChecked && SharedPreferenceHelper.isFastProvisionEnable(SettingsActivity.this)) {
                    switch_remote_prov.setChecked(false);
                }
            }
        });

        switch_no_oob = findViewById(R.id.switch_no_oob);
        switch_no_oob.setChecked(SharedPreferenceHelper.isNoOOBEnable(this));
        switch_no_oob.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                SharedPreferenceHelper.setNoOOBEnable(SettingsActivity.this, isChecked);
            }
        });

        switch_auto_provision = findViewById(R.id.switch_auto_provision);
        switch_auto_provision.setChecked(SharedPreferenceHelper.isAutoPvEnable(this));
        switch_auto_provision.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                SharedPreferenceHelper.setAutoPvEnable(SettingsActivity.this, isChecked);
            }
        });

        enableBackNav(true);


        findViewById(R.id.iv_copy_net_key).setOnClickListener(this);
        findViewById(R.id.iv_copy_app_key).setOnClickListener(this);
        findViewById(R.id.btn_reset_mesh).setOnClickListener(this);
        findViewById(R.id.iv_tip_auto_provision).setOnClickListener(this);
        findViewById(R.id.iv_tip_remote_prov).setOnClickListener(this);
        findViewById(R.id.iv_tip_default_bound).setOnClickListener(this);
        findViewById(R.id.iv_tip_no_oob).setOnClickListener(this);
        findViewById(R.id.tv_select_database).setOnClickListener(this);
        findViewById(R.id.iv_tip_fast_prov).setOnClickListener(this);
        findViewById(R.id.tv_cert).setOnClickListener(this);
        et_extend = findViewById(R.id.et_extend_type);
        et_extend.setOnClickListener(this);
        et_net_key = findViewById(R.id.et_net_key);
        et_app_key = findViewById(R.id.et_app_key);
        showMeshInfo();
        tv_online_status = findViewById(R.id.tv_online_status);
        tv_online_status.setText(AppSettings.ONLINE_STATUS_ENABLE ? R.string.online_status_enabled : R.string.online_status_disabled);

        onExtendTypeSelect(SharedPreferenceHelper.getExtendBearerMode(this).ordinal());
    }

    private void showMeshInfo() {
        mesh = TelinkMeshApplication.getInstance().getMeshInfo();
        et_net_key.setText(Arrays.bytesToHexString(mesh.meshNetKeyList.get(0).key, ""));
        et_app_key.setText(Arrays.bytesToHexString(mesh.appKeyList.get(0).key, ""));
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.iv_copy_net_key:
                if (copyTextToClipboard(et_net_key.getText().toString())) {
                    toastMsg("net key copied");
                }
                break;

            case R.id.iv_copy_app_key:
                if (copyTextToClipboard(et_app_key.getText().toString())) {
                    toastMsg("app key copied");
                }
                break;

            case R.id.iv_tip_default_bound:
                startActivity(
                        new Intent(this, TipsActivity.class)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_RES_ID, R.string.private_mode_tip)
                );
                break;
            case R.id.iv_tip_auto_provision:
                startActivity(
                        new Intent(this, TipsActivity.class)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_RES_ID, R.string.auto_provision_tip)
                );
                break;

            case R.id.iv_tip_remote_prov:
                startActivity(
                        new Intent(this, TipsActivity.class)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_RES_ID, R.string.remote_prov_tip)
                );
                break;

            case R.id.iv_tip_no_oob:
                startActivity(
                        new Intent(this, TipsActivity.class)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_RES_ID, R.string.use_no_oob_tip)
                );
                break;

            case R.id.tv_select_database:
                startActivity(new Intent(this, OOBInfoActivity.class));
                break;

            case R.id.btn_reset_mesh:
                showConfirmDialog("Wipe all mesh info? ", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        MeshService.getInstance().idle(true);
                        FUCacheService.getInstance().clear(SettingsActivity.this);
                        MeshInfo meshInfo = MeshInfo.createNewMesh(SettingsActivity.this);
                        TelinkMeshApplication.getInstance().setupMesh(meshInfo);
                        MeshService.getInstance().setupMeshNetwork(meshInfo.convertToConfiguration());
                        toastMsg("Wipe mesh info success");
                        showMeshInfo();
                    }
                });
                break;

            case R.id.iv_tip_fast_prov:
                startActivity(
                        new Intent(this, TipsActivity.class)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_RES_ID, R.string.fast_pv_tip)
                );
                break;

            case R.id.et_extend_type:
                showExtendDialog();
                break;

            case R.id.tv_cert:
                startActivity(new Intent(this, CertListActivity.class));
                break;
        }
    }

    private void showExtendDialog() {
        if (extendDialog == null) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setItems(EXTEND_TYPES, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    onExtendTypeSelect(which);
                    extendDialog.dismiss();
                }
            });
            builder.setTitle("Types");
            extendDialog = builder.create();
        }
        extendDialog.show();
    }

    private void onExtendTypeSelect(int position) {
        et_extend.setText(EXTEND_TYPES[position]);
        MeshService.getInstance().resetExtendBearerMode(ExtendBearerMode.values()[position]);
        SharedPreferenceHelper.setExtendBearerMode(SettingsActivity.this, ExtendBearerMode.values()[position]);
    }


    private boolean copyTextToClipboard(String text) {
        ClipboardManager clipboard = (ClipboardManager) getSystemService(Context.CLIPBOARD_SERVICE);
        ClipData clip = ClipData.newPlainText("com.telink.bluetooth.light", text);
        if (clipboard != null) {
            clipboard.setPrimaryClip(clip);
            return true;
        } else {
            return false;
        }
    }

}
