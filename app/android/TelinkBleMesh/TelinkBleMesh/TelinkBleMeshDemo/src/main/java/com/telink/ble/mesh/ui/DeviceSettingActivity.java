/********************************************************************************************************
 * @file DeviceSettingActivity.java
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

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;

import com.google.android.material.tabs.TabLayout;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.ui.fragment.DeviceControlFragment;
import com.telink.ble.mesh.ui.fragment.DeviceGroupFragment;
import com.telink.ble.mesh.ui.fragment.DeviceSettingFragment;

import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentPagerAdapter;
import androidx.viewpager.widget.ViewPager;

/**
 * container for device control , group,  device settings
 * Created by kee on 2017/8/30.
 */
public class DeviceSettingActivity extends BaseActivity implements EventListener<String> {

    NodeInfo deviceInfo;
    private Handler delayHandler = new Handler();
    private View ll_offline;
    private String[] titles = {"Control", "Group", "Settings"};
    private Fragment[] tabFragments = new Fragment[3];


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_device_setting);
        setTitle("Device Setting");
        enableBackNav(true);
        final Intent intent = getIntent();
        if (intent.hasExtra("deviceAddress")) {
            int address = intent.getIntExtra("deviceAddress", -1);
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

        ll_offline = findViewById(R.id.ll_offline);

        // todo mesh interface
        TelinkMeshApplication.getInstance().addEventListener(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, this);
//        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_GROUP_INFO, this);
//        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_CTL_STATUS_NOTIFY, this);
//        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);
//        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_SUB_OP_CONFIRM, this);
//        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_KICK_OUT_CONFIRM, this);

        initTab();
        refreshUI();
    }

    private void initTab() {
        Bundle bundle = new Bundle();
        bundle.putInt("address", deviceInfo.meshAddress);

        Fragment controlFragment = new DeviceControlFragment();
        controlFragment.setArguments(bundle);

        Fragment groupFragment = new DeviceGroupFragment();
        groupFragment.setArguments(bundle);

        Fragment settingFragment = new DeviceSettingFragment();
        settingFragment.setArguments(bundle);
        tabFragments[0] = controlFragment;
        tabFragments[1] = groupFragment;
        tabFragments[2] = settingFragment;


        TabLayout tabLayout = findViewById(R.id.tab_device_setting);
        ViewPager viewPager = findViewById(R.id.vp_setting);
        viewPager.setAdapter(new FragmentPagerAdapter(getSupportFragmentManager()) {
            @Override
            public Fragment getItem(int position) {
                return tabFragments[position];
            }

            @Override
            public int getCount() {
                return tabFragments.length;
            }

            @Override
            public CharSequence getPageTitle(int position) {
                return titles[position];
            }
        });
        tabLayout.setupWithViewPager(viewPager);
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
        }
    }

    private void refreshUI() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ll_offline.setVisibility(deviceInfo.getOnOff() == -1 ? View.VISIBLE : View.GONE);
            }
        });
    }

}
