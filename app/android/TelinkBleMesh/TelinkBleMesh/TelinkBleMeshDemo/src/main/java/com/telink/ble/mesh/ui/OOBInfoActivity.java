package com.telink.ble.mesh.ui;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.MenuItem;
import android.widget.Toast;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.OOBPair;
import com.telink.ble.mesh.ui.adapter.BaseRecyclerViewAdapter;
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

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * show device composition data
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
    private AlertDialog.Builder deleteDialog;

    @SuppressLint("HandlerLeak")
    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            if (msg.what == MSG_IMPORT_COMPLETE) {
                dismissWaitingDialog();
                if (msg.obj != null) {
                    List<OOBPair> oobFromFile = (List<OOBPair>) msg.obj;
                    TelinkMeshApplication.getInstance().getMeshInfo().oobPairs.addAll(oobFromFile);
                    mAdapter.notifyDataSetChanged();
                    TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate(OOBInfoActivity.this);
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
        toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                if (item.getItemId() == R.id.item_oob_add) {
                    showModeSelectDialog();
                } else if (item.getItemId() == R.id.item_oob_clear) {
                    showClearDialog();
                }
                return false;
            }
        });

        mAdapter = new OOBListAdapter(this);
        mAdapter.setOnItemLongClickListener(new BaseRecyclerViewAdapter.OnItemLongClickListener() {
            @Override
            public boolean onLongClick(int position) {
                showDeleteConfirmDialog(position);
                return false;
            }
        });
        RecyclerView rv_oob = findViewById(R.id.rv_oob);
        rv_oob.setLayoutManager(new LinearLayoutManager(this));
        rv_oob.setAdapter(mAdapter);
    }

    private void showDeleteConfirmDialog(final int position) {
        deleteDialog = new AlertDialog.Builder(this);
        OOBPair pair = TelinkMeshApplication.getInstance().getMeshInfo().oobPairs.get(position);
        String[] title = new String[]{String.format("delete")};
        deleteDialog.setItems(title, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                TelinkMeshApplication.getInstance().getMeshInfo().oobPairs.remove(position);
                TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate(OOBInfoActivity.this);
                mAdapter.notifyItemRemoved(position);
                dialog.dismiss();
            }
        });
        deleteDialog.show();
    }


    private void showModeSelectDialog() {
        if (actionSelectDialog == null) {
            actionSelectDialog = new AlertDialog.Builder(this);
            actionSelectDialog.setItems(ACTIONS, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    if (which == 0) {
                        startActivityForResult(new Intent(OOBInfoActivity.this, OOBEditActivity.class), REQUEST_CODE_ADD_OOB);
                    } else if (which == 1) {
                        startActivityForResult(new Intent(OOBInfoActivity.this, FileSelectActivity.class)
                                        .putExtra(FileSelectActivity.KEY_SUFFIX, ".txt")
                                , REQUEST_CODE_SELECT_DATABASE);
                    }
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
                MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
                meshInfo.oobPairs.clear();
                meshInfo.saveOrUpdate(OOBInfoActivity.this);
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
            new Thread(new Runnable() {
                @Override
                public void run() {
                    List<OOBPair> parseResult = parseOOBDatabase(path);
                    mHandler.obtainMessage(MSG_IMPORT_COMPLETE, parseResult).sendToTarget();
                }
            }).start();
        } else if (requestCode == REQUEST_CODE_ADD_OOB) {
            // add success
            OOBPair pair = (OOBPair) data.getSerializableExtra(OOBEditActivity.EXTRA_OOB);
            List<OOBPair> pairs = TelinkMeshApplication.getInstance().getMeshInfo().oobPairs;
            pairs.add(pair);
            TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate(this);
            mAdapter.notifyItemInserted(pairs.size() - 1);
        } else if (requestCode == REQUEST_CODE_EDIT_OOB) {
            // edit success
            OOBPair pair = (OOBPair) data.getSerializableExtra(OOBEditActivity.EXTRA_OOB);
            final int position = data.getIntExtra(OOBEditActivity.EXTRA_POSITION, 0);
            TelinkMeshApplication.getInstance().getMeshInfo().oobPairs.set(position, pair);
            mAdapter.notifyItemChanged(position);
        }


    }


    /**
     * parse oob database
     */
    public List<OOBPair> parseOOBDatabase(String filePath) {
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
            List<OOBPair> result = null;
            OOBPair oobPair;
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

                oobPair = new OOBPair();
                oobPair.deviceUUID = uuid;
                oobPair.oob = oob;
                oobPair.timestamp = curTimestamp;
                oobPair.importMode = OOBPair.IMPORT_MODE_FILE;
                if (result == null) {
                    result = new ArrayList<>();
                }
                result.add(oobPair);
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
