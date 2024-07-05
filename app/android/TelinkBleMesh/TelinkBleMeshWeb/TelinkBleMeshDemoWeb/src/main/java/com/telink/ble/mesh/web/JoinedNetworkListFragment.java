/********************************************************************************************************
 * @file JoinedNetworkListFragment.java
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

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONException;
import com.google.android.material.bottomsheet.BottomSheetDialog;
import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.web.MeshNetworkShareInfo;
import com.telink.ble.mesh.model.web.NetworkShareState;
import com.telink.ble.mesh.ui.activity.BaseActivity;
import com.telink.ble.mesh.ui.activity.MainActivity;
import com.telink.ble.mesh.ui.fragment.BaseFragment;
import com.telink.ble.mesh.ui.qrcode.NetworkQrCodeInfo;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.IOException;
import java.util.List;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.Response;

/**
 * select network or create new network
 * show created networks  and joined networks
 */
public class JoinedNetworkListFragment extends BaseFragment implements View.OnClickListener {
    public final static String URL_GET_JOINED_NETWORK_LIST = TelinkHttpClient.URL_BASE + "mesh-network/getJoinedList";

    public final static String URL_JOIN_NETWORK = TelinkHttpClient.URL_BASE + "mesh-network/joinNetworkFromLink";

    public final static String URL_LEAVE_NETWORK = TelinkHttpClient.URL_BASE + "mesh-network/leaveNetwork";

    private final static int REQUEST_CODE_SCAN = 0x01;

    private ShareInfoApplyAdapter adapter;

    private BottomSheetDialog shareInfoActionDialog;

    /**
     * actions in bottom dialog
     */
    private TextView tv_dialog_title, tv_switch; // tv_show_detail,
    private int selectedPosition = -1;

    @SuppressWarnings("HandlerLeak")
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
        }
    };


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_joined_network_list, null);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        initView(view);
        getJoinedNetworkList();
    }

    private void initView(View view) {
        ll_empty = view.findViewById(R.id.ll_empty);
        FloatingActionButton fab_scan = view.findViewById(R.id.fab_scan);
        fab_scan.setOnClickListener(this);
        RecyclerView rv_share_info = view.findViewById(R.id.rv_share_info);
        rv_share_info.setLayoutManager(new LinearLayoutManager(getActivity()));
        adapter = new ShareInfoApplyAdapter(getActivity(), this::showBottomDialog);
        adapter.setOnItemClickListener(position -> {
//                gotoNetworkDetail();
        });

        adapter.setOnItemLongClickListener(position -> {
            showBottomDialog(position);
            return false;
        });
        rv_share_info.setAdapter(adapter);
        initBottomDialog();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.iv_close:
                shareInfoActionDialog.dismiss();
                break;
            case R.id.tv_switch:
                // 切换到该网络
                switchToNetwork();
                shareInfoActionDialog.dismiss();
                break;
            case R.id.btn_leave:
                showLeaveDialog();
                shareInfoActionDialog.dismiss();
                break;

            case R.id.fab_scan:
                startActivityForResult(new Intent(getActivity(), QRCodeScanActivity.class), REQUEST_CODE_SCAN);
                break;
        }
    }

    /**
     * switch to network
     */
    private void switchToNetwork() {
        int meshNetworkId = adapter.get(selectedPosition).getNetworkId();
        int userId = TelinkMeshApplication.getInstance().getUser().getId();
        SharedPreferenceHelper.setNetworkId(getActivity(), userId, meshNetworkId);
        gotoMainPage();
    }

    private void initBottomDialog() {
        shareInfoActionDialog = new BottomSheetDialog(getActivity(), R.style.BottomSheetDialog);
        shareInfoActionDialog.setCancelable(true);
        shareInfoActionDialog.setContentView(R.layout.dialog_bottom_share_info_apply);
        tv_dialog_title = shareInfoActionDialog.findViewById(R.id.tv_dialog_title);

        tv_switch = shareInfoActionDialog.findViewById(R.id.tv_switch);
        shareInfoActionDialog.findViewById(R.id.iv_close).setOnClickListener(this);
        shareInfoActionDialog.findViewById(R.id.btn_leave).setOnClickListener(this);
        tv_switch.setOnClickListener(this);
    }

    private void showBottomDialog(int position) {
        this.selectedPosition = position;
        MeshNetworkShareInfo shareInfo = adapter.get(position);
        String title = "select action for : " + adapter.get(position).getNetworkName();
        tv_dialog_title.setText(title);
        NetworkShareState st = NetworkShareState.getByState(shareInfo.getState());
        if (st == NetworkShareState.VISITOR || st == NetworkShareState.MEMBER) {
            tv_switch.setVisibility(View.VISIBLE);
        } else {
            tv_switch.setVisibility(View.GONE);
        }

        shareInfoActionDialog.show();
    }

    private void showLeaveDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setTitle("Warning")
                .setMessage("Leave Mesh Network?")
                .setPositiveButton("Confirm", (dialog, which) -> leaveNetwork())
                .setNegativeButton("Cancel", (dialog, which) -> dialog.cancel());
        builder.show();
    }

    private void joinNetwork(NetworkQrCodeInfo result) {
        showIndeterminateLoading();
        FormBody formBody = new FormBody.Builder()
                .add("shareLinkId", result.shareLinkId + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_JOIN_NETWORK, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.d("call onFailure : " + call.request().url());
                dismissIndeterminateLoading();
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, (BaseActivity) getActivity());
                if (responseInfo == null) {
                    return;
                }
                MeshLogger.d("join network success");
                try {
                    MeshNetworkShareInfo shareInfo = JSON.parseObject(responseInfo.data, MeshNetworkShareInfo.class);
                    getActivity().runOnUiThread(() -> {
                        adapter.addInfoItem(shareInfo);
                        updateEmptyVisibility(false);
                    });
                } catch (
                        JSONException e) {
                    e.getStackTrace();
                }
            }
        });
    }

    private void leaveNetwork() {
        MeshNetworkShareInfo shareInfo = adapter.get(selectedPosition);
        FormBody formBody = new FormBody.Builder()
                .add("shareInfoId", shareInfo.getId() + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_LEAVE_NETWORK, formBody, new Callback() {
            @Override
            public void onFailure(@NonNull Call call, @NonNull IOException e) {
                MeshLogger.d("delete network call onFailure");
                dismissIndeterminateLoading();
            }

            @Override
            public void onResponse(@NonNull Call call, @NonNull Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, (BaseActivity) getActivity());
                if (responseInfo == null) {
                    return;
                }
                getActivity().runOnUiThread(() -> {
                    adapter.remove(selectedPosition);
                    updateEmptyVisibility(adapter.getItemCount() == 0);
                });
            }
        });
    }


    public void getJoinedNetworkList() {
        showIndeterminateLoading();
        TelinkHttpClient.getInstance().get(URL_GET_JOINED_NETWORK_LIST, null, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                dismissIndeterminateLoading();
                MeshLogger.e("call fail");
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, (BaseActivity) getActivity());
                if (responseInfo == null) return;

                if (!responseInfo.isSuccess()) {
                    showError(responseInfo.message);
                    return;
                }

                try {
                    List<MeshNetworkShareInfo> list = JSON.parseArray(responseInfo.data, MeshNetworkShareInfo.class);
                    getActivity().runOnUiThread(() -> {
                        adapter.resetData(list);
                        updateEmptyVisibility(adapter.getItemCount() == 0);
                    });
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    private void gotoMainPage() {
        Intent intent = new Intent(getActivity(), MainActivity.class);
        startActivity(intent);
        getActivity().finish();
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode == Activity.RESULT_OK && requestCode == REQUEST_CODE_SCAN && data != null) {
            NetworkQrCodeInfo scanResult =
                    (NetworkQrCodeInfo) data.getSerializableExtra(QRCodeScanActivity.QR_SCAN_RESULT);
            if (scanResult == null) return;
            String title = "join network?" + "\n\tname: " + scanResult.networkName
                    + "\n\tcreator: " + scanResult.userName;
            ((BaseActivity) getActivity()).showConfirmDialog(title, (dialog, which) -> joinNetwork(scanResult));
        }
    }
}
