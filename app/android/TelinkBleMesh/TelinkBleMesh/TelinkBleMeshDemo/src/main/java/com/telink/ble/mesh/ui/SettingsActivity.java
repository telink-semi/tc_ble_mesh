/********************************************************************************************************
 * @file SettingsActivity.java
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

import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

/**
 * Created by kee on 2018/9/18.
 */
public class SettingsActivity extends BaseActivity implements View.OnClickListener {

    private Switch switch_log, switch_private, switch_remote_prov, switch_fast_prov, switch_no_oob, switch_dle, switch_auto_provision;
    private EditText et_net_key, et_app_key;
    private MeshInfo mesh;
    private TextView tv_online_status;


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
            findViewById(R.id.line_rp).setVisibility(View.VISIBLE);
        } else {
            findViewById(R.id.ll_rp).setVisibility(View.GONE);
            findViewById(R.id.line_rp).setVisibility(View.GONE);
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

        switch_dle = findViewById(R.id.switch_dle);
        switch_dle.setChecked(SharedPreferenceHelper.isDleEnable(this));
        switch_dle.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                MeshService.getInstance().resetDELState(isChecked);
                SharedPreferenceHelper.setDleEnable(SettingsActivity.this, isChecked);
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
        findViewById(R.id.iv_tip_dle).setOnClickListener(this);
        findViewById(R.id.iv_tip_fast_prov).setOnClickListener(this);

        et_net_key = findViewById(R.id.et_net_key);
        et_app_key = findViewById(R.id.et_app_key);
        showMeshInfo();
        tv_online_status = findViewById(R.id.tv_online_status);
        tv_online_status.setText(AppSettings.ONLINE_STATUS_ENABLE ? R.string.online_status_enabled : R.string.online_status_disabled);
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
                        MeshInfo meshInfo = MeshInfo.createNewMesh(SettingsActivity.this);
                        TelinkMeshApplication.getInstance().setupMesh(meshInfo);
                        MeshService.getInstance().setupMeshNetwork(meshInfo.convertToConfiguration());
                        toastMsg("Wipe mesh info success");
                        showMeshInfo();
                    }
                });
                break;

            case R.id.iv_tip_dle:
                startActivity(
                        new Intent(this, TipsActivity.class)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_RES_ID, R.string.dle_tip)
                );
                break;

            case R.id.iv_tip_fast_prov:
                startActivity(
                        new Intent(this, TipsActivity.class)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_RES_ID, R.string.fast_pv_tip)
                );
                break;
        }
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
