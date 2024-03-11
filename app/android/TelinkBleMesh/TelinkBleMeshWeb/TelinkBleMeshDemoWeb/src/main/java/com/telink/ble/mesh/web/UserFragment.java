/********************************************************************************************************
 * @file SettingFragment.java
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
package com.telink.ble.mesh.web;

import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONException;
import com.google.android.material.textfield.TextInputEditText;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.activity.BaseActivity;
import com.telink.ble.mesh.ui.activity.ChangePasswordActivity;
import com.telink.ble.mesh.ui.activity.LoginActivity;
import com.telink.ble.mesh.ui.activity.SettingsActivity;
import com.telink.ble.mesh.ui.fragment.BaseFragment;

import java.io.IOException;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.Response;

/**
 * setting
 */

public class UserFragment extends BaseFragment implements View.OnClickListener {

    private static final String URL_USER_LOGOUT = TelinkHttpClient.URL_BASE + "user/logout";

    private static final String URL_UPDATE_INFO = TelinkHttpClient.URL_BASE + "user/updateInfo";
    private TextView tv_nickname, tv_username;

    // input in dialog
    private TextInputEditText et_input;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_user, null);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        Toolbar toolbar = view.findViewById(R.id.title_bar);
        toolbar.setNavigationIcon(null);
        toolbar.inflateMenu(R.menu.setting_tab);
        toolbar.getMenu().findItem(R.id.item_version).setTitle(getVersion());
        setTitle(view, "User");

        view.findViewById(R.id.view_change_pwd).setOnClickListener(this);
        view.findViewById(R.id.view_manage_networks).setOnClickListener(this);
        view.findViewById(R.id.iv_edit_nickname).setOnClickListener(this);
        view.findViewById(R.id.view_logout).setOnClickListener(this);
        view.findViewById(R.id.view_settings).setOnClickListener(this);
        tv_nickname = view.findViewById(R.id.tv_nickname);
        tv_username = view.findViewById(R.id.tv_username);
        showUserInfo();
    }

    private void showUserInfo() {
        User user = TelinkMeshApplication.getInstance().getUser();
        String nickname = user.getNickname();
        tv_nickname.setText(nickname == null ? "[NULL]" : nickname);
        tv_username.setText(user.getName());
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.view_change_pwd:
                startActivity(new Intent(getActivity(), ChangePasswordActivity.class));
                break;
            case R.id.view_manage_networks:
                startActivity(new Intent(getActivity(), NetworkListActivity.class).putExtra(NetworkListActivity.EXTRA_FROM, NetworkListActivity.FROM_MAIN));
                break;
            case R.id.iv_edit_nickname:
                showEditNickNameDialog();
                break;

            case R.id.view_logout:
                ((BaseActivity) getActivity()).showConfirmDialog("User logout?", (dialog, which) -> logout());
                break;

            case R.id.view_settings:
                startActivity(new Intent(getActivity(), SettingsActivity.class));
                break;
        }
    }

    private void logout() {
        showIndeterminateLoading();
        TelinkHttpClient.getInstance().post(URL_USER_LOGOUT, new FormBody.Builder().build(), new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
            }
        });
        Intent intent = new Intent(getActivity(), LoginActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
        startActivity(intent);
        getActivity().finish();
    }


    private void showEditNickNameDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setTitle("Reset nickname ")
                .setView(R.layout.dialog_single_input)
//                .setMessage("Input Network Name")
                .setPositiveButton("Confirm", (dialog, which) -> updateNickname(et_input.getText().toString()))
                .setNegativeButton("Cancel", (dialog, which) -> dialog.cancel());
        AlertDialog dialog = builder.show();
        et_input = dialog.findViewById(R.id.et_input);
    }


    // apply node address from cloud
    private void updateNickname(String nickname) {
        if (TextUtils.isEmpty(nickname)) {
            showError("nickname input error");
            return;
        }
        FormBody formBody = new FormBody.Builder()
                .add("nickname", nickname)
                .build();
        TelinkHttpClient.getInstance().post(URL_UPDATE_INFO, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                showError("call fail");
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                ResponseInfo responseInfo = WebUtils.parseResponse(response, (BaseActivity) getActivity());
                if (responseInfo == null) {
                    return;
                }
                try {
                    User user = JSON.parseObject(responseInfo.data, User.class);
                    if (user != null) {
                        TelinkMeshApplication.getInstance().setUser(user);
                        showSuccess("update nickname success");
                        getActivity().runOnUiThread(() -> tv_nickname.setText(user.getNickname()));
                    }
                } catch (JSONException e) {
                    e.getStackTrace();
                    showError("json parse error");
                }
            }
        });
    }

    private String getVersion() {
        try {
            PackageManager manager = this.getActivity().getPackageManager();
            PackageInfo info = manager.getPackageInfo(this.getActivity().getPackageName(), 0);
            String version = info.versionName;
            return "V" + version;
        } catch (Exception e) {
            e.printStackTrace();
            return "";
        }
    }
}
