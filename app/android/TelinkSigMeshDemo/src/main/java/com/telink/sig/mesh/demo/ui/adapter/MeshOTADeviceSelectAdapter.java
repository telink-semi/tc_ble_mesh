/********************************************************************************************************
 * @file DeviceSelectAdapter.java
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
package com.telink.sig.mesh.demo.ui.adapter;

import android.content.Context;
import android.media.Image;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.ui.IconGenerator;
import com.telink.sig.mesh.model.DeviceBindState;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.SigMeshModel;

import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * 指定设备的分组信息
 * Created by kee on 2017/8/18.
 */
public class MeshOTADeviceSelectAdapter extends BaseSelectableListAdapter<MeshOTADeviceSelectAdapter.ViewHolder> {

    private Context mContext;
    private List<DeviceInfo> mDevices;
    private boolean started = false;
    private Set<Integer> completeNodes;
    private Map<Integer, String> versions;

    public MeshOTADeviceSelectAdapter(Context context, List<DeviceInfo> devices, Map<Integer, String> versions) {
        this.mContext = context;
        this.mDevices = devices;
        this.versions = versions;
    }

    public void setStarted(boolean started) {
        this.started = started;
    }

    public boolean allSelected() {
        for (DeviceInfo deviceInfo : mDevices) {
            if (!deviceInfo.selected) {
                return false;
            }
        }
        return true;
    }

    public void resetCompleteNodes(Set<Integer> completeNodes) {
        this.completeNodes = completeNodes;
        notifyDataSetChanged();
    }

    public void setAll(boolean selected) {
        for (DeviceInfo deviceInfo : mDevices) {
            deviceInfo.selected = selected;
        }
        notifyDataSetChanged();
    }


    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_device_select_mesh_ota, parent, false);
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

        DeviceInfo deviceInfo = mDevices.get(position);
        final int deviceType = deviceInfo.nodeInfo != null && deviceInfo.nodeInfo.cpsData.lowPowerSupport() ? 1 : 0;
        holder.iv_device.setImageResource(IconGenerator.getIcon(deviceType, deviceInfo.getOnOff()));
        String pidInfo = deviceInfo.getPidDesc();

        holder.tv_device_info.setText(mContext.getString(R.string.device_state_desc_mesh_ota,
                String.format("%04X", deviceInfo.meshAddress),
                pidInfo));
        holder.tv_version.setText("Ver:"+getDeviceVersion(deviceInfo));
        holder.cb_device.setTag(position);
        holder.cb_device.setChecked(deviceInfo.selected);
        if (deviceInfo.getOnOff() != -1) {
            holder.cb_device.setEnabled(!started);
            holder.cb_device.setOnCheckedChangeListener(this.checkedChangeListener);
        } else {
            holder.cb_device.setEnabled(!started && deviceInfo.selected);
        }

        holder.iv_complete.setVisibility(isDeviceComplete(deviceInfo) ? View.VISIBLE : View.GONE);
    }

    private String getDeviceVersion(DeviceInfo deviceInfo) {
        if (versions == null) return "NULL";
        for (Integer adr : versions.keySet()) {
            if (adr == deviceInfo.meshAddress) {
                return versions.get(adr);
            }
        }
        return "NULL";
    }

    private boolean isDeviceComplete(DeviceInfo deviceInfo) {
        if (completeNodes == null || completeNodes.size() == 0) return false;
        for (int node : completeNodes) {
            if (node == deviceInfo.meshAddress) {
                return true;
            }
        }
        return false;
    }

    private CompoundButton.OnCheckedChangeListener checkedChangeListener = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            int position = (int) buttonView.getTag();
            mDevices.get(position).selected = isChecked;
            if (statusChangedListener != null) {
                statusChangedListener.onStatusChanged(MeshOTADeviceSelectAdapter.this);
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
