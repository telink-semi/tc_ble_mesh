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
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

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
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.model.OnlineState;
import com.telink.ble.mesh.ui.activity.BaseActivity;
import com.telink.ble.mesh.ui.activity.CmdActivity;
import com.telink.ble.mesh.ui.activity.DeviceProvisionActivity;
import com.telink.ble.mesh.ui.activity.DeviceSettingActivity;
import com.telink.ble.mesh.ui.activity.KeyBindActivity;
import com.telink.ble.mesh.ui.activity.LogActivity;
import com.telink.ble.mesh.ui.activity.MainActivity;
import com.telink.ble.mesh.ui.adapter.OnlineDeviceListAdapter;
import com.telink.ble.mesh.ui.test.OnOffTestActivity;
import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.MeshInfoUpdateEvent;
import com.telink.ble.mesh.web.entity.MeshNode;

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
    private List<MeshNode> mDevices;
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

        toolbar.setNavigationOnClickListener(v -> {

//            ((MainActivity)getActivity()).getNetworkInfo(); //  for test

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
                for (MeshNode deviceInfo : mDevices) {
                    deviceInfo.setOnlineState(OnlineState.OFFLINE);
                }
                mAdapter.notifyDataSetChanged();
            }

        });

        toolbar.setOnMenuItemClickListener(item -> {
            if (TelinkMeshApplication.getInstance().getMeshInfo().ivIndex == MeshInfo.UNINITIALIZED_IVI) {
                showIvWarningDialog();
//                toastMsg("");
                return false;
            }
            if (item.getItemId() == R.id.item_add) {
                if (((BaseActivity) getActivity()).validateConfigPermission()) {
                    startActivity(new Intent(getActivity(), DeviceProvisionActivity.class));
                }
//                    startActivity(new Intent(getActivity(), DeviceProvisionActivity.class));
//                /*if (SharedPreferenceHelper.isRemoteProvisionEnable(getActivity()) && AppSettings.DRAFT_FEATURES_ENABLE) {
////                    startActivity(new Intent(getActivity(), RemoteProvisionActivity.class));
//                } else if (SharedPreferenceHelper.isFastProvisionEnable(getActivity())) {
////                    startActivity(new Intent(getActivity(), FastProvisionActivity.class));
//                } else if (SharedPreferenceHelper.isAutoPvEnable(getActivity())) {
////                    startActivity(new Intent(getActivity(), DeviceAutoProvisionActivity.class));
//                } else {
//                    startActivity(new Intent(getActivity(), DeviceProvisionActivity.class));
//                }*/
            }
            return false;
        });
        view.findViewById(R.id.tv_on).setOnClickListener(this);
        view.findViewById(R.id.tv_off).setOnClickListener(this);
        view.findViewById(R.id.tv_cmd).setOnClickListener(this);
        view.findViewById(R.id.tv_log).setOnClickListener(this);
        view.findViewById(R.id.tv_cycle).setOnClickListener(this);
        view.findViewById(R.id.btn_test).setOnClickListener(this);

        RecyclerView gv_devices = view.findViewById(R.id.rv_online_devices);
        mDevices = TelinkMeshApplication.getInstance().getNodeList();
        mAdapter = new OnlineDeviceListAdapter(getActivity(), mDevices);

        gv_devices.setLayoutManager(new GridLayoutManager(getActivity(), 4));
        gv_devices.setAdapter(mAdapter);

        mAdapter.setOnItemClickListener(position -> {
            if (mDevices.get(position).isOffline()) return;

            int onOff = 0;
            if (mDevices.get(position).getOnlineState() == OnlineState.OFF) {
                onOff = 1;
            }

            int address = mDevices.get(position).address;
            int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
            OnOffSetMessage onOffSetMessage = OnOffSetMessage.getSimple(address, appKeyIndex, onOff, !AppSettings.ONLINE_STATUS_ENABLE, !AppSettings.ONLINE_STATUS_ENABLE ? 1 : 0);
            MeshService.getInstance().sendMeshMessage(onOffSetMessage);
//                MeshService.getInstance().setOnOff(mDevices.get(position).meshAddress, onOff, !AppSettings.ONLINE_STATUS_ENABLE, !AppSettings.ONLINE_STATUS_ENABLE ? 1 : 0, 0, (byte) 0, null);
        });

        mAdapter.setOnItemLongClickListener(position -> {
            MeshNode deviceInfo = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(mDevices.get(position).address);
            if (deviceInfo == null) {
                toastMsg("device info null!");
                return false;
            }
            /*if (deviceInfo.compositionData != null){
                MeshLogger.d("device cps: " + deviceInfo.compositionData.toString());
            }*/
            MeshLogger.d("deviceKey: " + (deviceInfo.deviceKey));
            Intent intent;
            if (deviceInfo.isBond()) {
                intent = new Intent(getActivity(), DeviceSettingActivity.class);
            } else {
                intent = new Intent(getActivity(), KeyBindActivity.class);
            }
            intent.putExtra("deviceAddress", deviceInfo.address);
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
//                TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate(getActivity());
            } catch (Exception e) {
                e.printStackTrace();
            }
        });
        builder.setPositiveButton("Cancel", null);
        builder.show();
    }

    private void refreshUI() {
        mDevices = TelinkMeshApplication.getInstance().getNodeList();
        getActivity().runOnUiThread(() -> mAdapter.resetDevices(mDevices));
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {

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
//                startActivity(new Intent(getActivity(), CmdTestActivity.class));
                break;

            case R.id.tv_log:
                startActivity(new Intent(getActivity(), LogActivity.class));
                break;

            case R.id.btn_test:
//                startActivity(new Intent(getActivity(), ConnectionTestActivity.class));
                startActivity(new Intent(getActivity(), OnOffTestActivity.class));
                break;
        }
    }


    @Override
    public void performed(Event<String> event) {
        String eventType = event.getType();
        if (eventType.equals(MeshEvent.EVENT_TYPE_DISCONNECTED)
                || eventType.equals(MeshEvent.EVENT_TYPE_MESH_RESET)
                || eventType.equals(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED)
                || eventType.equals(AutoConnectEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN)
                || eventType.equals(MeshInfoUpdateEvent.EVENT_TYPE_NODES_UPDATE)) {
            refreshUI();
        } else if (eventType.equals(CompositionDataStatusMessage.class.getName())) {
            StatusNotificationEvent notificationEvent = (StatusNotificationEvent) event;
            CompositionDataStatusMessage statusMessage = (CompositionDataStatusMessage) notificationEvent.getNotificationMessage().getStatusMessage();
            MeshLogger.d("cps status page: " + statusMessage.getPage());
            MeshLogger.d("cps status: " + statusMessage.getCompositionData().toString());
        }
    }


}
