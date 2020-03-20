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
 * Device Setting : grouping ,, lum setting ,, OTA ,, kick out
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
