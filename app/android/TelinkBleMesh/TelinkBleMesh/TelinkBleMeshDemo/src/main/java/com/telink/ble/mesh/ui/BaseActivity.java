package com.telink.ble.mesh.ui;

import android.annotation.SuppressLint;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.util.MeshLogger;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;


/**
 * Created by Administrator on 2017/2/21.
 */
public class BaseActivity extends AppCompatActivity {

    private AlertDialog.Builder confirmDialogBuilder;
    protected Toast toast;
    protected final String TAG = getClass().getSimpleName();
    private AlertDialog mWaitingDialog;
    private TextView waitingTip;


    @Override
    @SuppressLint("ShowToast")
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        MeshLogger.w(TAG + " onCreate");
        this.toast = Toast.makeText(this, "", Toast.LENGTH_SHORT);
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
}
