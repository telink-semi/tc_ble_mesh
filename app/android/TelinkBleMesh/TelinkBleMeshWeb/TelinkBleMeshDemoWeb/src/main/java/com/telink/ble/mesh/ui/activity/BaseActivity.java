/********************************************************************************************************
 * @file BaseActivity.java
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
package com.telink.ble.mesh.ui.activity;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.provider.Settings;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

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
import com.telink.ble.mesh.web.ResponseInfo;


/**
 * base activity
 * Created by Administrator on 2017/2/21.
 */
@SuppressLint("Registered")
public class BaseActivity extends AppCompatActivity implements EventListener<String> {

    protected final String TAG = getClass().getSimpleName();

    private Toast toast;

    /**
     * waiting dialog
     */
    private AlertDialog waitingDialog;

    /**
     * waiting text
     */
    private TextView waitingTip;

    /**
     * shown when phone location is closed
     */
    private AlertDialog locationWarningDialog;

    /**
     * shown when bluetooth is closed
     */
    private AlertDialog bleStateDialog;

    /**
     * confirm dialog with 2 buttons, confirm and cancel
     */
    private AlertDialog confirmDialogBuilder;

    /**
     * dialog with only one confirm button
     */
    private AlertDialog tipDialog;

    // progressBar for loading
    private ProgressBar pb_loading;

    // parent view of progressBar
    private FrameLayout fl_pg_parent;

    protected View ll_empty;

    @Override
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

    /**
     * show loading progress bar below title
     */
    public void showIndeterminateLoading() {
        if (pb_loading == null) {
            pb_loading = findViewById(R.id.pb_loading);
        }
        if (pb_loading != null) {
            pb_loading.setIndeterminate(true);
        }
        if (fl_pg_parent == null) {
            fl_pg_parent = findViewById(R.id.fl_pg_parent);
        }
        if (fl_pg_parent != null) {
            fl_pg_parent.setVisibility(View.VISIBLE);
        }
    }

    /**
     * dismiss loading progress bar below title
     */
    public void dismissIndeterminateLoading() {
        if (pb_loading == null) {
            pb_loading = findViewById(R.id.pb_loading);
        }
        if (pb_loading != null) {
            pb_loading.setIndeterminate(false);
        }
        if (fl_pg_parent == null) {
            fl_pg_parent = findViewById(R.id.fl_pg_parent);
        }
        if (fl_pg_parent != null) {
            fl_pg_parent.setVisibility(View.INVISIBLE);
        }
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
            this.toast.cancel();
        }
        this.toast = Toast.makeText(getApplicationContext(), s, Toast.LENGTH_SHORT);
        this.toast.show();
    }

    public void showConfirmDialog(String msg, DialogInterface.OnClickListener confirmClick) {
        if (confirmDialogBuilder == null) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setCancelable(true);
            builder.setTitle("Warning");
            builder.setPositiveButton("Confirm", confirmClick);
            builder.setNegativeButton("Cancel", (dialog, which) -> dialog.dismiss());
            confirmDialogBuilder = builder.create();
        }
        confirmDialogBuilder.setMessage(msg);
        confirmDialogBuilder.show();
    }

    public void dismissConfirmDialog() {
        if (confirmDialogBuilder != null && confirmDialogBuilder.isShowing()) {
            confirmDialogBuilder.dismiss();
        }
    }

    public void showTipDialog(String msg) {
        showTipDialog(msg, true);
    }


    public void showTipDialog(String msg, boolean cancelable) {
        if (tipDialog == null) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("Tip");
            builder.setPositiveButton("Confirm", null);
            tipDialog = builder.create();
        }
        tipDialog.setMessage(msg);
        tipDialog.setCancelable(cancelable);
        tipDialog.show();
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
        if (waitingDialog == null) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            View dialogView = LayoutInflater.from(this).inflate(R.layout.view_dialog_waiting, null);
            waitingTip = dialogView.findViewById(R.id.waiting_tips);
            builder.setView(dialogView);
            builder.setCancelable(false);
            waitingDialog = builder.create();
        }
        if (waitingTip != null) {
            waitingTip.setText(tip);
        }
        waitingDialog.show();
    }

    public void dismissWaitingDialog() {
        if (waitingDialog != null && waitingDialog.isShowing()) {
            waitingDialog.dismiss();
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
            toolbar.setNavigationIcon(R.drawable.ic_arrow_left);
            toolbar.setNavigationOnClickListener(v -> onBackPressed());
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
                    showDialog = MeshService.getInstance().getCurrentMode() == MeshController.Mode.AUTO_CONNECT;
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

    protected void showItemSelectDialog(String title, String[] items, DialogInterface.OnClickListener onClickListener) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setItems(items, onClickListener);
        builder.setTitle(title);
        builder.show();
    }


    public void showError(String error) {
        if (error == null) return;
        MeshLogger.e(error);
        runOnUiThread(() -> toastMsg(error));
    }

    public void showSuccess(String message) {
        MeshLogger.i(message);
//        runOnUiThread(() -> toastMsg(message));
    }

    /**
     * @return false: if auth error
     */
    public boolean validateWebAuth(ResponseInfo responseInfo) {
        if (responseInfo.isAuthErr()) {
            runOnUiThread(() -> Toast.makeText(getApplicationContext(), responseInfo.message, Toast.LENGTH_SHORT).show());
            Intent intent = new Intent(this, LoginActivity.class);
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
            startActivity(intent);
            finish();
            return false;
        }
        return true;
    }

    public BaseActivity getBaseActivity() {
        return this;
    }

    public boolean validateConfigPermission() {
        if (!TelinkMeshApplication.getInstance().getMeshInfo().hasConfigPermission()) {
            showTipDialog("sorry, you do not have config permission, please contact with the creator");
            return false;
        }
        return true;
    }


    protected void updateEmptyVisibility(boolean visible) {
        if (ll_empty == null) return;
        ll_empty.setVisibility(visible ? View.VISIBLE : View.GONE);
    }
}
