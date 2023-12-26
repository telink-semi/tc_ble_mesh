/********************************************************************************************************
 * @file QRCodeShareActivity.java
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
package com.telink.ble.mesh.ui.qrcode;

import android.annotation.SuppressLint;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.WindowMetrics;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.appcompat.app.AlertDialog;

import com.google.gson.Gson;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.MeshNetKey;
import com.telink.ble.mesh.model.db.MeshInfoService;
import com.telink.ble.mesh.model.json.MeshStorageService;
import com.telink.ble.mesh.ui.BaseActivity;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Response;

/**
 * share by QRCode
 */
public class QRCodeShareActivity extends BaseActivity {

    private ImageView iv_qr;
    private TextView tv_info;

    // time unit : second
    private static final int QRCODE_TIMEOUT = 5 * 60;
    private int countIndex;
    private QRCodeGenerator mQrCodeGenerator;
    private Handler countDownHandler = new Handler();
    List<MeshNetKey> meshNetKeyList;
    private MeshInfo meshInfo;

    @SuppressLint("HandlerLeak")
    private Handler mGeneratorHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == QRCodeGenerator.QRCode_Generator_Success) {
                if (mQrCodeGenerator.getResult() != null) {
                    iv_qr.setImageBitmap(mQrCodeGenerator.getResult());
                    countIndex = QRCODE_TIMEOUT;
                    countDownHandler.post(countDownTask);
                }
            } else {
                toastMsg("qr code data error!");
            }
        }
    };
    private Runnable countDownTask = new Runnable() {
        @Override
        public void run() {
            tv_info.setText("QR-Code available in " + countIndex + " seconds");
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
        setTitle("Share-QRCode");
        enableBackNav(true);
        long meshId = getIntent().getLongExtra("MeshInfoId", 0);
        meshInfo = MeshInfoService.getInstance().getById(meshId);

        /*Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.share_scan);
        toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                startActivity(new Intent(QRCodeShareActivity.this, QRCodeScanActivity.class));
                finish();
                return false;
            }
        });*/
        tv_info = findViewById(R.id.tv_info);
        iv_qr = findViewById(R.id.iv_qr);

//        onUploadSuccess("12:34:56:78:AB:CD"); // for test
        getNetKeyList();
        upload(meshNetKeyList);
    }

    private void getNetKeyList() {
        int[] selectedIndexes = getIntent().getIntArrayExtra("selectedIndexes");
        if (selectedIndexes == null) return;
        meshNetKeyList = new ArrayList<>();
        outer:
        for (MeshNetKey netKey : meshInfo.meshNetKeyList) {
            for (int idx : selectedIndexes) {
                if (idx == netKey.index) {
                    meshNetKeyList.add(netKey);
                    continue outer;
                }
            }
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        countDownHandler.removeCallbacksAndMessages(null);
        mGeneratorHandler.removeCallbacksAndMessages(null);
    }

    private void onCountDownComplete() {
        countDownHandler.removeCallbacks(countDownTask);
//        iv_qr.setImageBitmap(null);
        AlertDialog.Builder builder = new AlertDialog.Builder(QRCodeShareActivity.this);
        builder.setTitle("Warning")
                .setMessage("QRCode timeout")
                .setCancelable(false)
                .setPositiveButton("Regenerate", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        upload(meshNetKeyList);
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        finish();
                    }
                });
        builder.show();
    }

    private void upload(List<MeshNetKey> meshNetKeyList) {
        showWaitingDialog("uploading...");
        String jsonStr = MeshStorageService.getInstance().meshToJsonString(meshInfo, meshNetKeyList);
        MeshLogger.d("upload json string: " + jsonStr);
        TelinkHttpClient.getInstance().upload(jsonStr, QRCODE_TIMEOUT, uploadCallback);
    }


    private void onUploadSuccess(String uuid) {
        runOnUiThread(this::dismissWaitingDialog);
//        int size = iv_qr.getMeasuredWidth();
        int size = getWindowManager().getDefaultDisplay().getWidth();
        MeshLogger.d("qrcode size - " + size);
//        int size = 64; // for test
        mQrCodeGenerator = new QRCodeGenerator(mGeneratorHandler, size, uuid);
        mQrCodeGenerator.execute();
    }

    private void onUploadFail(final String desc) {
        MeshLogger.w(desc);
        runOnUiThread(() -> {
            dismissWaitingDialog();
            AlertDialog.Builder builder = new AlertDialog.Builder(QRCodeShareActivity.this);
            builder.setTitle("Warning")
                    .setMessage(desc)
                    .setCancelable(false)
                    .setPositiveButton("Retry", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            upload(meshNetKeyList);
                        }
                    })
                    .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            finish();
                        }
                    });
            builder.show();
        });

    }


    private Callback uploadCallback = new Callback() {
        @Override
        public void onFailure(Call call, IOException e) {
            MeshLogger.d("upload fail: " + e.toString());
            onUploadFail("request fail, pls check network");
        }

        @Override
        public void onResponse(Call call, Response response) throws IOException {
            String result = response.body().string();
            if (result == null) {
                onUploadFail("request fail: server status error");
                return;
            }
            Gson gson = new Gson();
            MeshLogger.d("result: " + result);
            HttpResponse httpResponse = gson.fromJson(result, HttpResponse.class);
            if (httpResponse == null) {
                onUploadFail("request fail: response invalid");
                return;
            }

            if (httpResponse.isSuccess) {
                String uuid = (String) httpResponse.data;
                onUploadSuccess(uuid);
            } else {
                onUploadFail("upload fail: " + httpResponse.msg);
            }

        }
    };

}
