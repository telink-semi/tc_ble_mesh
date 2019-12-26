/********************************************************************************************************
 * @file GroupSettingActivity.java
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
package com.telink.sig.mesh.demo.ui;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import com.telink.sig.mesh.demo.AppSettings;
import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.ui.adapter.BaseRecyclerViewAdapter;
import com.telink.sig.mesh.demo.ui.adapter.DevicesInGroupAdapter;
import com.telink.sig.mesh.demo.ui.adapter.OnlineDeviceListAdapter;
import com.telink.sig.mesh.event.Event;
import com.telink.sig.mesh.event.EventListener;
import com.telink.sig.mesh.event.MeshEvent;
import com.telink.sig.mesh.event.NotificationEvent;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.Group;
import com.telink.sig.mesh.util.TelinkLog;

import java.util.ArrayList;
import java.util.List;

/**
 * Device Setting : grouping ,, lum setting ,, OTA ,, kick out
 * Created by kee on 2017/8/30.
 */

public class GroupSettingActivity extends BaseActivity implements EventListener<String> {

    private OnlineDeviceListAdapter mAdapter;

    private SeekBar lum, temp;
    private TextView tv_lum, tv_temp;
    private RecyclerView rv_groups;
    private Group group;

    private SeekBar.OnSeekBarChangeListener onProgressChangeListener = new SeekBar.OnSeekBarChangeListener() {

        private long preTime;
        private static final int DELAY_TIME = 320;


        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            onProgressUpdate(seekBar, progress, false);
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {

        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            onProgressUpdate(seekBar, seekBar.getProgress(), true);
        }

        private void onProgressUpdate(SeekBar seekBar, int progress, boolean immediate) {
            long currentTime = System.currentTimeMillis();
            if ((currentTime - this.preTime) >= DELAY_TIME || immediate) {
                this.preTime = currentTime;
                if (seekBar == lum) {
//                    MeshService.getInstance().cmdSetLum(group.address, (byte) (progress + 1));
                    MeshService.getInstance().setLum(group.address, progress, false, 0, 0, (byte) 0, null);
                    tv_lum.setText(getString(R.string.lum_progress, progress + 1, Integer.toHexString(group.address)));
                } else if (seekBar == temp) {
//                    MeshService.getInstance().cmdSetTemp(group.address, (byte) progress);
                    MeshService.getInstance().setTemperature100(group.address, progress, false, 0, 0, (byte) 0, null);
                    tv_temp.setText(getString(R.string.temp_progress, progress, Integer.toHexString(group.address)));
                }
            } else {
                TelinkLog.w("CMD reject: " + progress);
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_group_setting);

        final Intent intent = getIntent();
        if (intent.hasExtra("group")) {
            group = (Group) intent.getSerializableExtra("group");
        } else {
            toastMsg("group null");
            finish();
            return;
        }

        TextView tv_group_name = findViewById(R.id.tv_group_name);
        tv_group_name.setText(group.name + ":");
        lum = findViewById(R.id.sb_brightness);
        temp = findViewById(R.id.sb_temp);

        rv_groups = findViewById(R.id.rv_device);

        setTitle("Group Setting");
        enableBackNav(true);
        final List<DeviceInfo> innerDevices = getDevicesInGroup();
        mAdapter = new OnlineDeviceListAdapter(this, innerDevices);
        mAdapter.setOnItemClickListener(new BaseRecyclerViewAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(int position) {
                if (innerDevices.get(position).getOnOff() == -1) return;

                byte onOff = 0;
                if (innerDevices.get(position).getOnOff() == 0) {
                    onOff = 1;
                }
                MeshService.getInstance().setOnOff(innerDevices.get(position).meshAddress, onOff, !AppSettings.ONLINE_STATUS_ENABLE, !AppSettings.ONLINE_STATUS_ENABLE ? 1 : 0, 0, (byte) 0, null);

//                MeshService.getInstance().cmdOnOff(innerDevices.get(position).meshAddress, (byte) 1, onOff, 1);
            }
        });

        rv_groups.setLayoutManager(new GridLayoutManager(this, 3));
        rv_groups.setAdapter(mAdapter);

        findViewById(R.id.tv_color).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent colorIntent = new Intent(GroupSettingActivity.this, ColorPanelActivity.class);
                colorIntent.putExtra("address", group.address);
                startActivity(colorIntent);
            }
        });
        lum.setEnabled(innerDevices.size() != 0);
        temp.setEnabled(innerDevices.size() != 0);

        tv_lum = (TextView) findViewById(R.id.tv_lum);
        tv_temp = (TextView) findViewById(R.id.tv_temp);
        tv_lum.setText(getString(R.string.lum_progress, 10, Integer.toHexString(group.address)));
        tv_temp.setText(getString(R.string.temp_progress, 10, Integer.toHexString(group.address)));

        lum.setOnSeekBarChangeListener(this.onProgressChangeListener);

        temp.setOnSeekBarChangeListener(this.onProgressChangeListener);

        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_DEVICE_ON_OFF_STATUS, this);
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_CTL_STATUS_NOTIFY, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);
    }

    private List<DeviceInfo> getDevicesInGroup() {

        List<DeviceInfo> localDevices = TelinkMeshApplication.getInstance().getMesh().devices;
        List<DeviceInfo> innerDevices = new ArrayList<>();
        outer:
        for (DeviceInfo device : localDevices) {
            if (device.subList != null) {
                for (int groupAdr : device.subList) {
                    if (groupAdr == group.address) {
                        innerDevices.add(device);
                        continue outer;
                    }
                }
            }
        }
        return innerDevices;
    }

    private void refreshUI() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mAdapter.notifyDataSetChanged();
            }
        });
    }

    @Override
    public void performed(Event<String> event) {
        super.performed(event);
        if (event.getType().equals(NotificationEvent.EVENT_TYPE_DEVICE_ON_OFF_STATUS)) {
            refreshUI();
        } else if (event.getType().equals(MeshEvent.EVENT_TYPE_DISCONNECTED)) {
            refreshUI();
        } else if (event.getType().equals(NotificationEvent.EVENT_TYPE_CTL_STATUS_NOTIFY)) {
            refreshUI();
        }
    }
}
