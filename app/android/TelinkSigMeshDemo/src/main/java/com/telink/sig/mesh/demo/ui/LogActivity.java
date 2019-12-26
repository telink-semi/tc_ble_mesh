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
package com.telink.sig.mesh.demo.ui;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AlertDialog;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.TextUtils;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.telink.sig.mesh.TelinkApplication;
import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.model.LogInfo;
import com.telink.sig.mesh.demo.ui.adapter.BaseRecyclerViewAdapter;
import com.telink.sig.mesh.demo.ui.adapter.LogInfoAdapter;

import java.text.SimpleDateFormat;
import java.util.List;
import java.util.Locale;

/**
 * Created by kee on 2017/9/11.
 */

public class LogActivity extends BaseActivity {
    private AlertDialog dialog;
    private List<LogInfo> logs;
    private LogInfoAdapter adapter;
    private Handler mHandler = new Handler();


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_log_info);
        RecyclerView recyclerView = findViewById(R.id.rv_log);
        logs = TelinkMeshApplication.getInstance().getLogInfo();
        adapter = new LogInfoAdapter(this, logs);
        enableBackNav(true);
        setTitle("Log");
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        recyclerView.setAdapter(adapter);
    }

    public void clear(View view) {
        TelinkMeshApplication.getInstance().clearLogInfo();
        logs = TelinkMeshApplication.getInstance().getLogInfo();
        adapter.refreshLogs(logs);
    }

    public void refresh(View view) {
        logs = TelinkMeshApplication.getInstance().getLogInfo();
        adapter.refreshLogs(logs);
    }


    public void save(View view) {
        if (dialog == null) {
            AlertDialog.Builder dialogBuilder = new AlertDialog.Builder(this);
            final EditText editText = new EditText(this);
            dialogBuilder.setTitle("Pls input filename(sdcard/TelinkSigMeshSetting/[filename].text)");
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
                for (LogInfo logInfo :
                        logs) {
                    sb.append(mDateFormat.format(logInfo.datetime)).append("/").append(logInfo.tag).append(":")
                            .append(logInfo.log).append("\n");
                }
                TelinkMeshApplication.getInstance().saveLogInFile(fileName, sb.toString());
                mHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        dismissWaitingDialog();
                        Toast.makeText(LogActivity.this, "fileName saved", Toast.LENGTH_SHORT).show();
                    }
                });
            }
        }).start();
    }

    public void checkConnectDevices(View view) {
        BluetoothManager manager = (BluetoothManager) this.getSystemService(Context.BLUETOOTH_SERVICE);
        List<BluetoothDevice> devices = manager.getConnectedDevices(BluetoothProfile.GATT);
//        Toast.makeText(this, "当前连接设备个数" + devices.size(), Toast.LENGTH_SHORT).showToast();
        TelinkMeshApplication.getInstance().saveLog("The connected device count: " + devices.size());
        for (BluetoothDevice device : devices) {
            TelinkMeshApplication.getInstance().saveLog("\tThe connected device: " + device.getName() + "--" + device.getAddress());
        }

//        String info = TelinkMeshApplication.getInstance().getLogInfo();
//        tv_info.setText(info);
    }


}
