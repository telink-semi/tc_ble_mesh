package com.telink.ble.mesh.ui;

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.ScrollView;
import android.widget.TextView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.config.NodeResetMessage;
import com.telink.ble.mesh.core.message.config.NodeResetStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;

import java.text.SimpleDateFormat;
import java.util.Date;

import androidx.appcompat.app.AlertDialog;


/**
 * Created by kee on 2017/8/17.
 */

public class LpnSettingActivity extends BaseActivity implements EventListener<String> {

    private MeshInfo mesh;
    private NodeInfo deviceInfo;
    private int eleAdr;

    private static final int OP_VENDOR_GET = 0x0211E0;

    private static final int OP_VENDOR_STATUS = 0x0211E1;

    private static final int VENDOR_MODEL_ID = 0x0211;

    private TextView tv_log;

    private ScrollView sv_log;

    private AlertDialog confirmDialog;

    private Handler delayHandler = new Handler();

    private static final int MSG_APPEND_LOG = 0;

    private static final int MSG_SCROLL_DOWN = 1;

    SimpleDateFormat dateFormat = new SimpleDateFormat("HH:mm:ss");

    private Handler msgHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == MSG_APPEND_LOG) {
                tv_log.append((String) msg.obj);
                msgHandler.obtainMessage(MSG_SCROLL_DOWN).sendToTarget();
            } else if (msg.what == MSG_SCROLL_DOWN) {
                sv_log.fullScroll(View.FOCUS_DOWN);
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_lpn_setting);
        setTitle("LPN Setting");
        enableBackNav(true);
        findViewById(R.id.btn_get_lpn_status).setOnClickListener(this.bvClick);
        findViewById(R.id.btn_kick).setOnClickListener(this.bvClick);
        tv_log = findViewById(R.id.tv_log);
        sv_log = findViewById(R.id.sv_log);
        Intent intent = getIntent();
        int address = intent.getIntExtra("deviceAddress", -1);
        if (address == -1) {
            finish();
            return;
        }
        mesh = TelinkMeshApplication.getInstance().getMeshInfo();
        deviceInfo = mesh.getDeviceByMeshAddress(address);
        eleAdr = deviceInfo.getTargetEleAdr(VENDOR_MODEL_ID);
        if (eleAdr == -1) {
            toastMsg("vendor model not found");
            return;
        }

        TelinkMeshApplication.getInstance().addEventListener(NodeResetStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(StatusNotificationEvent.EVENT_TYPE_NOTIFICATION_MESSAGE_UNKNOWN, this);
//        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_VENDOR_RESPONSE, this);
    }

    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(NodeResetStatusMessage.class.getName())) {
            onKickOutFinish();
        } else if (event.getType().equals(StatusNotificationEvent.EVENT_TYPE_NOTIFICATION_MESSAGE_UNKNOWN)) {
            StatusNotificationEvent statusNotificationEvent = (StatusNotificationEvent) event;
            NotificationMessage message = statusNotificationEvent.getNotificationMessage();
            int opcode = message.getOpcode();
            if (opcode == OP_VENDOR_STATUS) {
                byte[] params = message.getParams();
                if (params.length == 4) {
                    int humidity = (params[0] & 0xFF) + ((params[1] & 0xFF) << 8);
                    int temperature = (params[2] & 0xFF) + ((params[3] & 0xFF) << 8);
                    msgHandler.obtainMessage(MSG_APPEND_LOG, dateFormat.format(new Date()) + " Sensor State STATUS : H=" + humidity + " T=" + temperature + '\n')
                            .sendToTarget();
                }
            }
        }
        /*if (event.getType().equals(NotificationEvent.EVENT_TYPE_VENDOR_RESPONSE)) {
            NotificationInfo info = ((NotificationEvent) event).getNotificationInfo();
            MeshLogger.log("vendor response: " + info.toString());
            if (info.srcAdr == eleAdr && info.opcode == OP_VENDOR_STATUS) {
                byte[] params = info.params;
                if (params.length == 4) {
                    int humidity = (params[0] & 0xFF) + ((params[1] & 0xFF) << 8);
                    int temperature = (params[2] & 0xFF) + ((params[3] & 0xFF) << 8);
                    msgHandler.obtainMessage(MSG_APPEND_LOG, dateFormat.format(new Date()) + " Sensor State STATUS : H=" + humidity + " T=" + temperature + '\n')
                            .sendToTarget();
                } else if (params.length == 2) {

                }


            }


        } else if (event.getType().equals(NotificationEvent.EVENT_TYPE_KICK_OUT_CONFIRM)) {

            onKickOutFinish();

        }*/
    }

    private void showKickConfirmDialog() {
        if (confirmDialog == null) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setCancelable(true);
            builder.setTitle("Warn");
            builder.setMessage("Confirm to remove device?");
            builder.setPositiveButton("Confirm", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    kickOut();
                }
            });

            builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                }
            });
            confirmDialog = builder.create();
        }
        confirmDialog.show();
    }

    private void kickOut() {
        // send reset message
        MeshService.getInstance().sendMeshMessage(new NodeResetMessage(deviceInfo.meshAddress));
        showWaitingDialog("kick out processing");
        delayHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                onKickOutFinish();
            }
        }, 3 * 1000);
    }

    private void onKickOutFinish() {
        delayHandler.removeCallbacksAndMessages(null);
        MeshService.getInstance().removeDevice(deviceInfo.meshAddress);
        TelinkMeshApplication.getInstance().getMeshInfo().removeDeviceByMeshAddress(deviceInfo.meshAddress);
        TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate(getApplicationContext());
        dismissWaitingDialog();
        finish();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        delayHandler.removeCallbacksAndMessages(null);
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    public View.OnClickListener bvClick = new View.OnClickListener() {
        @Override
        public void onClick(View v) {

            switch (v.getId()) {

                case R.id.btn_get_lpn_status:
                    MeshMessage meshMessage = new MeshMessage();
                    meshMessage.setDestinationAddress(deviceInfo.meshAddress);
                    meshMessage.setOpcode(OP_VENDOR_GET);
                    meshMessage.setResponseOpcode(OP_VENDOR_STATUS);
                    meshMessage.setParams(new byte[]{0, 0});
                    if (MeshService.getInstance().sendMeshMessage(meshMessage)) {
                        msgHandler.obtainMessage(MSG_APPEND_LOG, dateFormat.format(new Date()) + " Sensor State GET \n").sendToTarget();
                    }
//                    meshMessage.setTidPosition(tidPosition);
                    /*
                    final byte[] params = new byte[]{
                            (byte) 0xa3, (byte) 0xff, //flag
                            (byte) 0x00, (byte) 0x00, // netKey index
                            (byte) 0x00, (byte) 0x00, // appKey index
                            (byte) 0x02, // retry cnt
                            (byte) 0x00, // rsp max
                            (byte) 0x01, (byte) 0x00, // dest address
                            (byte) 0xe0, (byte) 0x11, (byte) 0x02, // vendor opcode
                            (byte) 0xe1, (byte) 0x02, (byte) 0x00, (byte) 0x00}; // params
                    */
//                    Mesh mesh = TelinkMeshApplication.getInstance().getMeshInfo();

                    // todo mesh interface
                    /*MeshCommand command = MeshCommand.newInstance(
                            mesh.netKeyIndex, mesh.appKeyIndex,
                            1, eleAdr, OP_VENDOR_GET);
                    command.params = new byte[]{(byte) 0xe1, (byte) 0x02, (byte) 0x00, (byte) 0x00};
                    if (MeshService.getInstance().sendMeshCommand(command)) {
                        msgHandler.obtainMessage(MSG_APPEND_LOG, dateFormat.format(new Date()) + " Sensor State GET \n").sendToTarget();
                    }*/

                    break;

                case R.id.btn_kick:
                    showKickConfirmDialog();
                    break;

            }
        }
    };
}
