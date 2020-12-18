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

package com.telink.ble.mesh.ui.fragment;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.generic.OnOffGetMessage;
import com.telink.ble.mesh.core.message.generic.OnOffSetMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.AutoConnectEvent;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.ui.CmdActivity;
import com.telink.ble.mesh.ui.DeviceAutoProvisionActivity;
import com.telink.ble.mesh.ui.DeviceProvisionActivity;
import com.telink.ble.mesh.ui.DeviceSettingActivity;
import com.telink.ble.mesh.ui.FastProvisionActivity;
import com.telink.ble.mesh.ui.KeyBindActivity;
import com.telink.ble.mesh.ui.LogActivity;
import com.telink.ble.mesh.ui.MainActivity;
import com.telink.ble.mesh.ui.RemoteProvisionActivity;
import com.telink.ble.mesh.ui.adapter.BaseRecyclerViewAdapter;
import com.telink.ble.mesh.ui.adapter.OnlineDeviceListAdapter;
import com.telink.ble.mesh.ui.test.ConnectionTestActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.List;

import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * devices fragment
 * Created by kee on 2017/8/18.
 */

public class DeviceFragment extends BaseFragment implements View.OnClickListener, EventListener<String> {

    private OnlineDeviceListAdapter mAdapter;
    private List<NodeInfo> mDevices;
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
                boolean cmdSent = false;
                if (AppSettings.ONLINE_STATUS_ENABLE) {
                    cmdSent = MeshService.getInstance().getOnlineStatus();
                } else {
//                    int rspMax = TelinkMeshApplication.getInstance().getMeshInfo().getOnlineCountInAll();
                    int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
                    OnOffGetMessage message = OnOffGetMessage.getSimple(0xFFFF, appKeyIndex, /*rspMax*/ 0);
                    cmdSent = MeshService.getInstance().sendMeshMessage(message);
                }
                if (cmdSent) {
                    for (NodeInfo deviceInfo : mDevices) {
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
                    } else if (SharedPreferenceHelper.isAutoPvEnable(getActivity())) {
                        startActivity(new Intent(getActivity(), DeviceAutoProvisionActivity.class));
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
        view.findViewById(R.id.tv_cycle).setOnClickListener(this);
        view.findViewById(R.id.btn_test).setOnClickListener(this);

        RecyclerView gv_devices = view.findViewById(R.id.rv_online_devices);
        mDevices = TelinkMeshApplication.getInstance().getMeshInfo().nodes;
        mAdapter = new OnlineDeviceListAdapter(getActivity(), mDevices);

        gv_devices.setLayoutManager(new GridLayoutManager(getActivity(), 4));
        gv_devices.setAdapter(mAdapter);

        mAdapter.setOnItemClickListener(new BaseRecyclerViewAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(int position) {
                if (mDevices.get(position).getOnOff() == -1) return;

                int onOff = 0;
                if (mDevices.get(position).getOnOff() == 0) {
                    onOff = 1;
                }

                int address = mDevices.get(position).meshAddress;
                int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
                OnOffSetMessage onOffSetMessage = OnOffSetMessage.getSimple(address, appKeyIndex, onOff, !AppSettings.ONLINE_STATUS_ENABLE, !AppSettings.ONLINE_STATUS_ENABLE ? 1 : 0);
                MeshService.getInstance().sendMeshMessage(onOffSetMessage);
//                MeshService.getInstance().setOnOff(mDevices.get(position).meshAddress, onOff, !AppSettings.ONLINE_STATUS_ENABLE, !AppSettings.ONLINE_STATUS_ENABLE ? 1 : 0, 0, (byte) 0, null);
            }
        });

        mAdapter.setOnItemLongClickListener(new BaseRecyclerViewAdapter.OnItemLongClickListener() {
            @Override
            public boolean onLongClick(int position) {
                NodeInfo deviceInfo = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(mDevices.get(position).meshAddress);
                if (deviceInfo == null) {
                    toastMsg("device info null!");
                    return false;
                }
                MeshLogger.d("deviceKey: " + (Arrays.bytesToHexString(deviceInfo.deviceKey)));
                Intent intent;
                if (deviceInfo.bound) {
                    intent = new Intent(getActivity(), DeviceSettingActivity.class);
                } else {
                    intent = new Intent(getActivity(), KeyBindActivity.class);
                }
                intent.putExtra("deviceAddress", deviceInfo.meshAddress);
                startActivity(intent);
                return false;
            }

        });

        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);
        TelinkMeshApplication.getInstance().addEventListener(AutoConnectEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_MESH_RESET, this);
        TelinkMeshApplication.getInstance().addEventListener(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, this);
    }

    @Override
    public void onResume() {
        super.onResume();
//        mDevices = TelinkMeshApplication.getInstance().getMeshInfo().devices;
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
        mDevices = TelinkMeshApplication.getInstance().getMeshInfo().nodes;
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
            int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
            OnOffSetMessage onOffSetMessage = OnOffSetMessage.getSimple(0xFFFF, appKeyIndex,
                    index % 2, true, 0);
            MeshService.getInstance().sendMeshMessage(onOffSetMessage);
            index++;
            mCycleHandler.postDelayed(this, 2 * 1000);
        }
    };

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
//                startTime = System.currentTimeMillis();
                int rspMax = TelinkMeshApplication.getInstance().getMeshInfo().getOnlineCountInAll();

                int address = 0xFFFF;
                int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
                OnOffSetMessage onOffSetMessage = OnOffSetMessage.getSimple(address, appKeyIndex, 1, !AppSettings.ONLINE_STATUS_ENABLE, !AppSettings.ONLINE_STATUS_ENABLE ? rspMax : 0);
                MeshService.getInstance().sendMeshMessage(onOffSetMessage);
                break;
            case R.id.tv_off:
//                startTime = System.currentTimeMillis();
                rspMax = TelinkMeshApplication.getInstance().getMeshInfo().getOnlineCountInAll();

                address = 0xFFFF;
                appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
                onOffSetMessage = OnOffSetMessage.getSimple(address, appKeyIndex, 0, !AppSettings.ONLINE_STATUS_ENABLE, !AppSettings.ONLINE_STATUS_ENABLE ? rspMax : 0);
                MeshService.getInstance().sendMeshMessage(onOffSetMessage);

                break;

            case R.id.tv_cmd:
//                byte[] config = TelinkMeshApplication.getInstance().getMeshLib().getConfigInfo();
//                MeshLogger.log("config: " + Arrays.bytesToHexString(config, ":"));
                startActivity(new Intent(getActivity(), CmdActivity.class));
                break;

            case R.id.tv_log:
                startActivity(new Intent(getActivity(), LogActivity.class));
                break;

            case R.id.btn_test:
                startActivity(new Intent(getActivity(), ConnectionTestActivity.class));
//                startActivity(new Intent(getActivity(), OnOffTestActivity.class));
                break;
        }
    }


    @Override
    public void performed(Event<String> event) {
        String eventType = event.getType();
        if (eventType.equals(MeshEvent.EVENT_TYPE_DISCONNECTED)
                || eventType.equals(MeshEvent.EVENT_TYPE_MESH_RESET)
                || eventType.equals(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED)
                || eventType.equals(AutoConnectEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN)) {
            refreshUI();
        }
    }


}
