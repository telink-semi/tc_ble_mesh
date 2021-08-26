/********************************************************************************************************
 * @file DeviceConfigActivity.java
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
package com.telink.ble.mesh.ui;

import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.AppCompatSpinner;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.config.DefaultTTLGetMessage;
import com.telink.ble.mesh.core.message.config.DefaultTTLSetMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.ReliableMessageProcessEvent;
import com.telink.ble.mesh.model.ConfigState;
import com.telink.ble.mesh.model.DeviceConfig;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.ui.adapter.DeviceConfigListAdapter;
import com.telink.ble.mesh.ui.widget.EditableRecyclerView;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.ArrayList;
import java.util.List;

/**
 * container for device control , group,  device settings
 * Created by kee on 2017/8/30.
 */
public class DeviceConfigSettingActivity extends BaseActivity implements EventListener<String> {

    NodeInfo deviceInfo;
    DeviceConfig config;
    private Handler delayHandler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_device_config_setting);

        final Intent intent = getIntent();
        if (intent.hasExtra("meshAddress")) {
            int address = intent.getIntExtra("meshAddress", -1);
            deviceInfo = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(address);
        } else {
            toastMsg("device address null");
            finish();
            return;
        }

        if (deviceInfo == null) {
            toastMsg("device info null");
            finish();
            return;
        }

        config = (DeviceConfig) intent.getSerializableExtra("deviceConfig");
        if (config == null) return;
        /*if (config.configState == ConfigState.DEFAULT_TTL) {

        }*/

        setTitle(config.configState.name);
        enableBackNav(true);
        initConfigView();
        TelinkMeshApplication.getInstance().addEventListener(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, this);
    }

    private void initConfigView() {
        switch (config.configState) {
            case DEFAULT_TTL:
                initTTLConfigView();
                break;

            case RELAY:
//                initRelayConfigView();
                initRelayConfigViewSp();
                break;

            case SECURE_NETWORK_BEACON:
                initBeaconConfigView();
                break;

            case PROXY:
                initProxyConfigView();
                break;

            case NODE_IDENTITY:

                break;

            case FRIEND:
                break;

            case NETWORK_TRANSMIT:
                break;
        }
    }

    private void initTTLConfigView() {
        LinearLayout parentLayout = findViewById(R.id.ll_parent);
        View view = LayoutInflater.from(this).inflate(R.layout.layout_config_ttl, parentLayout);
        final EditText et_input = view.findViewById(R.id.et_input);

        findViewById(R.id.btn_confirm).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String input = et_input.getText().toString().trim();
                if (TextUtils.isEmpty(input)) {
                    Toast.makeText(DeviceConfigSettingActivity.this, "input error", Toast.LENGTH_SHORT).show();
                }
            }
        });
    }


    private void initRelayConfigViewSp() {
        LinearLayout parentLayout = findViewById(R.id.ll_parent);
        View view = LayoutInflater.from(this).inflate(R.layout.layout_cfg_relay_spinner, parentLayout);
        final String[] RELAY_OPTIONS = new String[]{"Enabled", "Disabled"};
        final int[] selectedIndex = {0};
        final AppCompatSpinner spinner = view.findViewById(R.id.sp_relay);

        spinner.setDropDownWidth(400);
        spinner.setDropDownHorizontalOffset(100);
        spinner.setDropDownVerticalOffset(100);
        spinner.setSelection(0);
        ArrayAdapter<String> spinnerAdapter = new ArrayAdapter<String>(this,
                R.layout.layout_config_spinner_item, R.id.tv_name, RELAY_OPTIONS);
        spinner.setAdapter(spinnerAdapter);
        spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                selectedIndex[0] = position;
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        findViewById(R.id.btn_confirm).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(DeviceConfigSettingActivity.this, RELAY_OPTIONS[selectedIndex[0]], Toast.LENGTH_SHORT).show();

            }
        });
    }

    private void initRelayConfigView() {
        LinearLayout parentLayout = findViewById(R.id.ll_parent);
        View view = LayoutInflater.from(this).inflate(R.layout.layout_config_relay, parentLayout);
        final String[] RELAY_OPTIONS = new String[]{"Enabled", "Disabled"};
        final int[] selectedIndex = {0};
        final EditText et_input = view.findViewById(R.id.et_input);

        et_input.setText(RELAY_OPTIONS[selectedIndex[0]]);
        et_input.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showItemSelectDialog("Select Relay Value", RELAY_OPTIONS, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        selectedIndex[0] = which;
                        et_input.setText(RELAY_OPTIONS[selectedIndex[0]]);
                    }
                });
            }
        });

        findViewById(R.id.btn_confirm).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(DeviceConfigSettingActivity.this, RELAY_OPTIONS[selectedIndex[0]], Toast.LENGTH_SHORT).show();

            }
        });
    }

    /**
     * show secure beacon
     */
    private void initBeaconConfigView() {
        LinearLayout parentLayout = findViewById(R.id.ll_parent);
        View view = LayoutInflater.from(this).inflate(R.layout.layout_config_beacon, parentLayout);
        final String[] BEACON_OPTIONS = new String[]{"Open the Broadcasting", "Close the Broadcasting"};
        final int[] selectedIndex = {0};
        final EditText et_input = view.findViewById(R.id.et_input);

        et_input.setText(BEACON_OPTIONS[selectedIndex[0]]);
        et_input.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showItemSelectDialog("Select Beacon Value", BEACON_OPTIONS, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        selectedIndex[0] = which;
                        et_input.setText(BEACON_OPTIONS[selectedIndex[0]]);
                    }
                });
            }
        });

        findViewById(R.id.btn_confirm).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(DeviceConfigSettingActivity.this, BEACON_OPTIONS[selectedIndex[0]], Toast.LENGTH_SHORT).show();

            }
        });
    }


    private void initProxyConfigView() {
        LinearLayout parentLayout = findViewById(R.id.ll_parent);
        View view = LayoutInflater.from(this).inflate(R.layout.layout_config_proxy, parentLayout);
        final String[] PROXY_OPTIONS = new String[]{"Enabled", "Disabled"};
        final int[] selectedIndex = {0};
        final EditText et_input = view.findViewById(R.id.et_input);

        et_input.setText(PROXY_OPTIONS[selectedIndex[0]]);
        et_input.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showItemSelectDialog("Select Relay Value", PROXY_OPTIONS, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        selectedIndex[0] = which;
                        et_input.setText(PROXY_OPTIONS[selectedIndex[0]]);
                    }
                });
            }
        });

        findViewById(R.id.btn_confirm).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(DeviceConfigSettingActivity.this, PROXY_OPTIONS[selectedIndex[0]], Toast.LENGTH_SHORT).show();

            }
        });
    }


    private void showSendWaitingDialog(String message) {
        showWaitingDialog(message);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (delayHandler != null) {
            delayHandler.removeCallbacksAndMessages(null);
        }
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }


    @Override
    public void performed(Event event) {
        if (event.getType().equals(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED)) {
            refreshUI();
        } else if (event.getType().equals(ReliableMessageProcessEvent.EVENT_TYPE_MSG_PROCESS_COMPLETE)) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    dismissWaitingDialog();
                }
            });
        }
    }

    private void refreshUI() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {

            }
        });
    }


    static class ConfigSetView extends RecyclerView {

        public ConfigSetView(@NonNull Context context) {
            super(context);
        }

        public ConfigSetView(@NonNull Context context, @Nullable AttributeSet attrs) {
            super(context, attrs);
        }

        public ConfigSetView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyle) {
            super(context, attrs, defStyle);
        }

        public void init(Context context) {
            this.setLayoutManager(new LinearLayoutManager(context));

        }

    }

}
