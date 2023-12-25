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
package com.telink.ble.mesh.ui.activity;

import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;

import com.google.android.material.textfield.TextInputEditText;
import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.ResponseInfo;
import com.telink.ble.mesh.web.TelinkHttpClient;
import com.telink.ble.mesh.web.WebUtils;

import java.io.IOException;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.Response;

/**
 * reset password by verify code
 */
public class ForgetPasswordActivity extends BaseActivity implements View.OnClickListener {

    public final static String URL_RESET_PASSWORD = TelinkHttpClient.URL_BASE + "user/resetPassword";

    public final static String URL_GET_CODE = TelinkHttpClient.URL_BASE + "user/getVerificationCode";

    private TextInputEditText tv_email, tv_code, tv_password;

    private Button btn_get_code, btn_confirm;

    private int timeCounter;

    private Handler counterHandler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_forget_password);
        setTitle("Forget Password");
        enableBackNav(true);
        initView();
    }

    private void initView() {
        tv_email = findViewById(R.id.tv_email);
        tv_code = findViewById(R.id.tv_code);
        tv_password = findViewById(R.id.tv_password);

        btn_get_code = findViewById(R.id.btn_get_code);
        btn_confirm = findViewById(R.id.btn_confirm);
        btn_confirm.setOnClickListener(this);
        btn_get_code.setOnClickListener(this);
    }

    private void getCode() {
        String emailInput = tv_email.getText().toString();
        if (TextUtils.isEmpty(emailInput)) {
            tv_email.setError("pls input email");
            return;
        }


        FormBody formBody = new FormBody.Builder()
                .add("mailAddress", emailInput)
                .add("usage", WebUtils.VERIFICATION_USAGE_RESET_PASSWORD + "")
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

    private void resetPwd() {
        showIndeterminateLoading();
        String emailInput = tv_email.getText().toString().trim();
        String codeInput = tv_code.getText().toString().trim();
        String passwordInput = tv_password.getText().toString().trim();
        if (TextUtils.isEmpty(emailInput)) {
            tv_email.setError("pls input email");
            return;
        }

        if (TextUtils.isEmpty(codeInput)) {
            tv_code.setError("pls input verification code");
            return;
        }

        if (TextUtils.isEmpty(passwordInput)) {
            tv_password.setError("pls input password");
            return;
        }
        FormBody formBody = new FormBody.Builder()
                .add("mailAddress", emailInput)
                .add("verificationCode", codeInput)
                .add("newPassword", WebUtils.sha256(passwordInput))
                .build();
        TelinkHttpClient.getInstance().post(URL_RESET_PASSWORD, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                dismissIndeterminateLoading();
                showError("call fail");
                e.printStackTrace();
                MeshLogger.d(e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, getBaseActivity());
                if (responseInfo == null) {
                    showError("reset fail: server response error");
                    return;
                }
                MeshLogger.d("responseInfo - " + responseInfo.toString());
                onResetSuccess(emailInput, passwordInput);
            }
        });
    }

    private void onResetSuccess(String username, String pwd) {
        SharedPreferenceHelper.setLoginInfo(this, username, pwd);
        runOnUiThread(() -> {
            showConfirmDialog("Reset password success, back to login?", (dialog, which) -> {
                setResult(RESULT_OK);
                finish();
            });
        });
    }


    @Override
    public void onClick(View v) {
        if (v == btn_get_code) {
            getCode();
        } else if (v == btn_confirm) {
            resetPwd();
        }
    }
}
