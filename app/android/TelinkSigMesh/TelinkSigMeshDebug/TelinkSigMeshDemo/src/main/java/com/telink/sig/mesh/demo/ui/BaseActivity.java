/********************************************************************************************************
 * @file BaseActivity.java
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

import android.annotation.SuppressLint;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.telink.sig.mesh.demo.ActivityManager;
import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.event.Event;
import com.telink.sig.mesh.event.EventListener;
import com.telink.sig.mesh.light.MeshController;
import com.telink.sig.mesh.util.TelinkLog;


/**
 * Created by Administrator on 2017/2/21.
 */
public class BaseActivity extends AppCompatActivity implements EventListener<String> {

    private AlertDialog.Builder confirmDialogBuilder;
    protected Toast toast;
    protected final String TAG = getClass().getSimpleName();
    private AlertDialog mWaitingDialog;
    private TextView waitingTip;

    @Override
    @SuppressLint("ShowToast")
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        TelinkLog.w(TAG + " onCreate");

        this.toast = Toast.makeText(this, "", Toast.LENGTH_SHORT);
        TelinkMeshApplication.getInstance().addEventListener(MeshController.EVENT_TYPE_SERVICE_DESTROY, this);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(MeshController.EVENT_TYPE_SERVICE_DESTROY, this);
        TelinkLog.w(TAG + " onDestroy");
        this.toast.cancel();
        this.toast = null;
    }

    @Override
    public void finish() {
        super.finish();
        TelinkLog.w(TAG + " finish");
    }

    @Override
    protected void onResume() {
        super.onResume();
        TelinkLog.w(TAG + " onResume");
    }

    @Override
    protected void onStart() {
        super.onStart();
        ActivityManager.getInstance().onActivityStart();
    }

    @Override
    protected void onStop() {
        super.onStop();
        ActivityManager.getInstance().onActivityStop();
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
            confirmDialogBuilder.setTitle("Warn");
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
        if (!ActivityManager.getInstance().isApplicationForeground() && event.getType().equals(MeshController.EVENT_TYPE_SERVICE_DESTROY)) {
            TelinkLog.d(TAG + "-- service destroyed event");
            finish();
        }
    }
}
