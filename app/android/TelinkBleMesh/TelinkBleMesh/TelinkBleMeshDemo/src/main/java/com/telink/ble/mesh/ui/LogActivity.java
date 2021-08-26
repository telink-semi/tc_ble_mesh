/********************************************************************************************************
 * @file LogActivity.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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
package com.telink.ble.mesh.ui;

import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.text.TextUtils;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.adapter.LogInfoAdapter;
import com.telink.ble.mesh.util.FileSystem;
import com.telink.ble.mesh.util.LogInfo;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Locale;

import androidx.appcompat.app.AlertDialog;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * Created by kee on 2017/9/11.
 */

public class LogActivity extends BaseActivity {
    private AlertDialog dialog;
    private LogInfoAdapter adapter;
    private Handler mHandler = new Handler();
    private static final String LOG_FILE_PATH = "TelinkBleMesh";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_log_info);
        RecyclerView recyclerView = findViewById(R.id.rv_log);
        adapter = new LogInfoAdapter(this, MeshLogger.logInfoList);
        enableBackNav(true);
        setTitle("Log");
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        recyclerView.setAdapter(adapter);
    }

    public void clear(View view) {
        MeshLogger.logInfoList.clear();
        adapter.notifyDataSetChanged();
    }

    public void refresh(View view) {
        adapter.notifyDataSetChanged();
    }


    public void save(View view) {
        if (dialog == null) {
            AlertDialog.Builder dialogBuilder = new AlertDialog.Builder(this);
            final EditText editText = new EditText(this);
            dialogBuilder.setTitle("Pls input filename(sdcard/" + LOG_FILE_PATH + "/[filename].text)");
            dialogBuilder.setView(editText);
            dialogBuilder.setNegativeButton("cancel", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                }
            }).setPositiveButton("confirm", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    if (TextUtils.isEmpty(editText.getText().toString())) {
                        Toast.makeText(LogActivity.this, "fileName cannot be null", Toast.LENGTH_SHORT).show();
                    } else {
                        showWaitingDialog("saving......");
                        saveLog(editText.getText().toString().trim());
                    }
                }
            });
            dialog = dialogBuilder.create();
        }
        dialog.show();
    }


    private void saveLog(final String fileName) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                SimpleDateFormat mDateFormat = new SimpleDateFormat("MM-dd HH:mm:ss.SSS", Locale.getDefault());
                final StringBuilder sb = new StringBuilder("TelinkLog\n");
                final LogInfo[] logs = MeshLogger.logInfoList.toArray(new LogInfo[]{});
                System.out.println("logs: " + logs.length);
                for (LogInfo logInfo : logs) {
                    if (logInfo != null) {
                        sb.append(mDateFormat.format(logInfo.millis)).append("/").append(logInfo.tag).append(":")
                                .append(logInfo.logMessage).append("\n");
                    }
                }
                saveLogInFile(fileName, sb.toString());
                mHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        dismissWaitingDialog();
                        Toast.makeText(LogActivity.this, fileName + " saved", Toast.LENGTH_SHORT).show();
                    }
                });
            }
        }).start();
    }

    /*public void checkConnectDevices(View view) {
        BluetoothManager manager = (BluetoothManager) this.getSystemService(Context.BLUETOOTH_SERVICE);
        List<BluetoothDevice> devices = manager.getConnectedDevices(BluetoothProfile.GATT);
//        Toast.makeText(this, "current connect" + devices.size(), Toast.LENGTH_SHORT).showToast();
        TelinkMeshApplication.getInstance().saveLog("The connected device count: " + devices.size());
        for (BluetoothDevice device : devices) {
            TelinkMeshApplication.getInstance().saveLog("\tThe connected device: " + device.getName() + "--" + device.getAddress());
        }

//        String info = TelinkMeshApplication.getInstance().getLogInfo();
//        tv_info.setText(info);
    }*/

    public void saveLogInFile(String fileName, String logInfo) {
        File root = Environment.getExternalStorageDirectory();
        File dir = new File(root.getAbsolutePath() + File.separator + LOG_FILE_PATH);
        if (FileSystem.writeString(dir, fileName + ".txt", logInfo) != null) {
            MeshLogger.d("logMessage saved in: " + fileName);
        }
    }

}
