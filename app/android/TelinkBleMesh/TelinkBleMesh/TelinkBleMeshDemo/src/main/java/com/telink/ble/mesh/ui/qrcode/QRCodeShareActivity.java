/********************************************************************************************************
 * @file     QRCodeShareActivity.java 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
package com.telink.ble.mesh.ui.qrcode;

import android.annotation.SuppressLint;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.MenuItem;
import android.widget.ImageView;
import android.widget.TextView;

import com.google.gson.Gson;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.json.MeshStorageService;
import com.telink.ble.mesh.ui.BaseActivity;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.IOException;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;
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
        upload();
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
                        upload();
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

    private void upload() {
        showWaitingDialog("uploading...");
        MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        String jsonStr = MeshStorageService.getInstance().meshToJsonString(meshInfo);
        MeshLogger.d("upload json string: " + jsonStr);
        TelinkHttpClient.getInstance().upload(jsonStr, QRCODE_TIMEOUT, uploadCallback);
    }


    private void onUploadSuccess(String uuid) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                dismissWaitingDialog();
            }
        });
        int size = iv_qr.getMeasuredWidth();
        mQrCodeGenerator = new QRCodeGenerator(mGeneratorHandler, size, uuid);
        mQrCodeGenerator.execute();
    }

    private void onUploadFail(final String desc) {
        MeshLogger.w(desc);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                dismissWaitingDialog();
                AlertDialog.Builder builder = new AlertDialog.Builder(QRCodeShareActivity.this);
                builder.setTitle("Warning")
                        .setMessage(desc)
                        .setCancelable(false)
                        .setPositiveButton("Retry", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                upload();
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
