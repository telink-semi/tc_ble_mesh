package com.telink.ble.mesh.ui;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.util.Arrays;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import androidx.appcompat.widget.Toolbar;


/**
 * Created by kee on 2017/8/17.
 */

@Deprecated
public class CmdActivity_bak extends BaseActivity implements View.OnClickListener, EventListener<String> {

    private EditText et_cmd;

    /**
     * #reference: app_mesh.h
     * <p>
     * typedef struct{
     * u16 nk_idx;
     * u16 ak_idx;
     * u8 retry_cnt;   // only for reliable command
     * u8 rsp_max;     // only for reliable command
     * u16 adr_dst;
     * u8 op;
     * u8 par[MESH_CMD_ACCESS_LEN_MAX];
     * }mesh_bulk_cmd_par_t;
     * <p>
     * typedef struct{
     * u8 op;
     * u16 vendor_id;
     * u8 op_rsp;
     * u8 tid_pos;
     * u8 par[MESH_CMD_ACCESS_LEN_MAX];
     * }mesh_vendor_par_ini_t;
     */

    // {(byte) 0xa3, (byte) 0xff, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, (byte) 0xff, (byte) 0xff, (byte) 0xc2, 0x11, 0x02, (byte) 0xc4, 0x01, 0x01, 0x00}
    /// default 0xa3ff, network_key_index: 0x0000, app_key_index: 0x0000, retry_cnt: 0x02, rsp_max: 0x00 (00 与 01 同效果)
    private byte[][] iniSettings = {
            {(byte) 0xa3, (byte) 0xff, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, (byte) 0xff, (byte) 0xff, (byte) 0xc2, 0x11, 0x02, (byte) 0xc4, 0x01, 0x01, 0x00},
            {(byte) 0xa3, (byte) 0xff, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, (byte) 0xff, (byte) 0xff, (byte) 0xc2, 0x11, 0x02, (byte) 0xc4, 0x01, 0x00, 0x00},
            {(byte) 0xa3, (byte) 0xff, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, (byte) 0xff, (byte) 0xff, (byte) 0xc3, 0x11, 0x02, (byte) 0x00, 0x01, 0x01, 0x00},
            {(byte) 0xa3, (byte) 0xff, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, (byte) 0xff, (byte) 0xff, (byte) 0xc3, 0x11, 0x02, (byte) 0x00, 0x01, 0x00, 0x00},
            {(byte) 0xa3, (byte) 0xff, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, (byte) 0xff, (byte) 0xff, (byte) 0xc1, 0x11, 0x02},
            {(byte) 0xa3, (byte) 0xff, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, (byte) 0xff, (byte) 0xff, (byte) 0x82, 0x02, 0x01, 0x00}
    };
    private static final int MSG_APPEND_LOG = 0x202;

    private static final int MSG_REFRESH_SCROLL = 0x201;

    private SimpleDateFormat dateFormat;
    private TextView tv_log;
    private ScrollView logContainer;

    private Handler logHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == MSG_APPEND_LOG) {
                tv_log.append("\n" + dateFormat.format(new Date()) + ": " + msg.obj + "\n");
                logHandler.obtainMessage(MSG_REFRESH_SCROLL).sendToTarget();
            } else if (msg.what == MSG_REFRESH_SCROLL) {
                logContainer.fullScroll(View.FOCUS_DOWN);
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cmd_input);
        et_cmd = findViewById(R.id.et_cmd);
        et_cmd.addTextChangedListener(new FormatTextWatcher(et_cmd));
        Toolbar toolbar = findViewById(R.id.title_bar);
        enableBackNav(true);
        setTitle("CMD");
        findViewById(R.id.tv_send).setOnClickListener(this);

        findViewById(R.id.tv_vendor_on).setOnClickListener(this);
        findViewById(R.id.tv_vendor_off).setOnClickListener(this);
        findViewById(R.id.tv_get).setOnClickListener(this);
        findViewById(R.id.tv_vendor_on_na).setOnClickListener(this);
        findViewById(R.id.tv_vendor_off_na).setOnClickListener(this);
        findViewById(R.id.tv_generic_on).setOnClickListener(this);

        tv_log = findViewById(R.id.tv_log);
        logContainer = findViewById(R.id.sv_log);
//        dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS", Locale.CHINA);
        dateFormat = new SimpleDateFormat("HH:mm:ss.SSS", Locale.CHINA);
//        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_VENDOR_RESPONSE, this);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tv_send:
                String cmdInput = et_cmd.getText().toString().trim();
//                MeshLogger.log("cmd input -- " + Arrays.bytesToHexString(getInputBytes(cmdInput), ":"));
                byte[] input = getInputBytes(cmdInput);
//                MeshService.getInstance().sendOpByINI(input);
                String rspInfo = "sendCmd: " + Arrays.bytesToHexString(input, ":");
                logHandler.obtainMessage(MSG_APPEND_LOG, rspInfo).sendToTarget();
                break;


            case R.id.tv_vendor_on:
                insertIni(iniSettings[0]);
                break;

            case R.id.tv_vendor_off:
                insertIni(iniSettings[1]);
                break;

            case R.id.tv_get:
                insertIni(iniSettings[2]);
                break;

            case R.id.tv_vendor_on_na:
                insertIni(iniSettings[3]);
                break;

            case R.id.tv_vendor_off_na:
                insertIni(iniSettings[4]);
                break;

            case R.id.tv_generic_on:
                insertIni(iniSettings[5]);
                break;
        }
    }

    @Override
    public void performed(Event<String> event) {
        switch (event.getType()) {
            /*case NotificationEvent.EVENT_TYPE_VENDOR_RESPONSE:
                byte[] data = ((NotificationEvent) event).getRawData();
                int index = 6;
                int opcode = (data[index++] & 0xFF)
                        | ((data[index++] & 0xFF) << 8)
                        | ((data[index] & 0xFF) << 16);
                String rspInfo = "CMD rsp -- opcode: 0x" + Integer.toHexString(opcode) + " -- raw: " + Arrays.bytesToHexString(data, ":");
                logHandler.obtainMessage(MSG_APPEND_LOG, rspInfo).sendToTarget();
                break;*/
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
            dealInput(editText, s.toString(), before);
        }

        @Override
        public void afterTextChanged(Editable s) {

        }
    }

    private void insertIni(byte[] params) {
        et_cmd.setText(Arrays.bytesToHexString(params, " ").toUpperCase());
    }

    private byte[] getInputBytes(String input) {
        String[] byteStrs = input.split(" ");

        if (input.equals("")) {
            return null;
        }
        byte[] result = new byte[byteStrs.length];

        for (int i = 0; i < byteStrs.length; i++) {
//            result[i] = Byte.parseByte(byteStrs[i], 16);
            result[i] = (byte) (Integer.parseInt(byteStrs[i], 16) & 0xFF);

        }

        return result;

    }

    private boolean isValidInput(String input) {
        if (input.length() == 0) return true;
        if (input.charAt(0) == ' ') {
            return false;
        }

        char[] chars = input.toCharArray();
        for (int i = 0; i < chars.length; i++) {
            if ((i + 1) % 3 == 0) {
                if (chars[i] != ' ') {
                    return false;
                }
            } else if ((i + 1) % 3 != 0) {
                if (chars[i] == ' ') {
                    return false;
                }
            }
        }

        return true;
    }


    private String formatParamsInput(String input) {
        String hexString = input.replace(" ", "");

//        String[] hexBytes =
        int len = hexString.length() % 2 == 0 ? hexString.length() / 2 : hexString.length() / 2 + 1;

        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < len; i++) {
            if (i == len - 1 && hexString.length() % 2 != 0) {
                sb.append(hexString.substring(i * 2, i * 2 + 1));
            } else {

                sb.append(hexString.substring(i * 2, i * 2 + 2));
                if (i != len - 1) {
                    sb.append(" ");
                }
            }
        }
        return sb.toString().toUpperCase();
    }

    private void dealInput(EditText et, String input, int before) {

        if (!isValidInput(input)) {
            int sel = et.getSelectionStart();
            String result = formatParamsInput(input);

            et.setText(result);

            int inputLen = input.length();
            int resultLen = result.length();


            if (before < input.length() && inputLen + 1 == resultLen) {
                // 添加
                et.setSelection(sel + 1);
            } else {
                // 删除
                et.setSelection(sel);
            }

        }
    }
}
