/********************************************************************************************************
 * @file ResponseTestActivity.java
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
package com.telink.ble.mesh.ui.test;

import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;

import androidx.appcompat.app.AlertDialog;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.OnOffSetMessage;
import com.telink.ble.mesh.core.message.generic.OnOffStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.ReliableMessageProcessEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.ui.activity.BaseActivity;
import com.telink.ble.mesh.ui.adapter.LogInfoAdapter;
import com.telink.ble.mesh.util.LogInfo;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;


/**
 * send all on/off cmd , 统计收齐所有回复的时间
 * test for calculate response time after send command
 * Created by kee on 2021/3/17.
 */

public class ResponseTestActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {

    private final String[] CMD_ACTION = {
            "ALL ON",
            "ALL OFF"};
    private int appKeyIndex;
    private Handler mHandler = new Handler();
    private Button btn_start;
    private EditText et_cmd_action;
    private EditText et_interval;
    private EditText et_cnt;
    private CheckBox cb_scroll;
    private AlertDialog cmdDialog;
    /**
     * read from text edit
     */
    private int targetCnt = 0;

    /**
     * 0 for ALL OFF
     * 1 for ALL ON
     */
    private int cmdType = 1;

    /**
     * test interval
     */
    private int interval = 3 * 1000;
    /**
     * test count
     */
    private int testCnt = 0;

    /**
     * on/off status received every round
     */
    private int roundRcvCnt = 0;

    private long roundStartTime = 0;

    private boolean autoScroll = true;

    private RecyclerView rv_log;

    private List<LogInfo> logs = new ArrayList<>();

    private LogInfoAdapter logInfoAdapter;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_response_test);
        initTitle();

        logInfoAdapter = new LogInfoAdapter(this, logs);
        rv_log = findViewById(R.id.rv_log);
        rv_log.setLayoutManager(new LinearLayoutManager(this));
        rv_log.setAdapter(logInfoAdapter);
        cb_scroll = findViewById(R.id.cb_scroll);
        autoScroll = cb_scroll.isChecked();
        cb_scroll.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                autoScroll = isChecked;
            }
        });
        et_cmd_action = findViewById(R.id.et_cmd_action);
        et_cmd_action.setOnClickListener(this);
        et_interval = findViewById(R.id.et_interval);
        et_cnt = findViewById(R.id.et_cnt);
        btn_start = findViewById(R.id.btn_start);
        btn_start.setOnClickListener(this);
        findViewById(R.id.btn_clear_log).setOnClickListener(this);
        appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();

        TelinkMeshApplication.getInstance().addEventListener(OnOffStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(StatusNotificationEvent.EVENT_TYPE_NOTIFICATION_MESSAGE_UNKNOWN, this);
        TelinkMeshApplication.getInstance().addEventListener(ReliableMessageProcessEvent.EVENT_TYPE_MSG_PROCESS_COMPLETE, this);
    }


    private void initTitle() {
        enableBackNav(true);
        setTitle("Response Test");
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
        if (mHandler != null) {
            mHandler.removeCallbacksAndMessages(null);
        }
    }

    private void showActionDialog() {
        if (cmdDialog == null) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setItems(CMD_ACTION, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    cmdType = which;
                    et_cmd_action.setText(CMD_ACTION[which]);
                    cmdDialog.dismiss();
                }
            });
            builder.setTitle("Select First Command");
            cmdDialog = builder.create();
        }
        cmdDialog.show();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_start:
                startTest();
                break;

            case R.id.btn_clear_log:
                clearLog();
                break;

            case R.id.et_cmd_action:
                showActionDialog();
                break;

        }
    }


    private void startTest() {
        String intervalInput = et_interval.getText().toString().trim();
        if (TextUtils.isEmpty(intervalInput)) {
            toastMsg("input interval time");
            return;
        }
        interval = Integer.parseInt(intervalInput);

        String countInput = et_cnt.getText().toString().trim();
        if (TextUtils.isEmpty(countInput)) {
            toastMsg("input count");
            return;
        }
        targetCnt = Integer.parseInt(countInput);
        enableUI(false);
        roundStart();
    }

    private void enableUI(boolean enable) {
        btn_start.setEnabled(enable);
        et_cmd_action.setEnabled(enable);
        et_interval.setEnabled(enable);
        et_cnt.setEnabled(enable);
    }

    private void roundStart() {
        addLog(String.format(Locale.getDefault(), "round start(%d)", testCnt + 1));
        roundRcvCnt = 0;
        roundStartTime = System.currentTimeMillis();
        sendOffCmd();
    }

    private void sendOffCmd() {
        String onOff = cmdType == 0 ? "OFF(0)" : "ON(1)";
        addLog(" send on/off - " + onOff);
        cmdType = cmdType == 0 ? 1 : 0;
        int rspMax = TelinkMeshApplication.getInstance().getMeshInfo().getOnlineCountInAll();
        OnOffSetMessage offSetMessage = OnOffSetMessage.getSimple(0xFFFF, appKeyIndex, cmdType, true, rspMax);
        offSetMessage.setComplete(true);
        if (!MeshService.getInstance().sendMeshMessage(offSetMessage)) {
            addLog("err: cmd send fail");
            onRoundComplete(0);
        }
    }

    private Runnable nextRoundTask = new Runnable() {
        @Override
        public void run() {
            roundStart();
        }
    };

    private void clearLog() {
        logs.clear();
        logInfoAdapter.notifyDataSetChanged();
        if (autoScroll) {
            rv_log.smoothScrollToPosition(logs.size() - 1);
        }
    }


    /**
     * round complete, prepare for next round
     */
    private void onRoundComplete(long timeSpent) {
        addLog("time spent(ms) : " + timeSpent);
        addLog(String.format(Locale.getDefault(), "round complete (%d) \n\n", testCnt + 1));
        testCnt++;
        if (testCnt < targetCnt) {
            mHandler.postDelayed(nextRoundTask, interval);
        } else {
            addLog(" ===== test complete ===== \n\n");
            testCnt = 0;
            enableUI(true);
        }
    }

    private void addLog(String log) {
        addLog(log, MeshLogger.LEVEL_DEBUG);
    }

    private void addLog(String log, int level) {
        MeshLogger.d(log);
        logs.add(new LogInfo("RSP-TEST", log, level));
        logInfoAdapter.notifyDataSetChanged();
        if (autoScroll) {
            rv_log.smoothScrollToPosition(logs.size() - 1);
        }
    }

    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(OnOffStatusMessage.class.getName())) {
            NotificationMessage notificationMessage = ((StatusNotificationEvent) event).getNotificationMessage();
            OnOffStatusMessage msg = (OnOffStatusMessage) notificationMessage.getStatusMessage();
            int onOff = msg.isComplete() ? msg.getTargetOnOff() : msg.getPresentOnOff();
//            addLog(String.format("msg received: on/off -- %b | address -- %04X", onOff == 1, notificationMessage.getSrc()));

        } else if (event.getType().equals(ReliableMessageProcessEvent.EVENT_TYPE_MSG_PROCESS_COMPLETE)) {
            ReliableMessageProcessEvent msgPcEvent = (ReliableMessageProcessEvent) event;
            boolean success = msgPcEvent.isSuccess();
            if (msgPcEvent.getOpcode() != Opcode.G_ONOFF_SET.value) {
                MeshLogger.w("not on/off opcode");
                return;
            }
            addLog(String.format(Locale.getDefault(), "on/off msg complete :   rspCnt: %d rspMax: %d  success: %b",
                    msgPcEvent.getRspCount(), msgPcEvent.getRspMax(), msgPcEvent.isSuccess()));
            onRoundComplete(System.currentTimeMillis() - roundStartTime);
        }
    }

}
