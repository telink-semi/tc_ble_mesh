///********************************************************************************************************
// * @file RemoteControlSettingActivity.java
// *
// * @brief for TLSR chips
// *
// * @author telink
// * @date Sep. 30, 2017
// *
// * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
// *
// *          Licensed under the Apache License, Version 2.0 (the "License");
// *          you may not use this file except in compliance with the License.
// *          You may obtain a copy of the License at
// *
// *              http://www.apache.org/licenses/LICENSE-2.0
// *
// *          Unless required by applicable law or agreed to in writing, software
// *          distributed under the License is distributed on an "AS IS" BASIS,
// *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// *          See the License for the specific language governing permissions and
// *          limitations under the License.
// *******************************************************************************************************/
//package com.telink.ble.mesh.ui;
//
//import android.content.Intent;
//import android.os.Bundle;
//import android.os.Handler;
//import android.view.View;
//import android.widget.TextView;
//import android.widget.Toast;
//
//import androidx.fragment.app.Fragment;
//import androidx.fragment.app.FragmentPagerAdapter;
//import androidx.viewpager.widget.ViewPager;
//
//import com.google.android.material.tabs.TabLayout;
//import com.telink.ble.mesh.TelinkMeshApplication;
//import com.telink.ble.mesh.core.message.config.ModelPublicationStatusMessage;
//import com.telink.ble.mesh.demo.R;
//import com.telink.ble.mesh.entity.ConnectionFilter;
//import com.telink.ble.mesh.entity.MeshAdvFilter;
//import com.telink.ble.mesh.foundation.Event;
//import com.telink.ble.mesh.foundation.EventListener;
//import com.telink.ble.mesh.foundation.MeshService;
//import com.telink.ble.mesh.foundation.event.GattConnectionEvent;
//import com.telink.ble.mesh.foundation.event.MeshEvent;
//import com.telink.ble.mesh.foundation.parameter.GattConnectionParameters;
//import com.telink.ble.mesh.model.NodeInfo;
//import com.telink.ble.mesh.ui.fragment.DeviceSettingFragment;
//import com.telink.ble.mesh.ui.fragment.RemoteControlFragment;
//
///**
// * remote control device settings
// */
//public class RemoteControlSettingActivity extends BaseActivity implements EventListener<String> {
//
//    NodeInfo deviceInfo;
//    private Handler delayHandler = new Handler();
//
//    // show online state
//    private TextView tv_ol_st;
//    private String[] titles = {"Control", "Settings"};
//    private Fragment[] tabFragments = new Fragment[2];
//
//    private static final int STATE_CONNECTED = 1;
//
//    private static final int STATE_CONNECTING = 2;
//
//    private static final int STATE_DISCONNECTED = 3;
//    private int connSt = STATE_DISCONNECTED;
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        if (!validateNormalStart(savedInstanceState)) {
//            return;
//        }
//        setContentView(R.layout.activity_rmt_ctrl_setting);
//        setTitle("Remote Control");
//        enableBackNav(true);
//        final Intent intent = getIntent();
//        if (intent.hasExtra("deviceAddress")) {
//            int address = intent.getIntExtra("deviceAddress", -1);
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
//        tv_ol_st = findViewById(R.id.tv_ol_st);
//        tv_ol_st.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                if (connSt == STATE_DISCONNECTED) {
//                    connectRemoteDevice();
//                }
//            }
//        });
////        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);
//        TelinkMeshApplication.getInstance().addEventListener(GattConnectionEvent.EVENT_TYPE_CONNECT_FAIL, this);
//        TelinkMeshApplication.getInstance().addEventListener(GattConnectionEvent.EVENT_TYPE_CONNECT_SUCCESS, this);
//        TelinkMeshApplication.getInstance().addEventListener(ModelPublicationStatusMessage.class.getName(), this);
//
//        initTab();
//        refreshUI();
//        connectRemoteDevice();
//    }
//
//    @Override
//    public void finish() {
//        super.finish();
//        MeshService.getInstance().idle(true);
//    }
//
//    private void connectRemoteDevice() {
//        MeshService.getInstance().idle(true);
//        connSt = STATE_CONNECTING;
//        refreshUI();
//        ConnectionFilter connectionFilter = new ConnectionFilter(ConnectionFilter.TYPE_MESH_ADDRESS, deviceInfo.meshAddress);
//
//        // remote device only use node identity
//        connectionFilter.advFilter = MeshAdvFilter.NODE_ID_ONLY;
//        MeshService.getInstance().startGattConnection(new GattConnectionParameters(connectionFilter));
//    }
//
//    private void initTab() {
//        Bundle bundle = new Bundle();
//        bundle.putInt("address", deviceInfo.meshAddress);
//
//        Fragment controlFragment = new RemoteControlFragment();
//        controlFragment.setArguments(bundle);
//
//        Fragment settingFragment = new DeviceSettingFragment();
//        settingFragment.setArguments(bundle);
//        tabFragments[0] = controlFragment;
//        tabFragments[1] = settingFragment;
//
//
//        TabLayout tabLayout = findViewById(R.id.tab_device_setting);
//        ViewPager viewPager = findViewById(R.id.vp_setting);
//        viewPager.setAdapter(new FragmentPagerAdapter(getSupportFragmentManager()) {
//            @Override
//            public Fragment getItem(int position) {
//                return tabFragments[position];
//            }
//
//            @Override
//            public int getCount() {
//                return tabFragments.length;
//            }
//
//            @Override
//            public CharSequence getPageTitle(int position) {
//                return titles[position];
//            }
//        });
//        tabLayout.setupWithViewPager(viewPager);
//    }
//
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
//        if (event.getType().equals(GattConnectionEvent.EVENT_TYPE_CONNECT_FAIL) ||
//                event.getType().equals(MeshEvent.EVENT_TYPE_DISCONNECTED)) {
//            connSt = STATE_DISCONNECTED;
//            refreshUI();
//        } else if (event.getType().equals(GattConnectionEvent.EVENT_TYPE_CONNECT_SUCCESS)) {
//            connSt = STATE_CONNECTED;
//            refreshUI();
//        } else if (event.getType().equals(ModelPublicationStatusMessage.class.getName())) {
//            runOnUiThread(new Runnable() {
//                @Override
//                public void run() {
//                    Toast.makeText(RemoteControlSettingActivity.this, "pub set complete", Toast.LENGTH_SHORT).show();
//                }
//            });
//        }
//    }
//
//    private void refreshUI() {
//        runOnUiThread(new Runnable() {
//            @Override
//            public void run() {
//                tv_ol_st.setTextColor(getStateColor());
//                tv_ol_st.setText(getStateText());
////                tv_ol_st.setVisibility(deviceInfo.isOffline() ? View.VISIBLE : View.GONE);
//            }
//        });
//    }
//
//    private String getStateText() {
//        switch (connSt) {
//            case STATE_CONNECTED:
//                return "device connected";
//            case STATE_DISCONNECTED:
//                return "device disconnected";
//            case STATE_CONNECTING:
//                return "device connecting...";
//        }
//        return "";
//    }
//
//    private int getStateColor() {
//        switch (connSt) {
//            case STATE_CONNECTED:
//                return getResources().getColor(R.color.colorPrimary);
//            case STATE_DISCONNECTED:
//                return getResources().getColor(R.color.warn_click);
//            case STATE_CONNECTING:
//                return getResources().getColor(R.color.grey);
//        }
//        return 0;
//    }
//}
