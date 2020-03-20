package com.telink.ble.mesh.ui;

import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
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
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.util.Arrays;

/**
 * Created by kee on 2018/9/18.
 */
public class SettingsActivity extends BaseActivity implements View.OnClickListener {

    private Switch switch_log, switch_private, switch_remote_prov;
    private EditText et_net_key, et_app_key;
    private MeshInfo mesh;
    private TextView tv_online_status;

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
//                TelinkMeshApplication.getInstance().setLogEnable(isChecked);

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
            }
        });

        enableBackNav(true);
        mesh = TelinkMeshApplication.getInstance().getMeshInfo();

        findViewById(R.id.iv_copy_net_key).setOnClickListener(this);
        findViewById(R.id.iv_copy_app_key).setOnClickListener(this);
        findViewById(R.id.iv_edit_net_key).setOnClickListener(this);
        findViewById(R.id.iv_edit_app_key).setOnClickListener(this);

        findViewById(R.id.iv_tip_remote_prov).setOnClickListener(this);
        et_net_key = findViewById(R.id.et_net_key);
        et_app_key = findViewById(R.id.et_app_key);
        et_net_key.setText(Arrays.bytesToHexString(mesh.networkKey, ""));
        et_app_key.setText(Arrays.bytesToHexString(mesh.appKeyList.get(0).key, ""));

        tv_online_status = findViewById(R.id.tv_online_status);
        tv_online_status.setText(AppSettings.ONLINE_STATUS_ENABLE ? R.string.online_status_enabled : R.string.online_status_disabled);
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
