/********************************************************************************************************
 * @file ShareLinkListFragment.java
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
package com.telink.ble.mesh.ui.fragment;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.AppCompatSpinner;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONException;
import com.google.android.material.bottomsheet.BottomSheetDialog;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.web.MeshNetworkShareLink;
import com.telink.ble.mesh.ui.activity.BaseActivity;
import com.telink.ble.mesh.ui.adapter.ShareLinkAdapter;
import com.telink.ble.mesh.ui.qrcode.NetworkQrCodeInfo;
import com.telink.ble.mesh.ui.qrcode.QRCodeGenerator;
import com.telink.ble.mesh.util.ContextUtil;
import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.ResponseInfo;
import com.telink.ble.mesh.web.TelinkHttpClient;
import com.telink.ble.mesh.web.WebUtils;

import java.io.IOException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.Response;

/**
 * 用户创建的分享链接列表
 */

public class ShareLinkListFragment extends BaseFragment implements View.OnClickListener {

    public final static String URL_GET_LINK_LIST = TelinkHttpClient.URL_BASE + "mesh-network/getShareLinkList";

    public final static String URL_CREATE_LINK = TelinkHttpClient.URL_BASE + "mesh-network/createShareLink";

    public final static String URL_DELETE_LINK = TelinkHttpClient.URL_BASE + "mesh-network/deleteShareLink";
    private int meshNetworkId;

    private String meshNetworkName;

    private ShareLinkAdapter adapter;
    private List<MeshNetworkShareLink> linkList;

    private BottomSheetDialog linkActionDialog;

    private ImageView iv_qrcode;
    // open / close
    private TextView tv_switch;
    private TextView tv_delete;

    private QRCodeGenerator mQrCodeGenerator;
    private int selectedPosition = -1;

    /**
     * dialog inputs
     */
    private EditText et_count = null, et_period = null;
    private AppCompatSpinner sp_unit;

    @SuppressLint("HandlerLeak")
    private Handler mGeneratorHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == QRCodeGenerator.QRCode_Generator_Success) {
                if (mQrCodeGenerator.getResult() != null) {
                    iv_qrcode.setImageBitmap(mQrCodeGenerator.getResult());
                }
            } else {
                toastMsg("qr code data error!");
            }
        }
    };

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_share_link, null);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        mGeneratorHandler.removeCallbacksAndMessages(null);
    }

    @Override
    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        adapter = new ShareLinkAdapter(getActivity(), linkList, this::showLinkDetail);
        RecyclerView rv_share_link = view.findViewById(R.id.rv_share_link);
        rv_share_link.setLayoutManager(new LinearLayoutManager(getActivity()));
        rv_share_link.setAdapter(adapter);

        view.findViewById(R.id.fab_link).setOnClickListener(this);
        initBottomDialog();
        getLinkList();
    }

    // user create new link
    public void showCreateLinkDialog() {

        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setTitle("Create New Share Link")
                .setView(R.layout.dialog_create_share_link)
                .setPositiveButton("Confirm", (dialog, which) -> {
                    validateDialogInput();
                })
                .setNegativeButton("Cancel", (dialog, which) -> dialog.cancel());
        AlertDialog dialog = builder.create();
        dialog.show();
        et_count = dialog.findViewById(R.id.et_link_count);
        et_period = dialog.findViewById(R.id.et_link_period);
        sp_unit = dialog.findViewById(R.id.sp_unit);
    }

    // check create share link dialog
    private void validateDialogInput() {
        if (et_count == null || et_period == null) {
            MeshLogger.d("input view null");
            return;
        }
        String countInput = et_count.getText().toString().trim();
        if (TextUtils.isEmpty(countInput)) {
            toastMsg("please input count");
            return;
        }
        String periodInput = et_period.getText().toString().trim();
        if (TextUtils.isEmpty(periodInput)) {
            toastMsg("please input period");
            return;
        }
        int multi = 1;
        int selectedSp = sp_unit.getSelectedItemPosition();
        // 0 for minute
        if (selectedSp == 1) {
            multi = 60;
            // hour
        } else if (selectedSp == 2) {
            // day
            multi = 60 * 24;
        }
        int period = Integer.parseInt(periodInput);
        if (period <= 0) {
            toastMsg("period input error");
            return;
        }
        period *= multi;
        createShareLink(countInput, period);
    }

    public void setMeshNetworkInfo(int meshNetworkId, String networkName) {
        this.meshNetworkId = meshNetworkId;
        this.meshNetworkName = networkName;
    }

    private void showLinkDetail(int position) {

        if (selectedPosition != position) {
            selectedPosition = position;
            iv_qrcode.setImageBitmap(null);
            int size = ContextUtil.dpToPx(getActivity(), 200);
            MeshLogger.d("size - " + size);
            NetworkQrCodeInfo info = new NetworkQrCodeInfo();
            info.shareLinkId = linkList.get(position).getId();
            info.userName = TelinkMeshApplication.getInstance().getUser().getName();
            info.networkName = meshNetworkName;
            mQrCodeGenerator = new QRCodeGenerator(mGeneratorHandler, size, JSON.toJSONString(info));
            mQrCodeGenerator.setCodeColor(getResources().getColor(R.color.colorPrimary));
            mQrCodeGenerator.execute();
        }
        linkActionDialog.show();
    }


    private void initBottomDialog() {
        linkActionDialog = new BottomSheetDialog(getActivity(), R.style.BottomSheetDialog);
        linkActionDialog.setCancelable(true);
        linkActionDialog.setContentView(R.layout.dialog_bottom_link_action);
        linkActionDialog.findViewById(R.id.iv_close).setOnClickListener(this);
        iv_qrcode = linkActionDialog.findViewById(R.id.iv_qrcode);
//        tv_switch = linkActionDialog.findViewById(R.id.tv_switch);
        tv_delete = linkActionDialog.findViewById(R.id.tv_delete);
        tv_delete.setOnClickListener(this);
//        tv_switch.setOnClickListener(this);
    }

    @Override
    public void onResume() {
        super.onResume();

    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.iv_close:
                linkActionDialog.dismiss();
                break;
            case R.id.tv_switch:
                // 切换 开启/关闭
                break;

            case R.id.tv_delete:
                showDeleteDialog();
                linkActionDialog.dismiss();
                break;

            case R.id.fab_link:
                showCreateLinkDialog();
                break;
        }
    }

    private void showDeleteDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setTitle("Warning")
                .setMessage("Delete Share Link?")
                .setPositiveButton("Confirm", (dialog, which) -> deleteShareLink(selectedPosition))
                .setNegativeButton("Cancel", (dialog, which) -> dialog.cancel());
        builder.show();
    }


    public void getLinkList() {
        showIndeterminateLoading();
        boolean expired = false;
        Map<String, String> params = new HashMap<>();
        params.put("networkId", meshNetworkId + "");
        params.put("expired", expired + "");
        TelinkHttpClient.getInstance().get(URL_GET_LINK_LIST, params, new Callback() {
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
                    MeshLogger.e("getLinkList error: server response error");
                    return;
                }
                try {
                    List<MeshNetworkShareLink> list = JSON.parseArray(responseInfo.data, MeshNetworkShareLink.class);
                    if (list == null) {
                        showError("list null in response");
                        return;
                    }
                    linkList = list;
                    Objects.requireNonNull(getActivity()).runOnUiThread(() -> adapter.setShareLinks(list));

                } catch (JSONException e) {
                    e.getStackTrace();
                }
            }
        });
    }

    /**
     * 创建分享链接
     */
    private void createShareLink(String count, int minutes) {
        FormBody formBody = new FormBody.Builder()
                .add("networkId", meshNetworkId + "")
                .add("maxCount", count)
                .add("validPeriod", minutes + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_CREATE_LINK, formBody, new Callback() {
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
                try {
                    MeshNetworkShareLink link = JSON.parseObject(responseInfo.data, MeshNetworkShareLink.class);
                    if (link == null) {
                        showError("share link null in response");
                        return;
                    }
                    Objects.requireNonNull(getActivity()).runOnUiThread(() -> adapter.addShareLink(link));

                } catch (JSONException e) {
                    e.getStackTrace();
                }
            }
        });
    }


    /**
     * 创建分享链接
     */
    private void deleteShareLink(int position) {
        int shareLinkId = linkList.get(position).getId();
        FormBody formBody = new FormBody.Builder()
                .add("shareLinkId", shareLinkId + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_DELETE_LINK, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.d("deleteShareLink call onFailure");
                dismissIndeterminateLoading();
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, (BaseActivity) getActivity());
                if (responseInfo == null) {
                    return;
                }
                MeshLogger.d("delete share link call response");
                Objects.requireNonNull(getActivity()).runOnUiThread(() -> adapter.removeShareLink(position));
            }
        });
    }
}
