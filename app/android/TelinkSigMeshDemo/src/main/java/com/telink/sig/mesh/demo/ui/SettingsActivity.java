/********************************************************************************************************
 * @file SceneSettingActivity.java
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
package com.telink.sig.mesh.demo.ui;

import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Switch;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.SharedPreferenceHelper;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.model.Mesh;
import com.telink.sig.mesh.util.Arrays;

/**
 * 场景配置
 * Created by kee on 2018/9/18.
 */
public class SettingsActivity extends BaseActivity implements View.OnClickListener {

    private Switch switch_log, switch_private, switch_remote_prov, switch_fast_prov;
    private EditText et_net_key, et_app_key;
    private Mesh mesh;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);
        setTitle("Settings");
        switch_log = findViewById(R.id.switch_log);
        switch_log.setChecked(SharedPreferenceHelper.isLogEnable(this));
        switch_log.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                SharedPreferenceHelper.setLogEnable(SettingsActivity.this, isChecked);
                TelinkMeshApplication.getInstance().setLogEnable(isChecked);

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

        enableBackNav(true);
        mesh = TelinkMeshApplication.getInstance().getMesh();

        findViewById(R.id.iv_copy_net_key).setOnClickListener(this);
        findViewById(R.id.iv_copy_app_key).setOnClickListener(this);
        findViewById(R.id.iv_edit_net_key).setOnClickListener(this);
        findViewById(R.id.iv_edit_app_key).setOnClickListener(this);

        findViewById(R.id.iv_tip_remote_prov).setOnClickListener(this);
        findViewById(R.id.iv_tip_fast_prov).setOnClickListener(this);
        et_net_key = findViewById(R.id.et_net_key);
        et_app_key = findViewById(R.id.et_app_key);
        et_net_key.setText(Arrays.bytesToHexString(mesh.networkKey, ""));
        et_app_key.setText(Arrays.bytesToHexString(mesh.appKey, ""));
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
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

            case R.id.iv_edit_net_key:
                break;
            case R.id.iv_edit_app_key:
                break;

            case R.id.iv_tip_remote_prov:
                startActivity(
                        new Intent(this, ShareTipActivity.class)
                                .putExtra(ShareTipActivity.INTENT_KEY_TIP_RES_ID, R.string.remote_prov_tip)
                );
                break;

            case R.id.iv_tip_fast_prov:
                startActivity(
                        new Intent(this, ShareTipActivity.class)
                                .putExtra(ShareTipActivity.INTENT_KEY_TIP_RES_ID, R.string.fast_prov_tip)
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
