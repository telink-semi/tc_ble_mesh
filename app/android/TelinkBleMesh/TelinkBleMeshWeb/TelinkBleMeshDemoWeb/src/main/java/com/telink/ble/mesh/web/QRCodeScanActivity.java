/********************************************************************************************************
 * @file QRCodeScanActivity.java
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

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.core.app.ActivityCompat;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONException;
import com.google.zxing.Result;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.activity.BaseActivity;
import com.telink.ble.mesh.ui.qrcode.NetworkQrCodeInfo;
import com.telink.ble.mesh.util.MeshLogger;

import me.dm7.barcodescanner.zxing.ZXingScannerView;

/**
 * QRCode scanning
 */
public class QRCodeScanActivity extends BaseActivity implements ZXingScannerView.ResultHandler {

    public static final String QR_SCAN_RESULT = "com.telink.ble.mesh.qr_result";

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
        validateScanResult(rawResult.getText());
    }


    private void validateScanResult(String scanText) {
        try {
            NetworkQrCodeInfo re = JSON.parseObject(scanText, NetworkQrCodeInfo.class);
            if (re == null || re.shareLinkId == null) {
                showErrorDialog("Content unrecognized");
                return;
            }
            setResult(RESULT_OK, new Intent().putExtra(QR_SCAN_RESULT, re));
            finish();
        } catch (
                JSONException e) {
            e.getStackTrace();
        }
    }


    private void showErrorDialog(String message) {
        MeshLogger.w("error dialog : " + message);
        if (errorDialogBuilder == null) {
            errorDialogBuilder = new AlertDialog.Builder(this);
            errorDialogBuilder.setTitle("Warning")
                    .setMessage(message)
                    .setCancelable(false)
                    .setPositiveButton("Rescan", (dialog, which) -> {
                        handling = false;
                        restartCamera();
//                            mScannerView.resumeCameraPreview(QRCodeScanActivity.this);
                    })
                    .setNegativeButton("Cancel", (dialog, which) -> finish());
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
}
