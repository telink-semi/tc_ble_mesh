package com.telink.ble.mesh.ui.test;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.generic.OnOffGetMessage;
import com.telink.ble.mesh.core.message.generic.OnOffSetMessage;
import com.telink.ble.mesh.core.message.generic.OnOffStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.ui.BaseActivity;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;


/**
 * Created by kee on 2017/8/17.
 */

public class OnOffTestActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {

    private int appKeyIndex;

    private static final int MSG_REFRESH_SCROLL = 0x201;

    private static final int MSG_APPEND_LOG = 0x202;

    private static final int MSG_REFRESH_COUNT = 0x203;

    private Handler mHandler = new Handler();
    private SimpleDateFormat dateFormat;
    private Button btn_start;
    private TextView tv_log, tv_status_count;
    private ScrollView sv_log;
    private EditText et_delay;
    private boolean isTestStarted;
    private Handler logHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == MSG_APPEND_LOG) {
                tv_log.append("\n" + dateFormat.format(new Date()) + ": " + msg.obj + "\n");
                logHandler.obtainMessage(MSG_REFRESH_SCROLL).sendToTarget();
            } else if (msg.what == MSG_REFRESH_SCROLL) {
                sv_log.fullScroll(View.FOCUS_DOWN);
            } else if (msg.what == MSG_REFRESH_COUNT) {
                tv_status_count.setText("on/off status: " + statusCount);
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_on_off_test);
        initTitle();
        tv_status_count = findViewById(R.id.tv_status_count);
        tv_log = findViewById(R.id.tv_log);
        sv_log = findViewById(R.id.sv_log);
        et_delay = findViewById(R.id.et_delay);
        btn_start = findViewById(R.id.btn_start);
        btn_start.setOnClickListener(this);
        findViewById(R.id.btn_clear_log).setOnClickListener(this);
        dateFormat = new SimpleDateFormat("HH:mm:ss.SSS", Locale.CHINA);
        appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();

        TelinkMeshApplication.getInstance().addEventListener(OnOffStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(StatusNotificationEvent.EVENT_TYPE_NOTIFICATION_MESSAGE_UNKNOWN, this);
    }


    private void initTitle() {
        enableBackNav(true);
        setTitle("On Off Test");
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
        if (mHandler != null) {
            mHandler.removeCallbacksAndMessages(null);
        }
        if (logHandler != null) {
            logHandler.removeCallbacksAndMessages(null);
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_start:
                if (!isTestStarted) {
                    startTest();
                }
                break;

            case R.id.btn_clear_log:
                clearLog();
                break;

        }
    }

    private int statusCount = 0;
    private int delay;

    private void startTest() {

        String delayInput = et_delay.getText().toString().trim();
        if (TextUtils.isEmpty(delayInput)) {
            toastMsg("input delay time");
            return;
        }

        delay = Integer.parseInt(delayInput);
        if (delay >= 6000) {
            toastMsg("delay should < 6000");
            return;
        }

        isTestStarted = true;
        btn_start.setEnabled(false);
        statusCount = 0;
        logHandler.obtainMessage(MSG_REFRESH_COUNT).sendToTarget();
        sendGetCmd();
    }

    private void sendGetCmd() {
        appendLog("Get On/Off");
        OnOffGetMessage onOffGetMessage = new OnOffGetMessage(0xFFFF, appKeyIndex) {
            @Override
            public int getResponseOpcode() {
                return MeshMessage.OPCODE_INVALID;
            }
        };
        MeshService.getInstance().sendMeshMessage(onOffGetMessage);

//        long delay = 3 * 1000;
        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                sendOffCmd();
            }
        }, delay);
    }

    private void sendOffCmd() {
        appendLog("Set Off");
        OnOffSetMessage offSetMessage = OnOffSetMessage.getSimple(0xFFFF, appKeyIndex, OnOffSetMessage.OFF, false, 0);
        offSetMessage.setComplete(true);
        MeshService.getInstance().sendMeshMessage(offSetMessage);

        long delayTime = 6000 - delay;
        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                isTestStarted = false;
                btn_start.setEnabled(true);
                logHandler.obtainMessage(MSG_APPEND_LOG, "Test Stopped").sendToTarget();
            }
        }, delayTime);
    }

    private void appendLog(String log) {
        logHandler.obtainMessage(MSG_APPEND_LOG, log).sendToTarget();
    }

    private void clearLog() {
        tv_log.setText("");

    }


    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(OnOffStatusMessage.class.getName())) {
            statusCount++;
            logHandler.obtainMessage(MSG_REFRESH_COUNT).sendToTarget();
        }
    }

}
