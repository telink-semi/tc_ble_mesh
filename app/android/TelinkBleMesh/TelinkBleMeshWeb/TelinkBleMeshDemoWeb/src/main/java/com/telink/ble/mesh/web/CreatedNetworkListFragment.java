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

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONException;
import com.google.android.material.bottomsheet.BottomSheetDialog;
import com.google.android.material.textfield.TextInputEditText;
import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.activity.BaseActivity;
import com.telink.ble.mesh.ui.activity.MainActivity;
import com.telink.ble.mesh.ui.activity.ShareApplicantsActivity;
import com.telink.ble.mesh.ui.activity.ShareExportActivity;
import com.telink.ble.mesh.ui.fragment.BaseFragment;
import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.entity.MeshNetwork;

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
public class CreatedNetworkListFragment extends BaseFragment implements View.OnClickListener {
    public final static String URL_GET_NETWORK_LIST = TelinkHttpClient.URL_BASE + "mesh-network/getCreatedList";

    public final static String URL_CREATE_NETWORK = TelinkHttpClient.URL_BASE + "mesh-network/create";

    public final static String URL_DELETE_NETWORK = TelinkHttpClient.URL_BASE + "mesh-network/delete";

    private MeshNetworkAdapter adapter;

//    private List<MeshNetwork> networkList;

    private TextInputEditText et_network_name = null;

    private BottomSheetDialog networkActionDialog;

    /**
     * actions in bottom dialog
     */
    private TextView tv_dialog_title;

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
        return inflater.inflate(R.layout.fragment_created_network_list, null);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        initView(view);
        getNetworkList();
    }

    private void initView(View view) {
        ll_empty = view.findViewById(R.id.ll_empty);
        view.findViewById(R.id.fab_network_list).setOnClickListener(this);
        RecyclerView rv_network = view.findViewById(R.id.rv_network);
        rv_network.setLayoutManager(new LinearLayoutManager(getActivity()));
        adapter = new MeshNetworkAdapter(getActivity(), this::showBottomDialog);

        adapter.setOnItemLongClickListener(position -> {
            showBottomDialog(position);
            return false;
        });
        rv_network.setAdapter(adapter);
        initBottomDialog();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.fab_network_list:
                showCreateNetworkDialog();
                break;
            case R.id.iv_close:
                networkActionDialog.dismiss();
                break;
            case R.id.tv_share:
                gotoNetworkShare();
                networkActionDialog.dismiss();
                break;
            case R.id.tv_applicant:
                gotoApplicants();
                networkActionDialog.dismiss();
                break;
            case R.id.tv_switch:
                // 切换到该网络
                switchToNetwork();
                networkActionDialog.dismiss();
                break;
            case R.id.tv_delete:
                showDeleteDialog();
                networkActionDialog.dismiss();
                break;
        }
    }

    /**
     * switch to network
     */
    private void switchToNetwork() {
        MeshNetwork meshNetwork = adapter.get(selectedPosition);
        int userId = TelinkMeshApplication.getInstance().getUser().getId();
        SharedPreferenceHelper.setNetworkId(getActivity(), userId, meshNetwork.getId());
        gotoMainPage();
    }


    private void gotoNetworkShare() {
        MeshNetwork meshNetwork = adapter.get(selectedPosition);
        startActivity(
                new Intent(getActivity(), ShareExportActivity.class)
                        .putExtra("networkId", meshNetwork.getId())
                        .putExtra("networkName", meshNetwork.getName())
        );
    }

    private void gotoApplicants() {
        MeshNetwork meshNetwork = adapter.get(selectedPosition);
        startActivity(
                new Intent(getActivity(), ShareApplicantsActivity.class)
                        .putExtra("networkId", meshNetwork.getId())
                        .putExtra("networkName", meshNetwork.getName())
        );
    }

    private void initBottomDialog() {
        networkActionDialog = new BottomSheetDialog(getActivity(), R.style.BottomSheetDialog);
        networkActionDialog.setCancelable(true);
        networkActionDialog.setContentView(R.layout.dialog_bottom_network_detail);
        tv_dialog_title = networkActionDialog.findViewById(R.id.tv_dialog_title);
        TextView tv_share = networkActionDialog.findViewById(R.id.tv_share);
        TextView tv_applicant = networkActionDialog.findViewById(R.id.tv_applicant);
        TextView tv_switch = networkActionDialog.findViewById(R.id.tv_switch);
        TextView tv_delete = networkActionDialog.findViewById(R.id.tv_delete);
        networkActionDialog.findViewById(R.id.iv_close).setOnClickListener(this);
        tv_share.setOnClickListener(this);
        tv_switch.setOnClickListener(this);
        tv_delete.setOnClickListener(this);
        tv_applicant.setOnClickListener(this);
    }

    private void showBottomDialog(int position) {
        this.selectedPosition = position;
        String title = "select action for : " + adapter.get(position).getName();
        tv_dialog_title.setText(title);
        networkActionDialog.show();
    }

    /********************************************************************************************************
     * create network - start
     ********************************************************************************************************/
    private void showCreateNetworkDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setTitle("Create New Mesh Network")
                .setView(R.layout.dialog_create_mesh_network)
//                .setMessage("Input Network Name")
                .setPositiveButton("Confirm", (dialog, which) -> createNetwork(et_network_name.getText().toString()))
                .setNegativeButton("Cancel", (dialog, which) -> dialog.cancel());
        AlertDialog dialog = builder.show();
        et_network_name = dialog.findViewById(R.id.et_network_name);
    }

    private void showDeleteDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setTitle("Warning")
                .setMessage("Delete Mesh Network?")
                .setPositiveButton("Confirm", (dialog, which) -> deleteNetwork())
                .setNegativeButton("Cancel", (dialog, which) -> dialog.cancel());
        builder.show();
    }

    private void createNetwork(String name) {
        if (TextUtils.isEmpty(name)) {
            toastMsg("pls input network name");
            return;
        }
        FormBody formBody = new FormBody.Builder()
                .add("name", name)
                .add("clientId", SharedPreferenceHelper.getClientId(getActivity()))
                .build();
        TelinkHttpClient.getInstance().post(URL_CREATE_NETWORK, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.d("create network call onFailure");
                dismissIndeterminateLoading();
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, (BaseActivity) getActivity());
                if (responseInfo == null) {
                    return;
                }
                MeshLogger.d("create network call response");
                try {

                    MeshNetwork network = JSON.parseObject(responseInfo.data, MeshNetwork.class);
                    if (network == null) {
                        showError("network null in response");
                        return;
                    }
                    getActivity().runOnUiThread(() -> {
                        adapter.add(network);
                        updateEmptyVisibility(false);
                    });
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    private void deleteNetwork() {
        MeshNetwork network = adapter.get(selectedPosition);
        FormBody formBody = new FormBody.Builder()
                .add("networkId", network.getId() + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_DELETE_NETWORK, formBody, new Callback() {
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

    /********************************************************************************************************
     * create network - end
     ********************************************************************************************************/


    public void getNetworkList() {
        showIndeterminateLoading();
        TelinkHttpClient.getInstance().get(URL_GET_NETWORK_LIST, null, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                dismissIndeterminateLoading();
                MeshLogger.e("call fail");
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, (BaseActivity) getActivity());
                if (responseInfo == null) {
                    return;
                }
                try {
                    List<MeshNetwork> list = JSON.parseArray(responseInfo.data, MeshNetwork.class);
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
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
        startActivity(intent);
        getActivity().finish();
    }

}
