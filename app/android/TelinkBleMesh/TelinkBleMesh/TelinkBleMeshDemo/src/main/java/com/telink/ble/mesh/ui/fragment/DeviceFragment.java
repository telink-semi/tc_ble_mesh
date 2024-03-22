/********************************************************************************************************
 * @file DeviceFragment.java
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

package com.telink.ble.mesh.ui.fragment;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.TextView;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.NodeSortType;
import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.config.CompositionDataStatusMessage;
import com.telink.ble.mesh.core.message.generic.OnOffGetMessage;
import com.telink.ble.mesh.core.message.generic.OnOffSetMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.AutoConnectEvent;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.model.OnlineState;
import com.telink.ble.mesh.ui.BaseActivity;
import com.telink.ble.mesh.ui.CmdActivity;
import com.telink.ble.mesh.ui.DeviceAutoProvisionActivity;
import com.telink.ble.mesh.ui.DeviceProvisionActivity;
import com.telink.ble.mesh.ui.DeviceSettingActivity;
import com.telink.ble.mesh.ui.FastProvisionActivity;
import com.telink.ble.mesh.ui.KeyBindActivity;
import com.telink.ble.mesh.ui.LogActivity;
import com.telink.ble.mesh.ui.MainActivity;
import com.telink.ble.mesh.ui.RemoteControlSettingActivity;
import com.telink.ble.mesh.ui.RemoteProvisionActivity;
import com.telink.ble.mesh.ui.adapter.OnlineDeviceListAdapter;
import com.telink.ble.mesh.ui.test.OnOffTestActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.Collections;
import java.util.List;
import java.util.Objects;

/**
 * devices fragment
 * show devices in mesh
 * refresh device online state when received notification
 * Created by kee on 2017/8/18.
 */
public class DeviceFragment extends BaseFragment implements View.OnClickListener, EventListener<String> {

    private OnlineDeviceListAdapter mAdapter;
    private List<NodeInfo> mDevices;
    private Handler mCycleHandler = new Handler();
    private MenuItem sortItem;

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
        sortItem = toolbar.getMenu().findItem(R.id.item_sort);
        resetSortIcon();
        toolbar.setNavigationIcon(R.drawable.ic_refresh);

        toolbar.setNavigationOnClickListener(v -> {
            boolean cmdSent;
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
                    deviceInfo.setOnlineState(OnlineState.OFFLINE);
                }
                mAdapter.notifyDataSetChanged();
            }
        });

        toolbar.setOnMenuItemClickListener(item -> {

            if (item.getItemId() == R.id.item_add) {
//                    startActivity(new Intent(getActivity(), DeviceProvisionActivity.class));
                if (TelinkMeshApplication.getInstance().getMeshInfo().ivIndex == MeshInfo.UNINITIALIZED_IVI) {
                    showIvWarningDialog();
//                toastMsg("");
                    return false;
                }
                if (SharedPreferenceHelper.isRemoteProvisionEnable(getActivity())) {
                    startActivity(new Intent(getActivity(), RemoteProvisionActivity.class));
                } else if (SharedPreferenceHelper.isFastProvisionEnable(getActivity())) {
                    startActivity(new Intent(getActivity(), FastProvisionActivity.class));
                } else if (SharedPreferenceHelper.isAutoPvEnable(getActivity())) {
                    startActivity(new Intent(getActivity(), DeviceAutoProvisionActivity.class));
                } else {
                    startActivity(new Intent(getActivity(), DeviceProvisionActivity.class));
                }
            } else if (item.getItemId() == R.id.item_sort) {
                showSortDialog();
            }
            return false;
        });
        view.findViewById(R.id.tv_on).setOnClickListener(this);
        view.findViewById(R.id.tv_off).setOnClickListener(this);
        view.findViewById(R.id.tv_cmd).setOnClickListener(this);
        view.findViewById(R.id.tv_log).setOnClickListener(this);
        view.findViewById(R.id.tv_cycle).setOnClickListener(this);
        view.findViewById(R.id.btn_test).setOnClickListener(this);
        view.findViewById(R.id.btn_sns_get_all).setOnClickListener(this);

        RecyclerView gv_devices = view.findViewById(R.id.rv_online_devices);
        mDevices = TelinkMeshApplication.getInstance().getMeshInfo().nodes;
        mAdapter = new OnlineDeviceListAdapter(getActivity(), mDevices);

        gv_devices.setLayoutManager(new GridLayoutManager(getActivity(), 4));
        gv_devices.setAdapter(mAdapter);

        mAdapter.setOnItemClickListener(position -> {
            NodeInfo node = mDevices.get(position);
            if (node.isOffline()) return;
            if (node.isSensor()) return;

            int onOff = 0;
            if (node.getOnlineState() == OnlineState.OFF) {
                onOff = 1;
            }

            int address = mDevices.get(position).meshAddress;
            int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
            OnOffSetMessage onOffSetMessage = OnOffSetMessage.getSimple(address, appKeyIndex, onOff, !AppSettings.ONLINE_STATUS_ENABLE, !AppSettings.ONLINE_STATUS_ENABLE ? 1 : 0);
            MeshService.getInstance().sendMeshMessage(onOffSetMessage);

            /*int address = mDevices.get(position).meshAddress;
            int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
            OnOffSetMessage onOffSetMessage = OnOffSetMessage.getSimple(address, appKeyIndex, onOff, false, 0);
            MeshService.getInstance().sendMeshMessage(onOffSetMessage);

            mCycleHandler.removeCallbacksAndMessages(null);
            mCycleHandler.postDelayed(() -> {
                int modelId = MeshSigModel.SIG_MD_LIGHT_HSL_S.modelId;
                int modelEleAdr = mDevices.get(position).getTargetEleAdr(modelId);
                if (modelEleAdr != -1) {
                    MeshService.getInstance().sendMeshMessage(HslGetMessage.getSimple(modelEleAdr, appKeyIndex, 0));
                }
            }, 3000);*/

        });

        mAdapter.setOnItemLongClickListener(position -> {
            NodeInfo deviceInfo = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(mDevices.get(position).meshAddress);
            if (deviceInfo == null) {
                toastMsg("device info null!");
                return false;
            }
            /*if (deviceInfo.compositionData != null){
                MeshLogger.d("device cps: " + deviceInfo.compositionData.toString());
            }*/
            MeshLogger.d("deviceKey: " + (Arrays.bytesToHexString(deviceInfo.deviceKey)));
            Intent intent;
            if (deviceInfo.bound) {
                // remote control device
                if (AppSettings.isRemote(deviceInfo.compositionData.pid)) {
                    intent = new Intent(getActivity(), RemoteControlSettingActivity.class);
                } else {
                    intent = new Intent(getActivity(), DeviceSettingActivity.class);
                }
            } else {
                intent = new Intent(getActivity(), KeyBindActivity.class);
            }
            intent.putExtra("deviceAddress", deviceInfo.meshAddress);
            startActivity(intent);
            return false;
        });

        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);
        TelinkMeshApplication.getInstance().addEventListener(AutoConnectEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_MESH_RESET, this);
        TelinkMeshApplication.getInstance().addEventListener(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, this);
        TelinkMeshApplication.getInstance().addEventListener(CompositionDataStatusMessage.class.getName(), this);
    }

    @Override
    public void onResume() {
        super.onResume();
        MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        setSubTitle(getView(), meshInfo.meshName);
//        mDevices = TelinkMeshApplication.getInstance().getMeshInfo().devices;
//        mAdapter.notifyDataSetChanged();
        updateNodeBySort(TelinkMeshApplication.getInstance().getSortType());
        mAdapter.resetDevices(mDevices);
    }

    private void updateNodeBySort(NodeSortType sortType) {
        switch (sortType) {
            case NAME_ASC:
                Collections.sort(mDevices, (o1, o2) -> (o1.name == null ? ("" + o1.meshAddress) : o1.name).compareTo(o2.name == null ? ("" + o2.meshAddress) : o2.name));
                break;

            case NAME_DESC:
                Collections.sort(mDevices, (o1, o2) -> -(o1.name == null ? ("" + o1.meshAddress) : o1.name).compareTo(o2.name == null ? ("" + o2.meshAddress) : o2.name));
                break;

            case ADDRESS_ASC:
                Collections.sort(mDevices, (o1, o2) -> o1.meshAddress - o2.meshAddress);
                break;

            case ADDRESS_DESC:
                Collections.sort(mDevices, (o1, o2) -> o2.meshAddress - o1.meshAddress);
                break;
        }
        mAdapter.notifyDataSetChanged();
//        TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate();
    }

    private void resetSortIcon() {
        NodeSortType sortType = TelinkMeshApplication.getInstance().getSortType();
        switch (sortType) {
            case NAME_ASC:
                sortItem.setIcon(R.drawable.ic_sort_name_asc);
                break;

            case NAME_DESC:
                sortItem.setIcon(R.drawable.ic_sort_name_desc);
                break;

            case ADDRESS_ASC:
                sortItem.setIcon(R.drawable.ic_sort_adr_asc);
                break;

            case ADDRESS_DESC:
                sortItem.setIcon(R.drawable.ic_sort_adr_desc);
                break;
        }
    }


    @Override
    public void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
        mCycleHandler.removeCallbacksAndMessages(null);
    }

    public void showSortDialog() {
        AlertDialog.Builder actionSelectDialog = new AlertDialog.Builder(getActivity());
        String[] actions = new String[NodeSortType.values().length];
        for (int i = 0; i < actions.length; i++) {
            actions[i] = NodeSortType.values()[i].name;
        }
        actionSelectDialog.setItems(actions, (dialog, which) -> {
            NodeSortType sortType = NodeSortType.values()[which];
            TelinkMeshApplication.getInstance().resetSortType(sortType);
            resetSortIcon();
            updateNodeBySort(sortType);
        });
        actionSelectDialog.setTitle("Select Sort Type");
        actionSelectDialog.show();
    }

    private void toastMsg(String s) {
        ((MainActivity) getActivity()).toastMsg(s);
    }

    private void showIvWarningDialog() {
        ((BaseActivity) Objects.requireNonNull(getActivity())).showConfirmDialog("connect to the current network to get IV index before add nodes", (dialog, which) -> showIvInputDialog());
    }

    private void showIvInputDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this.getActivity());
        EditText et = new EditText(this.getActivity());
        builder.setView(et);
        builder.setTitle("input iv index for mesh network(HEX)");
        builder.setNegativeButton("Confirm", (dialog, which) -> {
            String ivInput = et.getText().toString();
            if (ivInput.isEmpty()) {
                toastMsg("iv index input error");
                return;
            }
            try {
                int idx = Integer.valueOf(ivInput, 16);
                TelinkMeshApplication.getInstance().getMeshInfo().ivIndex = idx;
                TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate();
            } catch (Exception e) {
                e.printStackTrace();
            }
        });
        builder.setPositiveButton("Cancel", null);
        builder.show();
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
                startActivity(new Intent(getActivity(), CmdActivity.class));
                break;

            case R.id.tv_log:
                startActivity(new Intent(getActivity(), LogActivity.class));
                break;

            case R.id.btn_test:
                startActivity(new Intent(getActivity(), OnOffTestActivity.class));
                break;

            case R.id.btn_sns_get_all:
                ((MainActivity) getActivity()).getSensorStates();
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
        } else if (eventType.equals(CompositionDataStatusMessage.class.getName())) {
            StatusNotificationEvent notificationEvent = (StatusNotificationEvent) event;
            CompositionDataStatusMessage statusMessage = (CompositionDataStatusMessage) notificationEvent.getNotificationMessage().getStatusMessage();
            MeshLogger.d("cps status page: " + statusMessage.getPage());
            MeshLogger.d("cps status: " + statusMessage.getCompositionData().toString());
        }
    }

}
