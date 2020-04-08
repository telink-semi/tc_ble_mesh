package com.telink.ble.mesh.ui;

import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ScrollView;
import android.widget.TextView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.generic.OnOffSetMessage;
import com.telink.ble.mesh.core.message.generic.OnOffStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.util.Arrays;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;


/**
 * Created by kee on 2017/8/17.
 */

public class CmdActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {

    /**
     * preset messages
     */
    private final String[] PRESET_ACCESS_MESSAGES = {
            "Vendor On",
            "Vendor Off",
            "Vendor On/Off Get",
            "Vendor On NO-ACK",
            "Vendor Off NO-ACK",
            "Generic On",
            "Generic Off",
            "[Custom]"};
    // add new

    /*private final String[] TIDS = {
            "FALSE",
            "TRUE"};*/

    private final int MSG_DST_ADR = 0xFFFF;

    private int appKeyIndex;

    private int presetIndex = -1;

    private View ll_name;
    private EditText et_actions, et_dst_adr, et_opcode,
            et_rsp_opcode, et_rsp_max, et_retry_cnt, et_params, et_ttl, et_tid, et_name;
    private TextView tv_params_preview;
    private ImageView iv_toggle;
    private AlertDialog mShowPresetDialog;

    private static final int MSG_APPEND_LOG = 0x202;

    private static final int MSG_REFRESH_SCROLL = 0x201;

    private SimpleDateFormat dateFormat;
    private TextView tv_log;
    private ScrollView sv_log;
    private View ll_content;

    private Handler logHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == MSG_APPEND_LOG) {
                tv_log.append("\n" + dateFormat.format(new Date()) + ": " + msg.obj + "\n");
                logHandler.obtainMessage(MSG_REFRESH_SCROLL).sendToTarget();
            } else if (msg.what == MSG_REFRESH_SCROLL) {
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
        setContentView(R.layout.activity_message_assemble);
        initTitle();

        tv_params_preview = findViewById(R.id.tv_params_preview);
        et_dst_adr = findViewById(R.id.et_dst_adr);
        et_opcode = findViewById(R.id.et_opcode);
        et_rsp_opcode = findViewById(R.id.et_rsp_opcode);
        et_rsp_max = findViewById(R.id.et_rsp_max);
        et_retry_cnt = findViewById(R.id.et_retry_cnt);
        et_ttl = findViewById(R.id.et_ttl);
        et_tid = findViewById(R.id.et_tid);
        et_name = findViewById(R.id.et_name);
        ll_name = findViewById(R.id.ll_name);
        et_params = findViewById(R.id.et_params);
        et_params.addTextChangedListener(new FormatTextWatcher(et_params));
        et_actions = findViewById(R.id.et_actions);
        et_actions.setOnClickListener(this);

        tv_log = findViewById(R.id.tv_log);
        sv_log = findViewById(R.id.sv_log);
        iv_toggle = findViewById(R.id.iv_toggle);
        iv_toggle.setOnClickListener(this);
        ll_content = findViewById(R.id.ll_content);
        dateFormat = new SimpleDateFormat("HH:mm:ss.SSS", Locale.CHINA);
        appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();

        onActionSelect(0);
        TelinkMeshApplication.getInstance().addEventListener(OnOffStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(StatusNotificationEvent.EVENT_TYPE_NOTIFICATION_MESSAGE_UNKNOWN, this);
    }


    private void initTitle() {
        enableBackNav(true);
        setTitle("CMD");
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.message_assemble);
        toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                if (item.getItemId() == R.id.item_send) {
                    try {
                        MeshMessage meshMessage = assembleMessage();
                        if (meshMessage != null) {
                            MeshService.getInstance().sendMeshMessage(meshMessage);
                            logHandler.obtainMessage(MSG_APPEND_LOG, String.format("send message opcode: %04X", meshMessage.getOpcode())).sendToTarget();
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
                return false;
            }
        });
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.et_actions:
                showActionDialog();
                break;
            case R.id.iv_toggle:
                layoutToggle();
                break;
        }
    }

    private void layoutToggle() {
        if (ll_content.getVisibility() == View.VISIBLE) {
            ll_content.setVisibility(View.GONE);
            iv_toggle.setImageResource(R.drawable.ic_arrow_down);
        } else {
            ll_content.setVisibility(View.VISIBLE);
            iv_toggle.setImageResource(R.drawable.ic_arrow_up);
        }
    }

    private MeshMessage assembleMessage() throws Exception {
        String dstInput = et_dst_adr.getText().toString().trim();
        if (TextUtils.isEmpty(dstInput)) {
            toastMsg("input dst adr!");
            return null;
        }
        final int dstAdr = Integer.valueOf(dstInput, 16);
        if (dstAdr > 0xFFFF) {
            toastMsg("invalid dst adr!");
            return null;
        }
        String opcodeInput = et_opcode.getText().toString().trim();
        if (TextUtils.isEmpty(opcodeInput)) {
            toastMsg("input opcode!");
            return null;
        }
        final int opcode = Integer.valueOf(opcodeInput, 16);

        final byte[] params = Arrays.hexToBytes(et_params.getText().toString().trim());

        String rspOpcodeInput = et_rsp_opcode.getText().toString().trim();
        final int rspOpcode;
        final int rspMax;
        final int retryCnt;
        if (TextUtils.isEmpty(rspOpcodeInput)) {
            rspOpcode = MeshMessage.OPCODE_INVALID;
            rspMax = 0;
            retryCnt = 0;
        } else {
            rspOpcode = Integer.valueOf(rspOpcodeInput, 16);
            String rspMaxInput = et_rsp_max.getText().toString().trim();
            if (TextUtils.isEmpty(rspMaxInput)) {
                rspMax = 0;
            } else {
                rspMax = Integer.valueOf(rspMaxInput, 10);
            }


            String retryCntInput = et_retry_cnt.getText().toString().trim();
            if (TextUtils.isEmpty(retryCntInput)) {
                retryCnt = 0;
            } else {
                retryCnt = Integer.valueOf(retryCntInput, 10);
            }
        }

        String ttlInput = et_ttl.getText().toString().trim();
        if (TextUtils.isEmpty(ttlInput)) {
            toastMsg("input ttl!");
            return null;
        }

        final int ttl = Integer.valueOf(ttlInput, 10);


        String tidInput = et_tid.getText().toString().trim();
        if (TextUtils.isEmpty(tidInput)) {
            toastMsg("input tid position!");
            return null;
        }
        final int tidPosition = Integer.valueOf(tidInput);

        MeshMessage meshMessage = new MeshMessage();
        meshMessage.setDestinationAddress(dstAdr);
        meshMessage.setOpcode(opcode);
        meshMessage.setParams(params);
        meshMessage.setResponseOpcode(rspOpcode);
        meshMessage.setResponseMax(rspMax);
        meshMessage.setRetryCnt(retryCnt);
        meshMessage.setTtl(ttl);
        meshMessage.setTidPosition(tidPosition);
        return meshMessage;
    }


    private void resetUI(MeshMessage meshMessage) {
        if (meshMessage == null) return;

        final int dstAdr = meshMessage.getDestinationAddress();
        final int opcode = meshMessage.getOpcode();
        final byte[] params = meshMessage.getParams();
        final int rspOpcode = meshMessage.getResponseOpcode();
        final int rspMax = meshMessage.getResponseMax();
        final int retryCnt = meshMessage.getRetryCnt();
        final int ttl = meshMessage.getTtl();
        final int tidPosition = meshMessage.getTidPosition();

        et_dst_adr.setText(String.format("%04X", dstAdr));

        et_opcode.setText(MeshUtils.formatIntegerByHex(opcode));
        et_params.setText(Arrays.bytesToHexString(params));
        et_rsp_opcode.setText(
                rspOpcode == MeshMessage.OPCODE_INVALID
                        ?
                        "" : MeshUtils.formatIntegerByHex(rspOpcode));

        et_rsp_max.setText(String.valueOf(rspMax));
        et_retry_cnt.setText(String.valueOf(retryCnt));
        et_ttl.setText(String.valueOf(ttl));
        et_tid.setText(tidPosition < 0 ? "" : String.valueOf(tidPosition));
    }

    private void onActionSelect(int position) {
        this.presetIndex = position;
        et_actions.setText(PRESET_ACCESS_MESSAGES[position]);
        MeshMessage meshMessage = null;

        /*
            "Vendor On",
            "Vendor Off",
            "Vendor On/Off Get",
            "Vendor On NO-ACK",
            "Vendor Off NO-ACK",
            "Generic On",
            "Generic Off",
            "[Custom]"
         */
        final int onOffTidPosition = 1;
        switch (position) {
            case 0: // Vendor On
                meshMessage = createVendorMessage(0x0211C2, 0x0211C4, new byte[]{0x01, 0x00}, onOffTidPosition);
                break;

            case 1: // Vendor Off
                meshMessage = createVendorMessage(0x0211C2, 0x0211C4, new byte[]{0x00, 0x00}, onOffTidPosition);
                break;

            case 2: // vendor on/off get
                meshMessage = createVendorMessage(0x0211C1, 0x0211C4, null, onOffTidPosition);
                break;

            case 3: // Vendor On NO-ACK
                meshMessage = createVendorMessage(0x0211C3, MeshMessage.OPCODE_INVALID, new byte[]{0x01, 0x00}, onOffTidPosition);
                break;

            case 4: // Vendor Off NO-ACK
                meshMessage = createVendorMessage(0x0211C3, MeshMessage.OPCODE_INVALID, new byte[]{0x00, 0x00}, onOffTidPosition);
                break;

            case 5: // Generic On
                meshMessage = OnOffSetMessage.getSimple(MSG_DST_ADR, appKeyIndex, OnOffSetMessage.ON, true, 0);
                break;

            case 6: // Generic Off
                meshMessage = OnOffSetMessage.getSimple(MSG_DST_ADR, appKeyIndex, OnOffSetMessage.OFF, true, 0);
                break;
            case 7:
                createNewMessage();
                return;

        }
        resetUI(meshMessage);
    }

    private void createNewMessage() {
        et_dst_adr.setText(String.format("%04X", MSG_DST_ADR));
        et_opcode.setText("");
        et_params.setText("");
        et_rsp_opcode.setText("");

        et_rsp_max.setText("0");
        et_retry_cnt.setText(String.valueOf(MeshMessage.DEFAULT_RETRY_CNT));
        et_ttl.setText(String.valueOf(MeshMessage.DEFAULT_TTL));
        et_tid.setText("");
    }

    private MeshMessage createVendorMessage(int opcode, int rspOpcode, byte[] params, int tidPosition) {
        MeshMessage meshMessage = new MeshMessage();
        meshMessage.setDestinationAddress(MSG_DST_ADR);
        meshMessage.setOpcode(opcode);
        meshMessage.setParams(params);
        meshMessage.setResponseOpcode(rspOpcode);
//        meshMessage.setResponseMax(0);
//        meshMessage.setRetryCnt(2);
//        meshMessage.setTtl(5);
        meshMessage.setTidPosition(tidPosition);
        return meshMessage;
    }

    private void showActionDialog() {
        if (mShowPresetDialog == null) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setItems(PRESET_ACCESS_MESSAGES, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    onActionSelect(which);
                    mShowPresetDialog.dismiss();
                }
            });
            builder.setTitle("Actions");
            mShowPresetDialog = builder.create();
        }
        mShowPresetDialog.show();
    }


    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(OnOffStatusMessage.class.getName())) {
            logHandler.obtainMessage(MSG_APPEND_LOG, "On Off status notify").sendToTarget();
        } else if (event.getType().equals(StatusNotificationEvent.EVENT_TYPE_NOTIFICATION_MESSAGE_UNKNOWN)) {
            NotificationMessage notificationMessage = ((StatusNotificationEvent) event).getNotificationMessage();
            int opcode = notificationMessage.getOpcode();
            logHandler.obtainMessage(MSG_APPEND_LOG, String.format("Unknown status notify opcode:%04X", opcode) + " -- params:" + Arrays.bytesToHexString(notificationMessage.getParams())).sendToTarget();
        }
    }

    class FormatTextWatcher implements TextWatcher {
        EditText editText;

        FormatTextWatcher(EditText editText) {
            this.editText = editText;
        }

        @Override
        public void beforeTextChanged(CharSequence s, int start, int count, int after) {
        }

        @Override
        public void onTextChanged(CharSequence s, int start, int before, int count) {
            dealInput(s.toString());
        }

        @Override
        public void afterTextChanged(Editable s) {
        }
    }

    private void dealInput(String input) {

        byte[] params = Arrays.hexToBytes(input);
        if (params != null) {
            tv_params_preview.setText(Arrays.bytesToHexString(params, " "));
        } else {
            tv_params_preview.setText("");
        }
    }

}
