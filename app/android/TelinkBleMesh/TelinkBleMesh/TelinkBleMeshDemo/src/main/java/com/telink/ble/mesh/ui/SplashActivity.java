/********************************************************************************************************
 * @file SplashActivity.java
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
package com.telink.ble.mesh.ui;

import android.Manifest;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.provider.Settings;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.db.MeshInfoService;
import com.telink.ble.mesh.model.db.ObjectBox;
import com.telink.ble.mesh.model.json.MeshStorageService;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;


/**
 * splash page
 * Created by kee on 2019/4/8.
 */
public class SplashActivity extends BaseActivity {

    /**
     * permission request code
     */
    private static final int PERMISSIONS_REQUEST_ALL = 0x10;

    private Handler delayHandler = new Handler();

    /**
     * permission tip dialog
     */
    private AlertDialog settingDialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        Intent intent = getIntent();
        if (!this.isTaskRoot()) {
            if (intent != null) {
                String action = intent.getAction();
                if (intent.hasCategory(Intent.CATEGORY_LAUNCHER)
                        && Intent.ACTION_MAIN.equals(action)) {
                    finish();
                    return;
                }
            }
        }
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splash);
    }

    /**
     * check permissions when start
     * for SDK >= 31(Android S),check these permissions
     * {@link Manifest.permission#BLUETOOTH_SCAN},
     * {@link Manifest.permission#BLUETOOTH_CONNECT},
     * {@link Manifest.permission#BLUETOOTH_ADVERTISE},
     * {@link Manifest.permission#READ_EXTERNAL_STORAGE},
     * {@link Manifest.permission#WRITE_EXTERNAL_STORAGE}.
     * <p>
     * <p>
     * for SDK >= 23(Android M),check these permissions
     * {@link Manifest.permission#ACCESS_FINE_LOCATION},
     * {@link Manifest.permission#READ_EXTERNAL_STORAGE},
     * {@link Manifest.permission#WRITE_EXTERNAL_STORAGE}.
     */
    @Override
    protected void onStart() {
        super.onStart();
        if (settingDialog != null) {
            settingDialog.dismiss();
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {

                if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN) == PackageManager.PERMISSION_GRANTED
                        &&
                        ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED
                        &&
                        ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED
                        &&
                        ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_ADVERTISE) == PackageManager.PERMISSION_GRANTED
                        &&
                        ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED
                        &&
                        ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED
                ) {
                    onPermissionChecked();
                } else {
                    ActivityCompat.requestPermissions(this,
                            new String[]{
                                    Manifest.permission.BLUETOOTH_SCAN,
                                    Manifest.permission.BLUETOOTH_CONNECT,
                                    Manifest.permission.BLUETOOTH_ADVERTISE,
                                    Manifest.permission.READ_EXTERNAL_STORAGE,
                                    Manifest.permission.WRITE_EXTERNAL_STORAGE,
                                    Manifest.permission.ACCESS_FINE_LOCATION},
                            PERMISSIONS_REQUEST_ALL);
                }
            } else {

                if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED
                        &&
                        ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED
                        &&
                        ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED) {
                    onPermissionChecked();
                } else {
                    ActivityCompat.requestPermissions(this,
                            new String[]{
                                    Manifest.permission.ACCESS_FINE_LOCATION,
                                    Manifest.permission.READ_EXTERNAL_STORAGE,
                                    Manifest.permission.WRITE_EXTERNAL_STORAGE},
                            PERMISSIONS_REQUEST_ALL);
                }
            }
        } else {
            onPermissionChecked();
        }
    }

    /**
     * invoke when all permissions check complete
     */
    private void onPermissionChecked() {
        MeshLogger.log("permission check pass");
        delayHandler.removeCallbacksAndMessages(null);
        delayHandler.postDelayed(this::checkMeshInfo, 500);
    }

    /**
     * check and load database
     * 1. init local storage service;
     * 2. if exist load the last selected mesh network;
     * 3. if not exist, create a new mesh network
     * 4. setup the mesh network
     */
    private void checkMeshInfo() {
        MeshInfoService.getInstance().init(ObjectBox.get());
        long id = SharedPreferenceHelper.getSelectedMeshId(this);
        if (id != -1) {
            // exists
            MeshInfo meshInfo = MeshInfoService.getInstance().getById(id);
            if (meshInfo != null) {
                goToNext(meshInfo);
                return;
            }
        }
        MeshInfo meshInfo = MeshInfo.createNewMesh(this, "Default Mesh");
//        testAddDevice(meshInfo);
        MeshInfoService.getInstance().addMeshInfo(meshInfo);
        goToNext(meshInfo);
    }

    /**
     * add test data to mesh info,
     * only for test,
     * ignore.
     *
     * @param meshInfo target mesh network
     */
    private void testAddDevice(MeshInfo meshInfo) {
        NodeInfo nodeInfo = new NodeInfo();
        nodeInfo.meshAddress = 0x0002;
        nodeInfo.name = String.format("Provisioner Node: %04X", 0x0002);
        nodeInfo.compositionData = CompositionData.from(MeshStorageService.VC_TOOL_CPS);
        nodeInfo.elementCnt = 1;
        nodeInfo.deviceUUID = Arrays.hexToBytes(NodeInfo.LOCAL_DEVICE_KEY);
        nodeInfo.bound = true;
        nodeInfo.deviceKey = Arrays.hexToBytes(NodeInfo.LOCAL_DEVICE_KEY);
        meshInfo.nodes.add(nodeInfo);
    }

    /**
     * save the selected {@link MeshInfo#id} to SharedPreference
     *
     * @param meshInfo selected mesh network
     */
    private void goToNext(MeshInfo meshInfo) {
        SharedPreferenceHelper.setSelectedMeshId(this, meshInfo.id);
        Intent intent = new Intent(SplashActivity.this, MainActivity.class);
//                intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
        startActivity(intent);
        finish();
    }

    /**
     * show tip dialog when permission denied
     */
    private void onPermissionDenied() {
        if (settingDialog == null) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setCancelable(false);
            builder.setTitle("Warn");
            builder.setMessage("Location permission is necessary when searching bluetooth device on 6.0 or upper device");
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
            settingDialog = builder.create();
        }
        settingDialog.show();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        delayHandler.removeCallbacksAndMessages(null);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == PERMISSIONS_REQUEST_ALL) {
            boolean pass = true;
            for (int result : grantResults) {
                if (result != PackageManager.PERMISSION_GRANTED) {
                    pass = false;
                    break;
                }
            }

            if (pass) {
                onPermissionChecked();
            } else {
                onPermissionDenied();
                toastMsg("Permission Denied");
            }
        }
    }

}
