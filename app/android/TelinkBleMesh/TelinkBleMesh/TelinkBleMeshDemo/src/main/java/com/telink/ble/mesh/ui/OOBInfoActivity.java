/********************************************************************************************************
 * @file OOBInfoActivity.java
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

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.OobInfo;
import com.telink.ble.mesh.model.db.MeshInfoService;
import com.telink.ble.mesh.ui.adapter.OOBListAdapter;
import com.telink.ble.mesh.ui.file.FileSelectActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * show static OOB list
 */
public class OOBInfoActivity extends BaseActivity {

    private final String[] ACTIONS = new String[]{"Manual Input",
            "Import from file"};

    private static final int MSG_IMPORT_COMPLETE = 10;

    private static final int REQUEST_CODE_SELECT_DATABASE = 1;

    private static final int REQUEST_CODE_ADD_OOB = 2;

    public static final int REQUEST_CODE_EDIT_OOB = 3;


    private OOBListAdapter mAdapter;

    private AlertDialog.Builder actionSelectDialog;

    @SuppressLint("HandlerLeak")
    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            if (msg.what == MSG_IMPORT_COMPLETE) {
                dismissWaitingDialog();
                if (msg.obj != null) {
                    List<OobInfo> oobFromFile = (List<OobInfo>) msg.obj;
                    MeshInfoService.getInstance().addOobInfo(oobFromFile);
                    mAdapter.notifyDataSetChanged();
                    Toast.makeText(OOBInfoActivity.this, "Success : " + oobFromFile.size() + " oob imported", Toast.LENGTH_SHORT).show();
                } else {
                    Toast.makeText(OOBInfoActivity.this, "Import Fail: check the file format", Toast.LENGTH_SHORT).show();
                }

            }


        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_oob_info);
        setTitle("OOB List");
        enableBackNav(true);
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.oob_info);
        toolbar.setOnMenuItemClickListener(item -> {
            if (item.getItemId() == R.id.item_oob_add) {
                showModeSelectDialog();
            } else if (item.getItemId() == R.id.item_oob_clear) {
                showClearDialog();
            }
            return false;
        });

        mAdapter = new OOBListAdapter(this);
        mAdapter.setOnItemLongClickListener(position -> {
            showConfirmDialog("delete", (dialog, which) -> mAdapter.remove(position));
            return false;
        });
        RecyclerView rv_oob = findViewById(R.id.rv_oob);
        rv_oob.setLayoutManager(new LinearLayoutManager(this));
        rv_oob.setAdapter(mAdapter);
    }


    private void showModeSelectDialog() {
        if (actionSelectDialog == null) {
            actionSelectDialog = new AlertDialog.Builder(this);
            actionSelectDialog.setItems(ACTIONS, (dialog, which) -> {
                if (which == 0) {
                    startActivityForResult(new Intent(OOBInfoActivity.this, OOBEditActivity.class), REQUEST_CODE_ADD_OOB);
                } else if (which == 1) {
                    startActivityForResult(new Intent(OOBInfoActivity.this, FileSelectActivity.class)
                                    .putExtra(FileSelectActivity.KEY_SUFFIX, ".txt")
                            , REQUEST_CODE_SELECT_DATABASE);
                }
            });
            actionSelectDialog.setTitle("Select mode");

        }
        actionSelectDialog.show();
    }

    private void showClearDialog() {
        showConfirmDialog("Wipe all oob info? ", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                MeshInfoService.getInstance().clearAllOobInfo();
                toastMsg("Wipe oob info success");
                mAdapter.notifyDataSetChanged();
            }
        });
    }


    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode != Activity.RESULT_OK || data == null)
            return;
        if (requestCode == REQUEST_CODE_SELECT_DATABASE) {
            final String path = data.getStringExtra(FileSelectActivity.KEY_RESULT);
            MeshLogger.log("select: " + path);
            showWaitingDialog("parsing OOB database...");
            new Thread(() -> {
                List<OobInfo> parseResult = parseOOBDatabase(path);
                mHandler.obtainMessage(MSG_IMPORT_COMPLETE, parseResult).sendToTarget();
            }).start();
        } else if (requestCode == REQUEST_CODE_ADD_OOB) {
            mAdapter.resetData();
        } else if (requestCode == REQUEST_CODE_EDIT_OOB) {
            mAdapter.resetData();
        }
    }


    /**
     * parse oob database
     */
    public List<OobInfo> parseOOBDatabase(String filePath) {
        if (filePath == null) return null;
        File file = new File(filePath);
        if (!file.exists())
            return null;
        FileReader fr = null;
        BufferedReader br = null;
        try {
            fr = new FileReader(file);
            br = new BufferedReader(fr);
            String line;
            long curTimestamp = System.currentTimeMillis();
            List<OobInfo> result = null;
            OobInfo oobInfo;
            while ((line = br.readLine()) != null) {
                if (line.length() != 65) {
                    continue;
                }
                String[] rawPair = line.split(" ");
                if (rawPair.length != 2 || rawPair[0].length() != 32 || rawPair[1].length() != 32) {
                    continue;
                }
                byte[] uuid = Arrays.hexToBytes(rawPair[0]);
                byte[] oob = Arrays.hexToBytes(rawPair[1]);

                oobInfo = new OobInfo();
                oobInfo.deviceUUID = uuid;
                oobInfo.oob = oob;
                oobInfo.timestamp = curTimestamp;
                oobInfo.importMode = OobInfo.IMPORT_MODE_FILE;
                if (result == null) {
                    result = new ArrayList<>();
                }
                result.add(oobInfo);
            }
            return result;
        } catch (IOException | RuntimeException e) {

        } finally {
            if (br != null) {
                try {
                    br.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (fr != null) {
                try {
                    fr.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return null;
    }

}
