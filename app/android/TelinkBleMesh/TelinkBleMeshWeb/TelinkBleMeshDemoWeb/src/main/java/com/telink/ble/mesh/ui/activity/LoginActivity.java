/********************************************************************************************************
 * @file LoginActivity.java
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
package com.telink.ble.mesh.ui.activity;

import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.view.Window;

import androidx.annotation.Nullable;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONException;
import com.google.android.material.textfield.TextInputEditText;
import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.NetworkListActivity;
import com.telink.ble.mesh.web.RegisterActivity;
import com.telink.ble.mesh.web.ResponseInfo;
import com.telink.ble.mesh.web.TelinkHttpClient;
import com.telink.ble.mesh.web.User;
import com.telink.ble.mesh.web.WebUtils;

import java.io.IOException;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.Response;

/**
 * share
 */
public class LoginActivity extends BaseActivity implements View.OnClickListener {
    private TextInputEditText tv_name, tv_pwd;

    public final static String URL_LOGIN = TelinkHttpClient.URL_BASE + "user/login";

    public final static int REQUEST_CODE_REGISTER = 1;

    public final static int REQUEST_CODE_RESET_PASSWORD = 2;

    private final static int MSG_LOGIN_SUCCESS = 1;

    private final static int MSG_LOGIN_FAIL = 2;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
        setContentView(R.layout.activity_login);
        initView();
    }

    private void initView() {
        setTitle("Login");
        enableBackNav(false);
        tv_name = findViewById(R.id.tv_name);
        tv_pwd = findViewById(R.id.tv_pwd);
        tv_name.setText(SharedPreferenceHelper.getLoginName(this));
        tv_pwd.setText(SharedPreferenceHelper.getLoginPassword(this));
        findViewById(R.id.btn_login).setOnClickListener(this);
        findViewById(R.id.btn_forget_pwd).setOnClickListener(this);
        findViewById(R.id.btn_register).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_login:
                login();
                break;
            case R.id.btn_register:
                startActivityForResult(new Intent(this, RegisterActivity.class), REQUEST_CODE_REGISTER);
                break;

            case R.id.btn_forget_pwd:
                startActivityForResult(new Intent(this, ForgetPasswordActivity.class), REQUEST_CODE_RESET_PASSWORD);
                break;
        }
    }

    private void login() {
        showIndeterminateLoading();
        String nameInput = tv_name.getText().toString().trim();
        String pwdInput = tv_pwd.getText().toString().trim();
        if (TextUtils.isEmpty(nameInput)) {
            tv_name.setError("pls input name");
            return;
        }

        if (TextUtils.isEmpty(pwdInput)) {
            tv_pwd.setError("pls input password");
            return;
        }

        FormBody formBody = new FormBody.Builder()
                .add("username", nameInput)
                .add("password", WebUtils.sha256(pwdInput))
                .build();
        TelinkHttpClient.getInstance().post(URL_LOGIN, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                onLoginFail("call fail");
                e.printStackTrace();
                MeshLogger.d(e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                ResponseInfo responseInfo = WebUtils.parseResponse(response, getBaseActivity());
                if (responseInfo == null) {
                    onLoginFail("login fail: server response error");
                    return;
                }
                try {
                    User user = JSON.parseObject(responseInfo.data, User.class);
                    MeshLogger.d("user: " + user.toString());
                    TelinkMeshApplication.getInstance().setUser(user);
                    onLoginSuccess(nameInput, pwdInput, user);
                } catch (JSONException e) {
                    e.getStackTrace();
                    onLoginFail("user info parse error");
                }
            }
        });
    }


    private void onLoginFail(String message) {
        dismissIndeterminateLoading();
        showError(message);
    }

    private void onLoginSuccess(String name, String password, User user) {
        dismissIndeterminateLoading();
        runOnUiThread(() -> toastMsg("login success"));
        SharedPreferenceHelper.setLoginInfo(this, name, password);
        checkNetworkIdCache(user);
    }


    private void checkNetworkIdCache(User user) {
        int networkId = SharedPreferenceHelper.getNetworkId(this, user.getId());
//        networkId = 0; //  for test
        if (networkId == 0) {
            startActivity(new Intent(this, NetworkListActivity.class));
            finish();
        } else {
            gotoMainPage();
        }
    }

    private void gotoMainPage() {
        Intent intent = new Intent(this, MainActivity.class);
        startActivity(intent);
        finish();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode == RESULT_OK && (requestCode == REQUEST_CODE_REGISTER || requestCode == REQUEST_CODE_RESET_PASSWORD)) {
            tv_name.setText(SharedPreferenceHelper.getLoginName(this));
            tv_pwd.setText(SharedPreferenceHelper.getLoginPassword(this));
        }
    }
}
