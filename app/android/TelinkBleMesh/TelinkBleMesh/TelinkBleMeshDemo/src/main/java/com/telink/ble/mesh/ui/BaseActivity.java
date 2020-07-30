/********************************************************************************************************
 * @file     BaseActivity.java 
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
package com.telink.ble.mesh.ui;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.provider.Settings;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshController;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.BluetoothEvent;
import com.telink.ble.mesh.foundation.event.ScanEvent;
import com.telink.ble.mesh.util.MeshLogger;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;


/**
 * Created by Administrator on 2017/2/21.
 */
public class BaseActivity extends AppCompatActivity implements EventListener<String> {

    private AlertDialog.Builder confirmDialogBuilder;
    protected Toast toast;
    protected final String TAG = getClass().getSimpleName();
    private AlertDialog mWaitingDialog;
    private TextView waitingTip;

    private AlertDialog locationWarningDialog;

    private AlertDialog bleStateDialog;


    @Override
    @SuppressLint("ShowToast")
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        MeshLogger.w(TAG + " onCreate");
        this.toast = Toast.makeText(this, "", Toast.LENGTH_SHORT);
        TelinkMeshApplication.getInstance().addEventListener(ScanEvent.EVENT_TYPE_SCAN_LOCATION_WARNING, this);
        TelinkMeshApplication.getInstance().addEventListener(BluetoothEvent.EVENT_TYPE_BLUETOOTH_STATE_CHANGE, this);
    }

    protected boolean validateNormalStart(Bundle savedInstanceState) {
        if (savedInstanceState != null) {
            MeshLogger.w(TAG + " application recreate");
            Intent intent = new Intent(this, SplashActivity.class);
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
            startActivity(intent);
            finish();
            return false;
        }
        return true;
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(ScanEvent.EVENT_TYPE_SCAN_LOCATION_WARNING, this);
        TelinkMeshApplication.getInstance().removeEventListener(BluetoothEvent.EVENT_TYPE_BLUETOOTH_STATE_CHANGE, this);
        MeshLogger.w(TAG + " onDestroy");
        this.toast.cancel();
        this.toast = null;
    }

    @Override
    public void finish() {
        super.finish();
        MeshLogger.w(TAG + " finish");
    }

    @Override
    protected void onResume() {
        super.onResume();
        MeshLogger.w(TAG + " onResume");
    }

    public void toastMsg(CharSequence s) {

        if (this.toast != null) {
            this.toast.setView(this.toast.getView());
            this.toast.setDuration(Toast.LENGTH_SHORT);
            this.toast.setText(s);
            this.toast.show();
        }
    }

    public void showConfirmDialog(String msg, DialogInterface.OnClickListener confirmClick) {
        if (confirmDialogBuilder == null) {
            confirmDialogBuilder = new AlertDialog.Builder(this);
            confirmDialogBuilder.setCancelable(true);
            confirmDialogBuilder.setTitle("Warning");
//            confirmDialogBuilder.setMessage(msg);
            confirmDialogBuilder.setPositiveButton("Confirm", confirmClick);

            confirmDialogBuilder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                }
            });
        }
        confirmDialogBuilder.setMessage(msg);
        confirmDialogBuilder.show();
    }

    public void showLocationDialog() {
        if (locationWarningDialog == null) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setCancelable(true);
            builder.setTitle("Warning");
            builder.setMessage(R.string.message_location_disabled_warning);
            builder.setPositiveButton("Enable", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    Intent enableLocationIntent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
                    startActivityForResult(enableLocationIntent, 1);
                }
            });
            builder.setNegativeButton("Ignore", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                }
            });
            builder.setNeutralButton("Never Mind", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    SharedPreferenceHelper.setLocationIgnore(BaseActivity.this, true);
                    dialog.dismiss();
                }
            });
            locationWarningDialog = builder.create();
            locationWarningDialog.show();
        } else if (!locationWarningDialog.isShowing()) {
            locationWarningDialog.show();
        }
    }

    private void showBleStateDialog() {
        if (bleStateDialog == null) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setCancelable(true);
            builder.setTitle("Warning");
            builder.setMessage(R.string.message_ble_state_disabled);
            builder.setPositiveButton("Enable", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    MeshService.getInstance().enableBluetooth();
                }
            });
            builder.setNegativeButton("Ignore", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                }
            });
            builder.setNeutralButton("Go-Setting", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    Intent enableLocationIntent = new Intent(Settings.ACTION_BLUETOOTH_SETTINGS);
                    startActivityForResult(enableLocationIntent, 1);
                }
            });
            bleStateDialog = builder.create();
            bleStateDialog.show();
        } else if (!bleStateDialog.isShowing()) {
            bleStateDialog.show();
        }
    }

    private void dismissBleStateDialog() {
        if (bleStateDialog != null && bleStateDialog.isShowing()) {
            bleStateDialog.dismiss();
        }
    }


    public void showWaitingDialog(String tip) {
        if (mWaitingDialog == null) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            View dialogView = LayoutInflater.from(this).inflate(R.layout.view_dialog_waiting, null);
            waitingTip = dialogView.findViewById(R.id.waiting_tips);
            builder.setView(dialogView);
            builder.setCancelable(false);
            mWaitingDialog = builder.create();
        }
        if (waitingTip != null) {
            waitingTip.setText(tip);
        }
        mWaitingDialog.show();
    }

    public void dismissWaitingDialog() {
        if (mWaitingDialog != null && mWaitingDialog.isShowing()) {
            mWaitingDialog.dismiss();
        }
    }

    protected void setTitle(String title) {
        TextView tv_title = findViewById(R.id.tv_title);
        if (tv_title != null) {
            tv_title.setText(title);
        }
    }

    protected void enableBackNav(boolean enable) {
        Toolbar toolbar = findViewById(R.id.title_bar);
        if (enable) {
            toolbar.setNavigationOnClickListener(new View.OnClickListener() {
                public void onClick(View v) {
                    finish();
                }
            });
        } else {
            toolbar.setNavigationIcon(null);
        }

    }

    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(ScanEvent.EVENT_TYPE_SCAN_LOCATION_WARNING)) {
            if (!SharedPreferenceHelper.isLocationIgnore(this)) {
                boolean showDialog;
                if (this instanceof MainActivity) {
                    showDialog = MeshService.getInstance().getCurrentMode() == MeshController.Mode.MODE_AUTO_CONNECT;
                } else {
                    showDialog = true;
                }
                if (showDialog) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            showLocationDialog();
                        }
                    });
                }
            }
        } else if (event.getType().equals(BluetoothEvent.EVENT_TYPE_BLUETOOTH_STATE_CHANGE)) {
            int state = ((BluetoothEvent) event).getState();
            if (state == BluetoothAdapter.STATE_OFF) {
                showBleStateDialog();
            } else if (state == BluetoothAdapter.STATE_ON) {
                dismissBleStateDialog();
            }
        }
    }
}
