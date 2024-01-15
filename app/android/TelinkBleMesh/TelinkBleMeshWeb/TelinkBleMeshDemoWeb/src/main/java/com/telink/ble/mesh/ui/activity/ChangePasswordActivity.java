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
import android.text.InputType;
import android.text.TextUtils;
import android.view.View;
import android.view.Window;

import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.widget.DrawableClickableEditText;
import com.telink.ble.mesh.util.MeshLogger;
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
 * change password by old password
 */
public class ChangePasswordActivity extends BaseActivity implements View.OnClickListener {
    private DrawableClickableEditText tv_old_pwd, tv_new_pwd;

    public final static String URL_UPDATE_PASSWORD = TelinkHttpClient.URL_BASE + "user/updatePassword";

    private boolean oldPwdVisible = false, newPwdVisible = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
        setContentView(R.layout.activity_change_password);
        initView();
    }

    private void initView() {
        setTitle("Change Password");
        enableBackNav(true);
        tv_old_pwd = findViewById(R.id.tv_old_pwd);
        tv_old_pwd.setDrawableClickListener(() -> {
            oldPwdVisible = !oldPwdVisible;
            tv_old_pwd.setInputType(InputType.TYPE_CLASS_TEXT | (oldPwdVisible ? InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD : InputType.TYPE_TEXT_VARIATION_PASSWORD));
            tv_old_pwd.setCompoundDrawablesWithIntrinsicBounds(0, 0, oldPwdVisible ? R.drawable.ic_eye_open : R.drawable.ic_eye_close, 0);
            tv_old_pwd.setSelection(tv_old_pwd.getText().length());
        });


        tv_new_pwd = findViewById(R.id.tv_new_pwd);
        tv_new_pwd.setDrawableClickListener(() -> {
            newPwdVisible = !newPwdVisible;
            tv_new_pwd.setInputType(InputType.TYPE_CLASS_TEXT | (newPwdVisible ? InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD : InputType.TYPE_TEXT_VARIATION_PASSWORD));
//            tv_new_pwd.setInputType(newPwdVisible ? InputType.TYPE_CLASS_TEXT : InputType.TYPE_TEXT_VARIATION_PASSWORD);
            tv_new_pwd.setCompoundDrawablesWithIntrinsicBounds(0, 0, newPwdVisible ? R.drawable.ic_eye_open : R.drawable.ic_eye_close, 0);
            tv_new_pwd.setSelection(tv_new_pwd.getText().length());
        });

        findViewById(R.id.btn_change_pwd).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_change_pwd) {
            changePwd();
        }
    }

    private void changePwd() {
        showIndeterminateLoading();
        String oldInput = tv_old_pwd.getText().toString().trim();
        String newInput = tv_new_pwd.getText().toString().trim();
        if (TextUtils.isEmpty(oldInput)) {
            tv_old_pwd.setError("pls input old password");
            return;
        }

        if (TextUtils.isEmpty(newInput)) {
            tv_new_pwd.setError("pls input new password");
            return;
        }
        FormBody formBody = new FormBody.Builder()
                .add("oldPassword", WebUtils.sha256(oldInput))
                .add("newPassword", WebUtils.sha256(newInput))
                .build();
        TelinkHttpClient.getInstance().post(URL_UPDATE_PASSWORD, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                onUpdateFail("call fail: " + call.request().url());
                e.printStackTrace();
                MeshLogger.d(e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                ResponseInfo responseInfo = WebUtils.parseResponse(response, getBaseActivity(), false);
                if (responseInfo == null) {
                    onUpdateFail("update fail: server response error");
                    return;
                }
                MeshLogger.d("responseInfo - " + responseInfo.toString());
                if (responseInfo.isSuccess()) {
                    onUpdateSuccess(newInput);
                } else {
                    if (!responseInfo.isAuthErr()) {
                        showError(responseInfo.message);
                    }
                }
            }
        });
    }

    private void onUpdateFail(String message) {
        dismissIndeterminateLoading();
        showError(message);
    }

    private void onUpdateSuccess(String password) {
        dismissIndeterminateLoading();
        runOnUiThread(() -> toastMsg("update password success"));
        User user = TelinkMeshApplication.getInstance().getUser();
        SharedPreferenceHelper.setLoginInfo(this, user.getName(), password);
    }
}
