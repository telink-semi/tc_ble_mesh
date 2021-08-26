/********************************************************************************************************
 * @file     QRCodeScanActivity.java 
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

import android.Manifest;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.widget.Toast;

import com.google.gson.Gson;
import com.google.zxing.Result;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.json.MeshStorageService;
import com.telink.ble.mesh.ui.BaseActivity;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.IOException;
import java.util.UUID;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.core.app.ActivityCompat;
import me.dm7.barcodescanner.zxing.ZXingScannerView;
import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Response;

/**
 * QRCode scanning
 */
public class QRCodeScanActivity extends BaseActivity implements ZXingScannerView.ResultHandler {

    private static final int PERMISSION_REQUEST_CODE_CAMERA = 0x01;
    private ZXingScannerView mScannerView;

    AlertDialog.Builder errorDialogBuilder;

    AlertDialog.Builder syncDialogBuilder;

    private boolean handling = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_qrcode_scan);

        setTitle("QRCodeScan");
        enableBackNav(true);

        mScannerView = findViewById(R.id.scanner_view);
        int color = getResources().getColor(R.color.colorAccent);
        mScannerView.setLaserColor(color);

        int borderColor = getResources().getColor(R.color.colorPrimary);
        mScannerView.setBorderColor(borderColor);
    }


    @Override
    public void onResume() {
        super.onResume();
        checkPermissionAndStart();
    }

    private void checkPermissionAndStart() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) {
            restartCamera();
        } else {
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.CAMERA)
                    == PackageManager.PERMISSION_GRANTED) {
                restartCamera();
            } else {
                ActivityCompat.requestPermissions(this,
                        new String[]{Manifest.permission.CAMERA}, PERMISSION_REQUEST_CODE_CAMERA);
            }
        }
    }


    private void restartCamera() {
        if (!handling) {
            mScannerView.setResultHandler(this);
            mScannerView.startCamera();
        }
    }


    @Override
    public void onPause() {
        super.onPause();
        mScannerView.stopCamera();

    }


    @Override
    public void handleResult(Result rawResult) {
        handling = true;
        MeshLogger.d("qrcode scan: " + rawResult.getText());
        getCloudMeshJson(rawResult.getText());
    }


    private void getCloudMeshJson(String scanText) {
        try {
            UUID uuid = UUID.fromString(scanText);
            TelinkHttpClient.getInstance().download(uuid.toString(), downloadCallback);
        } catch (IllegalArgumentException exception) {
            showErrorDialog("Content unrecognized");
        }
    }


    private void showErrorDialog(String message) {
        MeshLogger.w("error dialog : " + message);
        if (errorDialogBuilder == null) {
            errorDialogBuilder = new AlertDialog.Builder(this);
            errorDialogBuilder.setTitle("Warning")
                    .setMessage(message)
                    .setCancelable(false)
                    .setPositiveButton("Rescan", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            handling = false;
                            restartCamera();
//                            mScannerView.resumeCameraPreview(QRCodeScanActivity.this);
                        }
                    })
                    .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            finish();
                        }
                    });
        }
        errorDialogBuilder.setMessage(message);
        errorDialogBuilder.show();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == PERMISSION_REQUEST_CODE_CAMERA) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                restartCamera();
            } else {
                Toast.makeText(this, "camera permission denied", Toast.LENGTH_SHORT).show();
                finish();
            }
        }
    }

    private void onDownloadSuccess(final String meshJson) {
        MeshLogger.d("device import json string: " + meshJson);
        MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        final MeshInfo result;
        try {
            result = MeshStorageService.getInstance().importExternal(meshJson, meshInfo);
        } catch (Exception e) {
            e.printStackTrace();
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    toastMsg("import failed");
                }
            });
            return;
        }
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                dismissWaitingDialog();
                if (result == null) {
                    showErrorDialog("mesh data error");
                } else {
                    if (syncDialogBuilder == null) {
                        syncDialogBuilder = new AlertDialog.Builder(QRCodeScanActivity.this);
                        syncDialogBuilder.setTitle("Tip").setCancelable(false);
                        syncDialogBuilder.setMessage("Get mesh data success, click CONFIRM to cover local data")
                                .setPositiveButton("confirm", new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        syncData(result);
                                    }
                                })
                                .setNegativeButton("cancel", new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        finish();
                                    }
                                });
                    }
                    syncDialogBuilder.show();
                }
            }
        });

    }

    private void syncData(MeshInfo newMesh) {
        MeshLogger.d("sync mesh : " + newMesh.toString());
        newMesh.saveOrUpdate(this);
        MeshService.getInstance().idle(true);
        TelinkMeshApplication.getInstance().setupMesh(newMesh);
        MeshService.getInstance().setupMeshNetwork(newMesh.convertToConfiguration());
        Toast.makeText(this, "import success", Toast.LENGTH_SHORT).show();
        finish();
    }


    private void onDownloadFail(final String desc) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                dismissWaitingDialog();
                MeshLogger.w(desc);
                showErrorDialog(desc);
            }
        });

    }

    private Callback downloadCallback = new Callback() {
        @Override
        public void onFailure(Call call, IOException e) {
            MeshLogger.d("download fail: " + e.toString());
            onDownloadFail("request fail, pls check network");
        }

        @Override
        public void onResponse(Call call, Response response) throws IOException {
            String result = response.body().string();
            if (result == null) {
                onDownloadFail("request fail: server status error");
                return;
            }
            Gson gson = new Gson();
            MeshLogger.d("result: " + result);
            HttpResponse httpResponse = gson.fromJson(result, HttpResponse.class);
            if (httpResponse == null) {
                onDownloadFail("request fail: response invalid");
                return;
            }

            if (httpResponse.isSuccess) {
                final String meshJson = (String) httpResponse.data;
                onDownloadSuccess(meshJson);

            } else {
                onDownloadFail("fail: " + httpResponse.msg);
            }
        }
    };

}
