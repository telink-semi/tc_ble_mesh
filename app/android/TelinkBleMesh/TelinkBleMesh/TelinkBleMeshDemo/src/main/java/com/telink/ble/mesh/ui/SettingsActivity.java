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
import android.view.View;
import android.widget.EditText;
import android.widget.RadioGroup;
import android.widget.Switch;
import android.widget.TextView;

import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.networking.ExtendBearerMode;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.util.MeshLogger;

/**
 * Created by kee on 2018/9/18.
 */
public class SettingsActivity extends BaseActivity implements View.OnClickListener {

    private Switch switch_log, switch_private, switch_no_oob, switch_level;
    private EditText et_extend, et_net_key, et_app_key;
    private RadioGroup rg_pv_mode, rg_extend_bearer;
    private MeshInfo mesh;
    private TextView tv_online_status;
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

        if (!AppSettings.DRAFT_FEATURES_ENABLE) {
            findViewById(R.id.rb_pv_remote).setVisibility(View.GONE);
        }

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

        findViewById(R.id.btn_reset_settings).setOnClickListener(this);

        findViewById(R.id.iv_tip_log).setOnClickListener(this);
        findViewById(R.id.iv_tip_default_bound).setOnClickListener(this);
        findViewById(R.id.iv_tip_pv_mode).setOnClickListener(this);
        findViewById(R.id.iv_tip_level).setOnClickListener(this);

        findViewById(R.id.iv_tip_bearer).setOnClickListener(this);

        findViewById(R.id.iv_tip_no_oob).setOnClickListener(this);

        findViewById(R.id.iv_tip_online_status).setOnClickListener(this);

//        showMeshInfo();
        tv_online_status = findViewById(R.id.tv_online_status);
        tv_online_status.setText(AppSettings.ONLINE_STATUS_ENABLE ? R.string.online_status_enabled : R.string.online_status_disabled);

        loadSettings();
    }

    private void loadSettings() {
        switch_log.setChecked(SharedPreferenceHelper.isLogEnable(this));
        switch_private.setChecked(SharedPreferenceHelper.isPrivateMode(this));
        rg_pv_mode.check(getPvRbId());
        switch_no_oob.setChecked(SharedPreferenceHelper.isNoOOBEnable(this));
        switch_level.setChecked(SharedPreferenceHelper.isLevelServiceEnable(this));
        rg_extend_bearer.check(getExBrModeResId());
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

            case R.id.btn_reset_settings:
                showConfirmDialog("Reset all settings to default values? ", (dialog, which) -> {
                    SharedPreferenceHelper.resetAll(this);
                    loadSettings();
                    toastMsg("reset all setting success");
                });
                break;
        }
    }
}