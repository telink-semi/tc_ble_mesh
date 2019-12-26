/********************************************************************************************************
 * @file DeviceFragment.java
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
package com.telink.sig.mesh.demo.ui.fragment;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.telink.sig.mesh.demo.AppSettings;
import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.SharedPreferenceHelper;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.ui.CmdActivity;
import com.telink.sig.mesh.demo.ui.DeviceProvisionActivity;
import com.telink.sig.mesh.demo.ui.DeviceSettingActivity;
import com.telink.sig.mesh.demo.ui.FastProvisionActivity;
import com.telink.sig.mesh.demo.ui.KeyBindActivity;
import com.telink.sig.mesh.demo.ui.LogActivity;
import com.telink.sig.mesh.demo.ui.MainActivity;
import com.telink.sig.mesh.demo.ui.MeshTestActivity;
import com.telink.sig.mesh.demo.ui.RemoteProvisionActivity;
import com.telink.sig.mesh.demo.ui.adapter.BaseRecyclerViewAdapter;
import com.telink.sig.mesh.demo.ui.adapter.OnlineDeviceListAdapter;
import com.telink.sig.mesh.event.CommandEvent;
import com.telink.sig.mesh.event.Event;
import com.telink.sig.mesh.event.EventListener;
import com.telink.sig.mesh.event.MeshEvent;
import com.telink.sig.mesh.event.NotificationEvent;
import com.telink.sig.mesh.event.OnlineStatusEvent;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.model.DeviceBindState;
import com.telink.sig.mesh.model.DeviceInfo;

import java.util.List;

/**
 * 设备控制fragment
 * Created by kee on 2017/8/18.
 */

public class DeviceFragment extends BaseFragment implements View.OnClickListener, EventListener<String> {

    private OnlineDeviceListAdapter mAdapter;
    private List<DeviceInfo> mDevices;
    private Handler mCycleHandler = new Handler();

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_device, null);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        setTitle(view, "Device");
        Toolbar toolbar = view.findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.device_tab);
        toolbar.setNavigationIcon(R.drawable.ic_refresh);

        toolbar.setNavigationOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                boolean cmdSent;
                if (AppSettings.ONLINE_STATUS_ENABLE) {
                    cmdSent = MeshService.getInstance().getOnlineStatus();
                } else {
                    int rspMax = TelinkMeshApplication.getInstance().getMesh().getOnlineCountInAll();
                    cmdSent = (MeshService.getInstance().getOnOff(0xFFFF, rspMax, null));
                }
                if (cmdSent) {
                    for (DeviceInfo deviceInfo : mDevices) {
                        deviceInfo.setOnOff(-1);
                    }
                    mAdapter.notifyDataSetChanged();
                }

            }
        });

        toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                if (item.getItemId() == R.id.item_add) {
//                    startActivity(new Intent(getActivity(), DeviceProvisionActivity.class));
                    if (SharedPreferenceHelper.isRemoteProvisionEnable(getActivity())) {
                        startActivity(new Intent(getActivity(), RemoteProvisionActivity.class));
                    } else if (SharedPreferenceHelper.isFastProvisionEnable(getActivity())) {
                        startActivity(new Intent(getActivity(), FastProvisionActivity.class));
                    } else {
                        startActivity(new Intent(getActivity(), DeviceProvisionActivity.class));
                    }
                }
                return false;
            }
        });
        view.findViewById(R.id.tv_provision).setOnClickListener(this);
        view.findViewById(R.id.tv_on).setOnClickListener(this);
        view.findViewById(R.id.tv_off).setOnClickListener(this);
        view.findViewById(R.id.tv_cmd).setOnClickListener(this);
        view.findViewById(R.id.tv_log).setOnClickListener(this);
        view.findViewById(R.id.tv_mesh_test).setOnClickListener(this);
        view.findViewById(R.id.tv_cycle).setOnClickListener(this);

        RecyclerView gv_devices = view.findViewById(R.id.rv_online_devices);
        mDevices = TelinkMeshApplication.getInstance().getMesh().devices;
        mAdapter = new OnlineDeviceListAdapter(getActivity(), mDevices);

        gv_devices.setLayoutManager(new GridLayoutManager(getActivity(), 4));
        gv_devices.setAdapter(mAdapter);

        mAdapter.setOnItemClickListener(new BaseRecyclerViewAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(int position) {
                if (mDevices.get(position).getOnOff() == -1) return;

                byte onOff = 0;
                if (mDevices.get(position).getOnOff() == 0) {
                    onOff = 1;
                }
                MeshService.getInstance().setOnOff(mDevices.get(position).meshAddress, onOff, !AppSettings.ONLINE_STATUS_ENABLE, !AppSettings.ONLINE_STATUS_ENABLE ? 1 : 0, 0, (byte) 0, null);
            }
        });

        mAdapter.setOnItemLongClickListener(new BaseRecyclerViewAdapter.OnItemLongClickListener() {
            @Override
            public boolean onLongClick(int position) {
                DeviceInfo deviceInfo = TelinkMeshApplication.getInstance().getMesh().getDeviceByMeshAddress(mDevices.get(position).meshAddress);
                if (deviceInfo == null) {
                    toastMsg("device info null!");
                    return false;
                }
                Intent intent;
                if (deviceInfo.bindState == DeviceBindState.BOUND) {
                    /*if (deviceInfo.nodeInfo.cpsData.lowPowerSupport()) {
                        intent = new Intent(getActivity(), LpnSettingActivity.class);
                    } else {
                        intent = new Intent(getActivity(), DeviceSettingActivity.class);
                    }*/
                    intent = new Intent(getActivity(), DeviceSettingActivity.class);
                } else {
                    intent = new Intent(getActivity(), KeyBindActivity.class);
                }

                intent.putExtra("deviceAddress", deviceInfo.meshAddress);
                startActivity(intent);
//                MeshService.getInstance().setHeartbeatPublication(deviceInfo.meshAddress, false, "HB_PUB_SET");
                return false;
            }

        });

        /*loadingDialog = new ProgressDialog(getActivity());
        loadingDialog.setMessage("loading...");
        loadingDialog.show();
*/
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_DEVICE_ON_OFF_STATUS, this);
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_CTL_STATUS_NOTIFY, this);
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_LIGHTNESS_STATUS_NOTIFY, this);
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_DEVICE_LEVEL_STATUS, this);
        TelinkMeshApplication.getInstance().addEventListener(OnlineStatusEvent.ONLINE_STATUS_NOTIFY, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DEVICE_OFFLINE, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_MESH_RESET, this);

        TelinkMeshApplication.getInstance().addEventListener(CommandEvent.EVENT_TYPE_CMD_COMPLETE, this);
    }

    @Override
    public void onResume() {
        super.onResume();
//        mDevices = TelinkMeshApplication.getInstance().getMesh().devices;
//        mAdapter.notifyDataSetChanged();
        mAdapter.resetDevices(mDevices);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
        mCycleHandler.removeCallbacksAndMessages(null);
    }

    private void toastMsg(String s) {
        ((MainActivity) getActivity()).toastMsg(s);
    }

    private void refreshUI() {
        mDevices = TelinkMeshApplication.getInstance().getMesh().devices;
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mAdapter.resetDevices(mDevices);
            }
        });
    }

    int index = 0;
    boolean cycleTestStarted = false;
    private Runnable cycleTask = new Runnable() {
        @Override
        public void run() {
//            MeshService.getInstance().cmdOnOff(index % 2 == 0 ? 3 : 4, (byte) 1, (byte) ((index / 2) % 2), 1);
            MeshService.getInstance().setOnOff(index % 2 == 0 ? 3 : 4, (byte) ((index / 2) % 2), true, 1, 0, (byte) 0, null);
            index++;
            mCycleHandler.postDelayed(this, 2 * 1000);
        }
    };

    private long startTime;
    private static final String TAG_ALL_ON = "ALL_ON";
    private static final String TAG_ALL_OFF = "ALL_OFF";

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tv_cycle:
                mCycleHandler.removeCallbacks(cycleTask);

                cycleTestStarted = !cycleTestStarted;
                if (cycleTestStarted) {
                    mCycleHandler.post(cycleTask);
                }
                ((TextView) v).setText(cycleTestStarted ? "Cycle Stop" : "Cycle Start");
                break;

            case R.id.tv_on:
                startTime = System.currentTimeMillis();
                int rspMax = TelinkMeshApplication.getInstance().getMesh().getOnlineCountInAll();
//                MeshService.getInstance().cmdOnOff(0xFFFF, (byte) 100, (byte) 1, 1);
                boolean onCmdRe = MeshService.getInstance().setOnOff(0xFFFF, (byte) 1, !AppSettings.ONLINE_STATUS_ENABLE, !AppSettings.ONLINE_STATUS_ENABLE ? rspMax : 0, 0, (byte) 0, TAG_ALL_ON);
                if (onCmdRe) {
                    saveLog("All On start!");
                } else {
                    saveLog("All On fail");
                }
                break;
            case R.id.tv_off:
                startTime = System.currentTimeMillis();
                rspMax = TelinkMeshApplication.getInstance().getMesh().getOnlineCountInAll();
//                MeshService.getInstance().cmdOnOff(0xFFFF, (byte) 100, (byte) 0, 1);
                boolean offCmdRe = MeshService.getInstance().setOnOff(0xFFFF, (byte) 0, !AppSettings.ONLINE_STATUS_ENABLE, !AppSettings.ONLINE_STATUS_ENABLE ? rspMax : 0, 0, (byte) 0, TAG_ALL_OFF);
                if (offCmdRe) {
                    saveLog("All Off start!");
                } else {
                    saveLog("All Off fail");
                }
                break;

            case R.id.tv_cmd:
//                byte[] config = TelinkMeshApplication.getInstance().getMeshLib().getConfigInfo();
//                TelinkLog.d("config: " + Arrays.bytesToHexString(config, ":"));
                startActivity(new Intent(getActivity(), CmdActivity.class));
                break;

            case R.id.tv_log:
//                throw new RuntimeException("crash handler test");
                startActivity(new Intent(getActivity(), LogActivity.class));
                break;

            case R.id.tv_mesh_test:
                startActivity(new Intent(getActivity(), MeshTestActivity.class));
                break;
        }
    }

    private void saveLog(String action) {
        TelinkMeshApplication.getInstance().saveLog(" --test-- " + action);
    }


    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(MeshEvent.EVENT_TYPE_DISCONNECTED) || event.getType().equals(MeshEvent.EVENT_TYPE_DEVICE_OFFLINE)
                || event.getType().equals(MeshEvent.EVENT_TYPE_MESH_RESET)
                || event.getType().equals(NotificationEvent.EVENT_TYPE_DEVICE_LEVEL_STATUS)) {
            refreshUI();
        } else if (event.getType().equals(CommandEvent.EVENT_TYPE_CMD_COMPLETE)) {
            CommandEvent commandEvent = (CommandEvent) event;
            long during = System.currentTimeMillis() - startTime;
            if (TAG_ALL_ON.equals(commandEvent.getMeshCommand().tag)) {
                saveLog("All On during:" + during + "ms");
            } else if (TAG_ALL_OFF.equals(commandEvent.getMeshCommand().tag)) {
                saveLog("All Off during:" + during + "ms");
            }
        } else {
            if (event instanceof NotificationEvent) {
                if (((NotificationEvent) event).isStatusChanged()) {
                    refreshUI();
                }
            } else if (event instanceof OnlineStatusEvent) {
                refreshUI();
            }
        }
    }


}
