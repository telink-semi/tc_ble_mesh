/********************************************************************************************************
 * @file ShareApplicantsActivity.java
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
import android.widget.TextView;

import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONException;
import com.google.android.material.bottomsheet.BottomSheetDialog;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.web.MeshNetworkShareInfo;
import com.telink.ble.mesh.model.web.NetworkShareState;
import com.telink.ble.mesh.ui.adapter.ShareInfoAdapter;
import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.ResponseInfo;
import com.telink.ble.mesh.web.TelinkHttpClient;
import com.telink.ble.mesh.web.WebUtils;

import java.io.IOException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.Response;

/**
 * 申请列表（显示用户信息）
 * 可以对每个用户修改操作权限
 */
public class ShareApplicantsActivity extends BaseActivity implements View.OnClickListener {

    private final static String URL_GET_INFO_LIST = TelinkHttpClient.URL_BASE + "mesh-network/getShareInfoList";

    private final static String URL_UPDATE_SHARE_INFO_STATE = TelinkHttpClient.URL_BASE + "mesh-network/updateShareInfoState";

    private int meshNetworkId;

    private ShareInfoAdapter adapter;

    private BottomSheetDialog shareInfoBottomDialog;

    private TextView tv_state, tv_member, tv_visitor, tv_forbidden;

    private int lastPosition;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_share_applicants);
        setTitle("Applicants");
        enableBackNav(true);
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.menu_single);
        toolbar.setOnMenuItemClickListener(item -> {
            getApplicants();
            return false;
        });

        meshNetworkId = getIntent().getIntExtra("networkId", 0);

        adapter = new ShareInfoAdapter(this, this::showInfoDetail);
        adapter.setOnItemLongClickListener(position -> {
            showInfoDetail(position);
            return false;
        });
        RecyclerView rv_share_info = findViewById(R.id.rv_share_info);
        rv_share_info.setLayoutManager(new LinearLayoutManager(this));
        rv_share_info.setAdapter(adapter);
        initBottomDialog();
        getApplicants();
    }


    private void showInfoDetail(int position) {
        if (lastPosition != position) {
            lastPosition = position;
        }
        MeshNetworkShareInfo shareInfo = adapter.get(position);

        NetworkShareState state = NetworkShareState.getByState(shareInfo.getState());
        if (state == null) {
            tv_state.setText("ERROR");
            shareInfoBottomDialog.show();
            return;
        }
        tv_state.setText("State: " + state);
        tv_state.setTextColor(getResources().getColor(NetworkShareState.getColor(shareInfo.getState())));
        tv_member.setVisibility(state == NetworkShareState.MEMBER ? View.GONE : View.VISIBLE);
        tv_visitor.setVisibility(state == NetworkShareState.VISITOR ? View.GONE : View.VISIBLE);
        tv_forbidden.setVisibility(state == NetworkShareState.FORBIDDEN ? View.GONE : View.VISIBLE);

        shareInfoBottomDialog.show();
    }


    private void initBottomDialog() {
        shareInfoBottomDialog = new BottomSheetDialog(this, R.style.BottomSheetDialog);
        shareInfoBottomDialog.setCancelable(true);
        shareInfoBottomDialog.setContentView(R.layout.dialog_bottom_share_info_action);

        tv_state = shareInfoBottomDialog.findViewById(R.id.tv_state);
        tv_member = shareInfoBottomDialog.findViewById(R.id.tv_member);
        tv_visitor = shareInfoBottomDialog.findViewById(R.id.tv_visitor);
        tv_forbidden = shareInfoBottomDialog.findViewById(R.id.tv_forbidden);

        tv_state.setOnClickListener(this);
        tv_member.setOnClickListener(this);
        tv_visitor.setOnClickListener(this);
        tv_forbidden.setOnClickListener(this);
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tv_member:
                updateShareInfoState(NetworkShareState.MEMBER.state);
                break;

            case R.id.tv_visitor:
                updateShareInfoState(NetworkShareState.VISITOR.state);
                break;

            case R.id.tv_forbidden:
                updateShareInfoState(NetworkShareState.FORBIDDEN.state);
                break;
        }
        shareInfoBottomDialog.dismiss();
    }


    public void getApplicants() {
        showIndeterminateLoading();
        Map<String, String> params = new HashMap<>();
        params.put("networkId", meshNetworkId + "");
        TelinkHttpClient.getInstance().get(URL_GET_INFO_LIST, params, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.d("create network call onFailure");
                dismissIndeterminateLoading();
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, getBaseActivity());
                if (responseInfo == null) return;
                try {
                    List<MeshNetworkShareInfo> list = JSON.parseArray(responseInfo.data, MeshNetworkShareInfo.class);
                    runOnUiThread(() -> adapter.resetData(list));
                } catch (JSONException e) {
                    e.printStackTrace();
                    showError("network parse json error");
                }
            }
        });
    }

    public void updateShareInfoState(int state) {
        showIndeterminateLoading();
        MeshNetworkShareInfo shareInfo = adapter.get(lastPosition);

        FormBody formBody = new FormBody.Builder()
                .add("shareInfoId", shareInfo.getId() + "")
                .add("state", state + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_UPDATE_SHARE_INFO_STATE, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.d("updateShareInfoState onFailure");
                dismissIndeterminateLoading();
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, getBaseActivity());
                if (responseInfo == null) {
                    return;
                }
                MeshLogger.d("updateShareInfoState response");
                runOnUiThread(() -> adapter.updateState(lastPosition, state));
            }
        });

    }

}
