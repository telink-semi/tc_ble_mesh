/********************************************************************************************************
 * @file FUDeviceSelectAdapter.java
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
package com.telink.ble.mesh.ui.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.ui.activity.IconGenerator;
import com.telink.ble.mesh.web.entity.MeshNode;
import com.telink.ble.mesh.web.entity.MeshProductInfo;

import java.util.List;
import java.util.Set;

/**
 * firmware update device list
 * Created by kee on 2017/8/18.
 */
public class FUDeviceSelectAdapter extends BaseSelectableListAdapter<FUDeviceSelectAdapter.ViewHolder> {

    private Context mContext;

    private List<MeshNode> mDevices;
    private boolean started = false;
    private Set<MeshUpdatingDevice> completeNodes;

    public FUDeviceSelectAdapter(Context context, List<MeshNode> devices) {
        this.mContext = context;
        this.mDevices = devices;
    }

    public MeshNode get(int position){
        return mDevices.get(position);
    }

    public void setStarted(boolean started) {
        this.started = started;
    }

    public boolean allSelected() {
        for (MeshNode deviceInfo : mDevices) {
            if (!deviceInfo.selected) {
                return false;
            }
        }
        return true;
    }

    public void resetCompleteNodes(Set<MeshUpdatingDevice> completeNodes) {
        this.completeNodes = completeNodes;
        notifyDataSetChanged();
    }

    public void setAll(boolean selected) {
        for (MeshNode deviceInfo : mDevices) {
            //  && deviceInfo.getOnOff() != -1
            if (isFirmwareUpdateSupport(deviceInfo))
                deviceInfo.selected = selected;
        }
        notifyDataSetChanged();
    }

    public void selectPid(int pid) {
        for (MeshNode deviceInfo : mDevices) {
            deviceInfo.selected = isFirmwareUpdateSupport(deviceInfo) && deviceInfo.compositionData().pid == pid && !deviceInfo.isOffline();
        }
        notifyDataSetChanged();
    }


    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_fu_device_select, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.iv_device = itemView.findViewById(R.id.iv_device);
        holder.tv_device_info = itemView.findViewById(R.id.tv_device_info);
        holder.tv_version = itemView.findViewById(R.id.tv_version);
        holder.cb_device = itemView.findViewById(R.id.cb_device);
        holder.iv_complete = itemView.findViewById(R.id.iv_complete);
        return holder;
    }

    @Override
    public int getItemCount() {
        return mDevices == null ? 0 : mDevices.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);

        MeshNode deviceInfo = mDevices.get(position);
        final int pid = deviceInfo.compositionData() != null ? deviceInfo.compositionData().pid : 0;
        holder.iv_device.setImageResource(IconGenerator.getIcon(pid, deviceInfo.getOnlineState()));
        String pidInfo = deviceInfo.getPidDesc();

        holder.tv_device_info.setText(mContext.getString(R.string.device_state_desc_mesh_ota,
                String.format("%04X", deviceInfo.address),
                pidInfo));
        boolean support = isFirmwareUpdateSupport(deviceInfo);
//        holder.tv_version.setText(support ? "FwId:" + getDeviceFirmwareId(deviceInfo) : "not support");
        holder.tv_version.setText(support ? "" : "not support");
        holder.cb_device.setTag(position);
        holder.cb_device.setChecked(deviceInfo.selected);
        // deviceInfo.getOnOff() != -1 &&
        if (support) {
            holder.cb_device.setEnabled(!started);
            holder.cb_device.setOnCheckedChangeListener(this.checkedChangeListener);
        } else {
            holder.cb_device.setEnabled(false);
        }
        int deviceState = getDeviceState(deviceInfo);

        if (deviceState == MeshUpdatingDevice.STATE_INITIAL) {
            holder.iv_complete.setVisibility(View.GONE);
        } else {
            holder.iv_complete.setVisibility(View.VISIBLE);
            if (deviceState == MeshUpdatingDevice.STATE_FAIL) {
//                holder.iv_complete.setColorFilter(R.color.grey);
                holder.iv_complete.setImageResource(R.drawable.ic_mesh_ota_fail);
            } else {
//                holder.iv_complete.setColorFilter(R.color.colorPrimary);
                holder.iv_complete.setImageResource(R.drawable.ic_mesh_ota_complete);
            }
        }
    }

    private boolean isFirmwareUpdateSupport(MeshNode nodeInfo) {
        return nodeInfo.getTargetEleAdr(MeshSigModel.SIG_MD_FW_UPDATE_S.modelId) != -1;
    }

    private int getDeviceState(MeshNode deviceInfo) {
        if (completeNodes == null || completeNodes.size() == 0) return 0;
        for (MeshUpdatingDevice node : completeNodes) {
            if (node.meshAddress == deviceInfo.address) {
                return node.state;
            }
        }
        return 0;
    }

    private CompoundButton.OnCheckedChangeListener checkedChangeListener = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            int position = (int) buttonView.getTag();
            mDevices.get(position).selected = isChecked;
            if (statusChangedListener != null) {
                statusChangedListener.onSelectStatusChanged(FUDeviceSelectAdapter.this, position);
            }
        }
    };

    class ViewHolder extends RecyclerView.ViewHolder {
        ImageView iv_device;
        ImageView iv_complete;
        TextView tv_device_info, tv_version;
        CheckBox cb_device;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
