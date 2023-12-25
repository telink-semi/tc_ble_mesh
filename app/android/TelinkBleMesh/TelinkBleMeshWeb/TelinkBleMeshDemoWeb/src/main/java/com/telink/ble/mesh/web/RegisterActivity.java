/********************************************************************************************************
 * @file ShareActivity.java
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

import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;

import com.google.android.material.textfield.TextInputEditText;
import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.activity.BaseActivity;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.IOException;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.Response;

/**
 * share
 */
public class RegisterActivity extends BaseActivity implements View.OnClickListener {
    private TextInputEditText tv_email, tv_code, tv_username, tv_pwd;
    private Button btn_get_code, btn_register;

    private int timeCounter;

    private Handler counterHandler = new Handler();

    public final static String URL_GET_CODE = TelinkHttpClient.URL_BASE + "user/getVerificationCode";

    public final static String URL_REGISTER = TelinkHttpClient.URL_BASE + "user/register";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_register);
        setTitle("Register");
        enableBackNav(true);
        initView();
    }

    private void initView() {
        tv_email = findViewById(R.id.tv_email);
        tv_code = findViewById(R.id.tv_code);
        tv_username = findViewById(R.id.tv_username);
        tv_pwd = findViewById(R.id.tv_pwd);

        btn_get_code = findViewById(R.id.btn_get_code);
        btn_register = findViewById(R.id.btn_register);
        btn_register.setOnClickListener(this);
        btn_get_code.setOnClickListener(this);
    }

    private void startCounter() {
        timeCounter = 60;
        counterHandler.post(COUNTER_TASK);
    }

    private void stopCounter() {
        counterHandler.removeCallbacksAndMessages(null);
    }

    private Runnable COUNTER_TASK = new Runnable() {
        @Override
        public void run() {
            timeCounter--;
            boolean enable = timeCounter <= 0;
            btn_get_code.setEnabled(enable);
            btn_get_code.setText(enable ? "GET CODE" : (timeCounter + "s"));
            if (enable) {
                stopCounter();
            } else {
                counterHandler.postDelayed(this, 1000);
            }
        }
    };

    private void getCode() {
        String emailInput = tv_email.getText().toString();
        if (TextUtils.isEmpty(emailInput)) {
            tv_email.setError("pls input email");
            return;
        }


        FormBody formBody = new FormBody.Builder()
                .add("mailAddress", emailInput)
                .add("usage", WebUtils.VERIFICATION_USAGE_REGISTER + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_GET_CODE, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                showError("network error");
                dismissIndeterminateLoading();
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, getBaseActivity());
                if (responseInfo == null) {
                    return;
                }
                runOnUiThread(() -> {
                    toastMsg("get code success");
                });

                startCounter();
            }
        });
    }

    private void register() {
        showIndeterminateLoading();
        String emailInput = tv_email.getText().toString();
        if (TextUtils.isEmpty(emailInput)) {
            tv_email.setError("pls input email");
            return;
        }

        String codeInput = tv_code.getText().toString();
        if (TextUtils.isEmpty(codeInput)) {
            tv_code.setError("pls input code");
            return;
        }


        String usernameInput = tv_username.getText().toString();
        if (TextUtils.isEmpty(usernameInput)) {
            tv_username.setError("pls input username");
            return;
        }


        String pwdInput = tv_pwd.getText().toString();
        if (TextUtils.isEmpty(pwdInput)) {
            tv_pwd.setError("pls input password");
            return;
        }

        FormBody formBody = new FormBody.Builder()
                .add("mailAddress", emailInput)
                .add("name", usernameInput)
                .add("verificationCode", codeInput)
                .add("password", WebUtils.sha256(pwdInput))
                .build();
        TelinkHttpClient.getInstance().post(URL_REGISTER, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.d("register request onFailure : " + e.getMessage());
                showError("network error");
                dismissIndeterminateLoading();
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, getBaseActivity());
                if (responseInfo == null) {
                    return;
                }
                onRegisterSuccess(usernameInput, pwdInput);
            }
        });
    }

    private void onRegisterSuccess(String username, String pwd) {
        SharedPreferenceHelper.setLoginInfo(this, username, pwd);
        runOnUiThread(() -> {
            showConfirmDialog("Register Success, back to login?", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    setResult(RESULT_OK);
                    finish();
                }
            });
        });
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_get_code:
                getCode();
                break;
            case R.id.btn_register:
                register();
                break;
        }
    }
}
