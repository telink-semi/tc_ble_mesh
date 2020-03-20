
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
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.ui.CmdActivity;
import com.telink.ble.mesh.ui.DeviceProvisionActivity;
import com.telink.ble.mesh.ui.DeviceSettingActivity;
import com.telink.ble.mesh.ui.KeyBindActivity;
import com.telink.ble.mesh.ui.LogActivity;
import com.telink.ble.mesh.ui.MainActivity;
import com.telink.ble.mesh.ui.MeshTestActivity;
import com.telink.ble.mesh.ui.RemoteProvisionActivity;
import com.telink.ble.mesh.ui.adapter.BaseRecyclerViewAdapter;
import com.telink.ble.mesh.ui.adapter.OnlineDeviceListAdapter;

import java.util.List;

import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * 设备控制fragment
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
                Intent intent;
                if (deviceInfo.state == NodeInfo.STATE_BIND_SUCCESS) {
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
            // todo mesh interface
//            MeshService.getInstance().setOnOff(index % 2 == 0 ? 3 : 4, (byte) ((index / 2) % 2), true, 1, 0, (byte) 0, null);
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
//                TelinkLog.d("config: " + Arrays.bytesToHexString(config, ":"));
                startActivity(new Intent(getActivity(), CmdActivity.class));
                break;

            case R.id.tv_log:
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
        // todo mesh interface
        String eventType = event.getType();
        if (eventType.equals(MeshEvent.EVENT_TYPE_DISCONNECTED)
                || eventType.equals(MeshEvent.EVENT_TYPE_MESH_RESET)
                || eventType.equals(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED)) {
            refreshUI();
        }

        /* if (event.getType().equals(CommandEvent.EVENT_TYPE_CMD_COMPLETE)) {
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
        }*/

        /*if (event.getType().equals(NotificationEvent.EVENT_TYPE_DEVICE_ON_OFF_STATUS)) {
            refreshUI();
        } else if (event.getType().equals(MeshEvent.EVENT_TYPE_DISCONNECTED)) {
            refreshUI();
        } else if (event.getType().equals(NotificationEvent.EVENT_TYPE_CTL_STATUS_NOTIFY)) {
            refreshUI();
        }*/
    }


}
