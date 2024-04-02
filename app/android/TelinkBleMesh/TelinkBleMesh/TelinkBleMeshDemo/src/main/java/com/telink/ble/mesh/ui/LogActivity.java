/********************************************************************************************************
 * @file LogActivity.java
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

import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.text.TextUtils;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.core.content.FileProvider;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.adapter.LogInfoAdapter;
import com.telink.ble.mesh.util.FileSystem;
import com.telink.ble.mesh.util.LogInfo;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

/**
 * Created by kee on 2017/9/11.
 */

public class LogActivity extends BaseActivity {
    private AlertDialog dialog;
    private LogInfoAdapter adapter;
    private Handler mHandler = new Handler();
    private static final String LOG_FILE_PATH = "TelinkBleMesh";
    private SimpleDateFormat dateFormat = new SimpleDateFormat("YYYY_MMdd_HHmmss", Locale.CHINA);

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
            dialogBuilder.setTitle("Pls input filename(sdcard/" + LOG_FILE_PATH + "/[filename].txt)");
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
                        saveLog(editText.getText().toString().trim(), false);
                    }
                }
            });
            dialog = dialogBuilder.create();
        }
        dialog.show();
    }

    public void share(View view) {
        String fileName = "telink_sig_mesh_log_" + dateFormat.format(new Date());
        saveLog(fileName, true);
    }


    private void saveLog(final String fileName, boolean share) {
        showWaitingDialog("log saving...");
        new Thread(() -> {
            SimpleDateFormat mDateFormat = new SimpleDateFormat("MM-dd HH:mm:ss.SSS", Locale.getDefault());

            final LogInfo[] logs = MeshLogger.logInfoList.toArray(new LogInfo[]{});
            File root = Environment.getExternalStorageDirectory();
            File dir = new File(root.getAbsolutePath() + File.separator + LOG_FILE_PATH);

            if (!dir.exists()) {
                dir.mkdirs();
            }
            boolean suc;
            File file = new File(dir, fileName + ".txt");
            try {
                if (!file.exists())
                    file.createNewFile();
                FileWriter writer = new FileWriter(file, true);
                writer.append("TelinkLog\n");
                for (LogInfo logInfo : logs) {
                    if (logInfo != null) {
                        writer.append(mDateFormat.format(logInfo.millis)).append("/").append(logInfo.tag).append(":")
                                .append(logInfo.logMessage).append("\n");
                    }
                }
                writer.flush();
                writer.close();
                suc = true;
            } catch (IOException e) {
                e.printStackTrace();
                suc = false;
            }
            MeshLogger.d("save log complete - " + suc + " " + fileName);
            boolean finalSuc = suc;
            mHandler.post(() -> {
                dismissWaitingDialog();
                if (finalSuc) {
                    if (share) {
                        shareFile(file);
                    } else {
                        Toast.makeText(LogActivity.this, fileName + " saved", Toast.LENGTH_SHORT).show();
                    }
                } else {
                    Toast.makeText(LogActivity.this, "save file error", Toast.LENGTH_SHORT).show();
                }

            });
        }).start();
    }

    public void shareFile(File file) {
        Intent intent = new Intent(Intent.ACTION_SEND);
        intent.setType("*/*");

        Uri uri;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            uri = FileProvider.getUriForFile(this, getPackageName() + ".fileprovider", file);
            intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        } else {
            uri = Uri.fromFile(file);
        }
        intent.putExtra(Intent.EXTRA_STREAM, uri);
        intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        startActivity(Intent.createChooser(intent, "share log file"));
    }

    public void saveLogInFile(String fileName, String logInfo) {
        File root = Environment.getExternalStorageDirectory();
        File dir = new File(root.getAbsolutePath() + File.separator + LOG_FILE_PATH);
        if (FileSystem.writeString(dir, fileName + ".txt", logInfo) != null) {
            MeshLogger.d("logMessage saved in: " + fileName);
        }
    }
}
