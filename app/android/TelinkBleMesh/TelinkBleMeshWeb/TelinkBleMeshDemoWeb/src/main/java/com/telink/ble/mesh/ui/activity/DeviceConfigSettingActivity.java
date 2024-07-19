/********************************************************************************************************
 * @file DeviceConfigSettingActivity.java
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
//package com.telink.ble.mesh.ui;
//
//import android.content.Context;
//import android.content.Intent;
//import android.os.Bundle;
//import android.os.Handler;
//import android.text.TextUtils;
//import android.util.AttributeSet;
//import android.view.LayoutInflater;
//import android.view.View;
//import android.widget.AdapterView;
//import android.widget.ArrayAdapter;
//import android.widget.EditText;
//import android.widget.LinearLayout;
//import android.widget.Toast;
//
//import androidx.annotation.NonNull;
//import androidx.annotation.Nullable;
//import androidx.appcompat.widget.AppCompatSpinner;
//import androidx.recyclerview.widget.LinearLayoutManager;
//import androidx.recyclerview.widget.RecyclerView;
//
//import com.telink.ble.mesh.TelinkMeshApplication;
//import com.telink.ble.mesh.demo.R;
//import com.telink.ble.mesh.foundation.Event;
//import com.telink.ble.mesh.foundation.EventListener;
//import com.telink.ble.mesh.foundation.event.ReliableMessageProcessEvent;
//import com.telink.ble.mesh.model.DeviceConfig;
//import com.telink.ble.mesh.model.NodeInfo;
//import com.telink.ble.mesh.model.NodeStatusChangedEvent;
//
///**
// * container for device control , group,  device settings
// * Created by kee on 2017/8/30.
// */
//public class DeviceConfigSettingActivity extends BaseActivity implements EventListener<String> {
//
//    NodeInfo deviceInfo;
//    DeviceConfig config;
//    private Handler delayHandler = new Handler();
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        if (!validateNormalStart(savedInstanceState)) {
//            return;
//        }
//        setContentView(R.layout.activity_device_config_setting);
//
//        final Intent intent = getIntent();
//        if (intent.hasExtra("meshAddress")) {
//            int address = intent.getIntExtra("meshAddress", -1);
//            deviceInfo = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(address);
//        } else {
//            toastMsg("device address null");
//            finish();
//            return;
//        }
//
//        if (deviceInfo == null) {
//            toastMsg("device info null");
//            finish();
//            return;
//        }
//
//        config = (DeviceConfig) intent.getSerializableExtra("deviceConfig");
//        if (config == null) return;
//        /*if (config.configState == ConfigState.DEFAULT_TTL) {
//
//        }*/
//
//        setTitle(config.configState.name);
//        enableBackNav(true);
//        initConfigView();
//        TelinkMeshApplication.getInstance().addEventListener(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, this);
//    }
//
//    private void initConfigView() {
//        switch (config.configState) {
//            case DEFAULT_TTL:
//                initTTLConfigView();
//                break;
//
//            case RELAY:
////                initRelayConfigView();
//                initRelayConfigView();
//                break;
//
//            case SECURE_NETWORK_BEACON:
//                initBeaconConfigView();
//                break;
//
//            case PROXY:
//                initProxyConfigView();
//                break;
//
//            case NODE_IDENTITY:
//
//                break;
//
//            case FRIEND:
//                break;
//
//            case NETWORK_TRANSMIT:
//                break;
//        }
//    }
//
//    private void initTTLConfigView() {
//        LinearLayout parentLayout = findViewById(R.id.ll_parent);
//        View view = LayoutInflater.from(this).inflate(R.layout.layout_config_ttl, parentLayout);
//        final EditText et_input = view.findViewById(R.id.et_input);
//
//        findViewById(R.id.btn_confirm).setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                String input = et_input.getText().toString().trim();
//                if (TextUtils.isEmpty(input)) {
//                    Toast.makeText(DeviceConfigSettingActivity.this, "input error", Toast.LENGTH_SHORT).show();
//                }
//            }
//        });
//    }
//
//
//    private void initRelayConfigView() {
//        LinearLayout parentLayout = findViewById(R.id.ll_parent);
//        View view = LayoutInflater.from(this).inflate(R.layout.layout_cfg_relay, parentLayout);
//        final String[] RELAY_OPTIONS = new String[]{"Enabled", "Disabled"};
//        final int[] selectedIndex = {0};
//        final AppCompatSpinner spinner = view.findViewById(R.id.sp);
//        initSpinner(spinner, RELAY_OPTIONS, new AdapterView.OnItemSelectedListener() {
//            @Override
//            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
//                selectedIndex[0] = position;
//            }
//
//            @Override
//            public void onNothingSelected(AdapterView<?> parent) {
//
//            }
//        });
//
//        findViewById(R.id.btn_confirm).setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Toast.makeText(DeviceConfigSettingActivity.this, RELAY_OPTIONS[selectedIndex[0]], Toast.LENGTH_SHORT).show();
//
//            }
//        });
//    }
//
//
//    /**
//     * show secure beacon
//     */
//    private void initBeaconConfigView() {
//        LinearLayout parentLayout = findViewById(R.id.ll_parent);
//        View view = LayoutInflater.from(this).inflate(R.layout.layout_cfg_beacon, parentLayout);
//        final String[] BEACON_OPTIONS = new String[]{"Open the Broadcasting", "Close the Broadcasting"};
//        final int[] selectedIndex = {0};
//
//        final AppCompatSpinner spinner = view.findViewById(R.id.sp);
//        initSpinner(spinner, BEACON_OPTIONS, new AdapterView.OnItemSelectedListener() {
//            @Override
//            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
//                selectedIndex[0] = position;
//            }
//
//            @Override
//            public void onNothingSelected(AdapterView<?> parent) {
//
//            }
//        });
//
//        findViewById(R.id.btn_confirm).setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Toast.makeText(DeviceConfigSettingActivity.this, BEACON_OPTIONS[selectedIndex[0]], Toast.LENGTH_SHORT).show();
//            }
//        });
//    }
//
//
//    private void initProxyConfigView() {
//        LinearLayout parentLayout = findViewById(R.id.ll_parent);
//        View view = LayoutInflater.from(this).inflate(R.layout.layout_cfg_proxy, parentLayout);
//        final String[] PROXY_OPTIONS = new String[]{"Enabled", "Disabled"};
//        final int[] selectedIndex = {0};
//        final AppCompatSpinner spinner = view.findViewById(R.id.sp);
//        initSpinner(spinner, PROXY_OPTIONS, new AdapterView.OnItemSelectedListener() {
//            @Override
//            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
//                selectedIndex[0] = position;
//            }
//
//            @Override
//            public void onNothingSelected(AdapterView<?> parent) {
//
//            }
//        });
//
//        findViewById(R.id.btn_confirm).setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Toast.makeText(DeviceConfigSettingActivity.this, PROXY_OPTIONS[selectedIndex[0]], Toast.LENGTH_SHORT).show();
//            }
//        });
//    }
//
//
//    private void initSingleSettingView(final String[] values) {
//        LinearLayout parentLayout = findViewById(R.id.ll_parent);
//        View view = LayoutInflater.from(this).inflate(R.layout.layout_cfg_proxy, parentLayout);
//        final int[] selectedIndex = {0};
//        final AppCompatSpinner spinner = view.findViewById(R.id.sp);
//        initSpinner(spinner, values, new AdapterView.OnItemSelectedListener() {
//            @Override
//            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
//                selectedIndex[0] = position;
//            }
//
//            @Override
//            public void onNothingSelected(AdapterView<?> parent) {
//
//            }
//        });
//
//        findViewById(R.id.btn_confirm).setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Toast.makeText(DeviceConfigSettingActivity.this, values[selectedIndex[0]], Toast.LENGTH_SHORT).show();
//            }
//        });
//    }
//
//
//    private void showSendWaitingDialog(String message) {
//        showWaitingDialog(message);
//    }
//
//
//    private void initSpinner(AppCompatSpinner spinner, String[] values, AdapterView.OnItemSelectedListener itemSelectedListener) {
//        spinner.setDropDownWidth(400);
//        spinner.setDropDownHorizontalOffset(100);
//        spinner.setDropDownVerticalOffset(100);
//        spinner.setSelection(0);
//        ArrayAdapter<String> spinnerAdapter = new ArrayAdapter<String>(this,
//                R.layout.layout_config_spinner_item, R.id.tv_name, values);
//        spinner.setAdapter(spinnerAdapter);
//        spinner.setOnItemSelectedListener(itemSelectedListener);
//    }
//
//    @Override
//    protected void onDestroy() {
//        super.onDestroy();
//        if (delayHandler != null) {
//            delayHandler.removeCallbacksAndMessages(null);
//        }
//        TelinkMeshApplication.getInstance().removeEventListener(this);
//    }
//
//
//    @Override
//    public void performed(Event event) {
//        if (event.getType().equals(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED)) {
//            refreshUI();
//        } else if (event.getType().equals(ReliableMessageProcessEvent.EVENT_TYPE_MSG_PROCESS_COMPLETE)) {
//            runOnUiThread(new Runnable() {
//                @Override
//                public void run() {
//                    dismissWaitingDialog();
//                }
//            });
//        }
//    }
//
//    private void refreshUI() {
//        runOnUiThread(new Runnable() {
//            @Override
//            public void run() {
//
//            }
//        });
//    }
//
//
//    static class ConfigSetView extends RecyclerView {
//
//        public ConfigSetView(@NonNull Context context) {
//            super(context);
//        }
//
//        public ConfigSetView(@NonNull Context context, @Nullable AttributeSet attrs) {
//            super(context, attrs);
//        }
//
//        public ConfigSetView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyle) {
//            super(context, attrs, defStyle);
//        }
//
//        public void init(Context context) {
//            this.setLayoutManager(new LinearLayoutManager(context));
//
//        }
//
//    }
//
//    /*
//
//    private void initRelayConfigView() {
//        LinearLayout parentLayout = findViewById(R.id.ll_parent);
//        View view = LayoutInflater.from(this).inflate(R.layout.layout_config_relay, parentLayout);
//        final String[] RELAY_OPTIONS = new String[]{"Enabled", "Disabled"};
//        final int[] selectedIndex = {0};
//        final EditText et_input = view.findViewById(R.id.et_input);
//
//        et_input.setText(RELAY_OPTIONS[selectedIndex[0]]);
//        et_input.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                showItemSelectDialog("Select Relay Value", RELAY_OPTIONS, new DialogInterface.OnClickListener() {
//                    @Override
//                    public void onClick(DialogInterface dialog, int which) {
//                        selectedIndex[0] = which;
//                        et_input.setText(RELAY_OPTIONS[selectedIndex[0]]);
//                    }
//                });
//            }
//        });
//
//        findViewById(R.id.btn_confirm).setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Toast.makeText(DeviceConfigSettingActivity.this, RELAY_OPTIONS[selectedIndex[0]], Toast.LENGTH_SHORT).show();
//
//            }
//        });
//    }
//     */
//
//}
