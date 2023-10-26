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
package com.telink.ble.mesh.ui.qrcode;

import android.Manifest;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.core.app.ActivityCompat;

import com.google.gson.Gson;
import com.google.zxing.Result;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.json.MeshStorageService;
import com.telink.ble.mesh.ui.BaseActivity;
import com.telink.ble.mesh.ui.ShareImportActivity;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.IOException;
import java.util.UUID;

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
    }

    @Override
    protected void onStart() {
        super.onStart();
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
                onPermissionDenied();
//                finish();
            }
        }
    }


    /**
     * show tip dialog when camera permission denied
     */
    AlertDialog permissionDialog;

    private void onPermissionDenied() {
        if (permissionDialog == null) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setCancelable(false);
            builder.setTitle("Warn");
            builder.setMessage("camera permission denied, click [Go Settings] to set permission");
            builder.setPositiveButton("Go Settings", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    Intent intent = new Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS, Uri.parse("package:" + getPackageName()));
                    startActivity(intent);
                }
            });

            builder.setNegativeButton("Cancel", (dialog, which) -> {
                dialog.dismiss();
                finish();
            });
            permissionDialog = builder.create();
        }
        if (permissionDialog.isShowing()) {
            return;
        }
        permissionDialog.show();
    }

    private void onDownloadSuccess(final String meshJson) {
        MeshLogger.d("device import json string: " + meshJson);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setCancelable(false);
        builder.setTitle("Warning");
        builder.setMessage("Mesh JSON receive complete, import data?");
        builder.setPositiveButton("Confirm", (dialog, which) -> {
            MeshInfo meshInfo = MeshStorageService.getInstance().importExternal(meshJson, this);
            if (meshInfo != null) {
                dialog.dismiss();
                Intent intent = new Intent();
                intent.putExtra(ShareImportActivity.EXTRA_NETWORK_ID, meshInfo.id);
                setResult(RESULT_OK, intent);
                finish();
            }
        });
        builder.setNegativeButton("Cancel", (dialog, which) -> {
            dialog.dismiss();
//            finish();
            handling = false;
            restartCamera();
        });
        builder.show();
        /* MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        if (MeshStorageService.getInstance().importExternal(meshJson, this)) {
            MeshLogger.d("Mesh storage import success");
        } else {
            MeshLogger.d("Mesh storage import fail");
        }*/
    }

    private void syncData(MeshInfo newMesh) {
        MeshLogger.d("sync mesh : " + newMesh.toString());
        newMesh.saveOrUpdate();
        MeshService.getInstance().idle(true);
        TelinkMeshApplication.getInstance().setupMesh(newMesh);
        Toast.makeText(this, "import success", Toast.LENGTH_SHORT).show();
        finish();
    }


    private void onDownloadFail(final String desc) {
        runOnUiThread(() -> {
            dismissWaitingDialog();
            MeshLogger.w(desc);
            showErrorDialog(desc);
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
                runOnUiThread(() -> onDownloadSuccess(meshJson));
            } else {
                onDownloadFail("fail: " + httpResponse.msg);
            }
        }
    };

}
