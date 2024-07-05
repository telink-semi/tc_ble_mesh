/********************************************************************************************************
 * @file ShareExportActivity.java
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

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONException;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.web.MeshNetworkShareLink;
import com.telink.ble.mesh.ui.qrcode.NetworkQrCodeInfo;
import com.telink.ble.mesh.ui.qrcode.QRCodeGenerator;
import com.telink.ble.mesh.util.ContextUtil;
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
 * share by QRCode
 */
public class ShareExportActivity extends BaseActivity {

    public final static String URL_CREATE_LINK = TelinkHttpClient.URL_BASE + "mesh-network/createShareLink";
    private int meshNetworkId;
    private String meshNetworkName;
    private ImageView iv_qr;
    private TextView tv_info;

    // time unit : second
    private static final int QRCODE_TIMEOUT = 30 * 60;
    private long countIndex;
    private QRCodeGenerator mQrCodeGenerator;
    private Handler countDownHandler = new Handler();

    @SuppressLint("HandlerLeak")
    private Handler mGeneratorHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == QRCodeGenerator.QRCode_Generator_Success) {
                if (mQrCodeGenerator.getResult() != null) {
                    iv_qr.setImageBitmap(mQrCodeGenerator.getResult());
                }
            } else {
                toastMsg("qr code data error!");
            }
        }
    };
    private Runnable countDownTask = new Runnable() {
        @Override
        public void run() {
            long minutes = countIndex / 60;
            String desc = "Available in ";
            if (minutes != 0) {
                desc += minutes + "min";
            }
            desc += (countIndex % 60) + "s";
            tv_info.setText(desc);
            if (countIndex <= 0) {
                onCountDownComplete();
            } else {
                countIndex--;
                countDownHandler.postDelayed(this, 1000);
            }
        }
    };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_share_qrcode);
        setTitle("Share");
        enableBackNav(true);

        meshNetworkId = getIntent().getIntExtra("networkId", 0);
        meshNetworkName = getIntent().getStringExtra("networkName");

        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.menu_single);
        toolbar.setOnMenuItemClickListener(item -> {
            createShareLink();
            return false;
        });


        tv_info = findViewById(R.id.tv_info);
        iv_qr = findViewById(R.id.iv_qr);

        createShareLink();
    }


    /**
     * 创建分享链接
     */
    private void createShareLink() {
        showIndeterminateLoading();
        int count = 10;
        int minutes = 30;
        FormBody formBody = new FormBody.Builder()
                .add("networkId", meshNetworkId + "")
                .add("maxCount", count + "")
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
                ResponseInfo responseInfo = WebUtils.parseResponse(response, getBaseActivity());
                if (responseInfo == null) {
                    return;
                }
                try {
                    MeshNetworkShareLink link = JSON.parseObject(responseInfo.data, MeshNetworkShareLink.class);
                    if (link == null) {
                        showError("share link null in response");
                        return;
                    }
                    refreshQrCode(link);
                } catch (
                        JSONException e) {
                    e.printStackTrace();
                    showError("network parse json error");
                }
            }
        });
    }

    private void refreshQrCode(MeshNetworkShareLink link) {
//        countIndex = QRCODE_TIMEOUT;
        countIndex = (link.getEndTime() - System.currentTimeMillis()) / 1000;
        countDownHandler.removeCallbacksAndMessages(null);
        countDownHandler.post(countDownTask);

        NetworkQrCodeInfo info = new NetworkQrCodeInfo();
        info.shareLinkId = link.getId();
        info.userName = TelinkMeshApplication.getInstance().getUser().getName();
        info.networkName = meshNetworkName;
        int size = ContextUtil.dpToPx(this, 250);
        mQrCodeGenerator = new QRCodeGenerator(mGeneratorHandler, size, JSON.toJSONString(info));
        mQrCodeGenerator.setCodeColor(getResources().getColor(R.color.colorPrimary));
        mQrCodeGenerator.execute();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        countDownHandler.removeCallbacksAndMessages(null);
        mGeneratorHandler.removeCallbacksAndMessages(null);
    }

    private void onCountDownComplete() {
        countDownHandler.removeCallbacks(countDownTask);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Warning")
                .setMessage("QRCode timeout")
                .setCancelable(false)
                .setPositiveButton("Regenerate", (dialog, which) -> createShareLink())
                .setNegativeButton("Cancel", (dialog, which) -> finish());
        builder.show();
    }


}
