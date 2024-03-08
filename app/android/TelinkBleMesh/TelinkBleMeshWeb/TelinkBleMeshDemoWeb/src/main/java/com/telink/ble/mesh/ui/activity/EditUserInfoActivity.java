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
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONException;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
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
 * 编辑个人信息
 */
public class EditUserInfoActivity extends BaseActivity implements View.OnClickListener {
    private TextView tv_email, tv_name;
    private EditText et_nickname, et_phone, et_company, et_sign;

    private Button btn_save_user_info;

    private final static String URL_UPDATE_USER_INFO = TelinkHttpClient.URL_BASE + "user/updateInfo";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_edit_user_info);
        setTitle("User Info");
        enableBackNav(true);
        initView();
    }

    private void initView() {
        tv_email = findViewById(R.id.tv_email);
        tv_name = findViewById(R.id.tv_name);
        et_nickname = findViewById(R.id.et_nickname);
        et_phone = findViewById(R.id.et_phone);

        et_company = findViewById(R.id.et_company);
        et_sign = findViewById(R.id.et_sign);
        btn_save_user_info = findViewById(R.id.btn_save_user_info);
        btn_save_user_info.setOnClickListener(this);
    }

    private void updateUserInfo() {
        showIndeterminateLoading();
        String nicknameInput = et_nickname.getText().toString();
        String phoneInput = et_phone.getText().toString();
        String companyInput = et_company.getText().toString();
        String signInput = et_sign.getText().toString();

        FormBody formBody = new FormBody.Builder()
                .add("nickname", nicknameInput)
                .add("phone", phoneInput)
                .add("company", companyInput)
                .add("sign", signInput)
                .build();
        TelinkHttpClient.getInstance().post(URL_UPDATE_USER_INFO, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.d("register request onFailure : " + e.getMessage());
                toastMsg("network error");
                dismissIndeterminateLoading();
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, getBaseActivity());
                if (responseInfo == null) {
                    return;
                }
                try {
                    User user = JSON.parseObject(responseInfo.data, User.class);
                    TelinkMeshApplication.getInstance().setUser(user);
                } catch (JSONException e) {
                    e.getStackTrace();
                }
            }
        });
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_save:
                updateUserInfo();
                break;
        }
    }
}
