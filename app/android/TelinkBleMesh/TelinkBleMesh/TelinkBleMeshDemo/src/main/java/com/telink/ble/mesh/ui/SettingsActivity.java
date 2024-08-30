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

import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Switch;
import android.widget.TextView;

import androidx.appcompat.app.AlertDialog;

import com.google.android.material.textfield.TextInputEditText;
import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.networking.ExtendBearerMode;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.util.MeshLogger;

/**
 * Created by kee on 2018/9/18.
 */
public class SettingsActivity extends BaseActivity implements View.OnClickListener {

    private Switch switch_log, switch_private, switch_no_oob, switch_level;
    private RadioGroup rg_pv_mode, rg_extend_bearer, rg_share_action;
    private TextView tv_online_status, tv_url;
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
        enableBackNav(true);
        switch_log = findViewById(R.id.switch_log);
        switch_log.setOnCheckedChangeListener((buttonView, isChecked) -> {
            SharedPreferenceHelper.setLogEnable(SettingsActivity.this, isChecked);
            MeshLogger.enableRecord(isChecked);
        });
//
        switch_private = findViewById(R.id.switch_private_mode);
        switch_private.setOnCheckedChangeListener((buttonView, isChecked) -> SharedPreferenceHelper.setPrivateMode(SettingsActivity.this, isChecked));

        rg_pv_mode = findViewById(R.id.rg_pv_mode);
        rg_pv_mode.setOnCheckedChangeListener((group, checkedId) -> updatePvMode(checkedId));

        switch_no_oob = findViewById(R.id.switch_no_oob);
        switch_no_oob.setOnCheckedChangeListener((buttonView, isChecked) -> SharedPreferenceHelper.setNoOOBEnable(SettingsActivity.this, isChecked));

        switch_level = findViewById(R.id.switch_level);
        switch_level.setOnCheckedChangeListener((buttonView, isChecked) -> {
                    SharedPreferenceHelper.setLevelServiceEnable(SettingsActivity.this, isChecked);
                    if (isChecked) {
                        if (TelinkMeshApplication.getInstance().getMeshInfo().extendGroups.size() == 0) {
                            TelinkMeshApplication.getInstance().getMeshInfo().addExtendGroups();
                        }
                    }
                }
        );


        rg_extend_bearer = findViewById(R.id.rg_extend_bearer);
        rg_extend_bearer.setOnCheckedChangeListener((group, checkedId) -> updateExBrMode(checkedId));

        rg_share_action = findViewById(R.id.rg_share_action);
        rg_share_action.setOnCheckedChangeListener((group, checkedId) -> updateShareAction(checkedId));


        findViewById(R.id.btn_reset_settings).setOnClickListener(this);

        findViewById(R.id.iv_tip_log).setOnClickListener(this);
        findViewById(R.id.iv_tip_default_bound).setOnClickListener(this);
        findViewById(R.id.iv_tip_pv_mode).setOnClickListener(this);
        findViewById(R.id.iv_tip_level).setOnClickListener(this);

        findViewById(R.id.iv_tip_bearer).setOnClickListener(this);

        findViewById(R.id.iv_tip_no_oob).setOnClickListener(this);

        findViewById(R.id.iv_tip_share_import).setOnClickListener(this);

        findViewById(R.id.iv_tip_online_status).setOnClickListener(this);

//        showMeshInfo();
        tv_online_status = findViewById(R.id.tv_online_status);
        tv_online_status.setText(AppSettings.ONLINE_STATUS_ENABLE ? R.string.online_status_enabled : R.string.online_status_disabled);


        findViewById(R.id.iv_tip_url).setOnClickListener(this);
        findViewById(R.id.iv_edit_url).setOnClickListener(this);
        tv_url = findViewById(R.id.tv_url);

        loadSettings();
    }

    private void loadSettings() {
        switch_log.setChecked(SharedPreferenceHelper.isLogEnable(this));
        switch_private.setChecked(SharedPreferenceHelper.isPrivateMode(this));
        ((RadioButton) rg_pv_mode.findViewById(getPvRbId())).setChecked(true);
        switch_no_oob.setChecked(SharedPreferenceHelper.isNoOOBEnable(this));
        switch_level.setChecked(SharedPreferenceHelper.isLevelServiceEnable(this));

        ((RadioButton) rg_extend_bearer.findViewById(getExBrModeResId())).setChecked(true);

        ((RadioButton) rg_share_action.findViewById(getShareActionResId())).setChecked(true);

        tv_url.setText(SharedPreferenceHelper.getBaseUrl(this));

    }

    private int getPvRbId() {
        int pvMode = SharedPreferenceHelper.getProvisionMode(this);
        switch (pvMode) {
            case SharedPreferenceHelper.PROVISION_MODE_NORMAL_SELECTABLE:
                return R.id.rb_pv_normal_sl;
            case SharedPreferenceHelper.PROVISION_MODE_NORMAL_AUTO:
                return R.id.rb_pv_normal_auto;
            case SharedPreferenceHelper.PROVISION_MODE_REMOTE:
                return R.id.rb_pv_remote;
            case SharedPreferenceHelper.PROVISION_MODE_FAST:
                return R.id.rb_pv_fast;
        }
        return R.id.rb_pv_normal_sl;
    }

    private void updatePvMode(int resId) {
        int mode = SharedPreferenceHelper.PROVISION_MODE_NORMAL_SELECTABLE;
        switch (resId) {
            case R.id.rb_pv_normal_sl:
                mode = SharedPreferenceHelper.PROVISION_MODE_NORMAL_SELECTABLE;
                break;

            case R.id.rb_pv_normal_auto:
                mode = SharedPreferenceHelper.PROVISION_MODE_NORMAL_AUTO;
                break;

            case R.id.rb_pv_remote:
                mode = SharedPreferenceHelper.PROVISION_MODE_REMOTE;
                break;
            case R.id.rb_pv_fast:
                mode = SharedPreferenceHelper.PROVISION_MODE_FAST;
                break;
        }
        SharedPreferenceHelper.setProvisionMode(this, mode);
    }

    private int getExBrModeResId() {
        ExtendBearerMode extendBearerMode = SharedPreferenceHelper.getExtendBearerMode(this);
        switch (extendBearerMode) {
            case NONE:
                return R.id.rb_ex_br_none;
            case GATT:
                return R.id.rb_ex_br_gatt;
            case GATT_ADV:
                return R.id.rb_ex_br_gatt_adv;
        }
        return R.id.rb_ex_br_none;
    }

    private void updateExBrMode(int resId) {
        ExtendBearerMode bearerMode = ExtendBearerMode.NONE;
        switch (resId) {
            case R.id.rb_ex_br_none:
                bearerMode = ExtendBearerMode.NONE;
                break;
            case R.id.rb_ex_br_gatt:
                bearerMode = ExtendBearerMode.GATT;
                break;
            case R.id.rb_ex_br_gatt_adv:
                bearerMode = ExtendBearerMode.GATT_ADV;
                break;
        }
        SharedPreferenceHelper.setExtendBearerMode(this, bearerMode);
        MeshService.getInstance().resetExtendBearerMode(bearerMode);
    }

    private void updateShareAction(int resId) {
        int action = SharedPreferenceHelper.IMPORT_COMPLETE_ACTION_DEFAULT;
        switch (resId) {
            case R.id.rb_share_action_default:
                action = SharedPreferenceHelper.IMPORT_COMPLETE_ACTION_DEFAULT;
                break;
            case R.id.rb_share_action_auto_switch:
                action = SharedPreferenceHelper.IMPORT_COMPLETE_ACTION_AUTO_SWITCH;
                break;
        }
        SharedPreferenceHelper.updateShareImportCompleteAction(this, action);
    }

    private int getShareActionResId() {
        int action = SharedPreferenceHelper.getShareImportCompleteAction(this);
        switch (action) {
            case SharedPreferenceHelper.IMPORT_COMPLETE_ACTION_DEFAULT:
                return R.id.rb_share_action_default;

            case SharedPreferenceHelper.IMPORT_COMPLETE_ACTION_AUTO_SWITCH:
                return R.id.rb_share_action_auto_switch;
        }
        return R.id.rb_share_action_default;
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.iv_tip_log:
                startActivity(
                        new Intent(this, TipsActivity.class)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_RES_ID, R.string.log_tip)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_SUB_TITLE, "Log")
                );
                break;
            case R.id.iv_tip_default_bound:
                startActivity(
                        new Intent(this, TipsActivity.class)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_RES_ID, R.string.private_mode_tip)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_SUB_TITLE, "Private Mode")
                );
                break;
            case R.id.iv_tip_pv_mode:
                startActivity(
                        new Intent(this, TipsActivity.class)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_RES_ID, R.string.provision_mode_tip)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_SUB_TITLE, "Provision Mode")
                );
                break;

            case R.id.iv_tip_level:
                startActivity(
                        new Intent(this, TipsActivity.class)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_RES_ID, R.string.sub_level_service_tip)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_SUB_TITLE, "Sub Level Service")
                );
                break;

            case R.id.iv_tip_no_oob:
                startActivity(
                        new Intent(this, TipsActivity.class)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_RES_ID, R.string.use_no_oob_tip)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_SUB_TITLE, "OOB")
                );
                break;

            case R.id.iv_tip_online_status:
                startActivity(
                        new Intent(this, TipsActivity.class)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_RES_ID, R.string.online_status_tip)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_SUB_TITLE, "Online Status")
                );
                break;
            case R.id.iv_tip_share_import:
                startActivity(
                        new Intent(this, TipsActivity.class)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_RES_ID, R.string.share_import_action_complete_tip)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_SUB_TITLE, "Share Complete Action")
                );
                break;

            case R.id.iv_tip_bearer:
                startActivity(
                        new Intent(this, TipsActivity.class)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_RES_ID, R.string.extend_bearer_mode_tip)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_SUB_TITLE, "Extend Bearer Mode")
                );
                break;

            case R.id.btn_reset_settings:
                showConfirmDialog("Reset all settings to default values? ", (dialog, which) -> {
                    SharedPreferenceHelper.resetAll(this);
                    loadSettings();
                    toastMsg("reset all setting success");
                });
                break;

            case R.id.iv_tip_url:
                startActivity(
                        new Intent(this, TipsActivity.class)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_RES_ID, R.string.base_url_tip)
                                .putExtra(TipsActivity.INTENT_KEY_TIP_SUB_TITLE, "Base URL")
                );
                break;

            case R.id.iv_edit_url:
                showUrlInputDialog();
                break;

        }
    }

    private void showUrlInputDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        View view = LayoutInflater.from(this).inflate(R.layout.dialog_single_input, null);
        final TextInputEditText et_input = view.findViewById(R.id.et_single_input);
        et_input.setText(SharedPreferenceHelper.getBaseUrl(this));
        et_input.setHint("please input url");
        builder.setTitle("Update Base URL");
        builder.setView(view).setPositiveButton("Confirm", (dialog, which) -> {
            dialog.dismiss();
            String url = et_input.getText().toString().trim();
            if (TextUtils.isEmpty(url)) {
                toastMsg("input empty");
                return;
            }
            SharedPreferenceHelper.setBaseUrl(this, url);
            tv_url.setText(url);
            toastMsg("save base url success ");
        }).setNegativeButton("Cancel", (dialog, which) -> dialog.dismiss());
        builder.show();
    }
}