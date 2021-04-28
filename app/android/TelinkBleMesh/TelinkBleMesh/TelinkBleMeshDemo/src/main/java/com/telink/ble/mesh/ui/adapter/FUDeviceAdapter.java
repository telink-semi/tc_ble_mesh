/********************************************************************************************************
 * @file MeshOTADeviceSelectAdapter.java
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
package com.telink.ble.mesh.ui.adapter;

import android.content.Context;
import android.content.res.ColorStateList;
import android.graphics.Color;
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
import com.telink.ble.mesh.ui.IconGenerator;
import com.telink.ble.mesh.util.Arrays;

import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * mesh ota adapter
 * Created by kee on 2017/8/18.
 */
public class FUDeviceAdapter extends BaseRecyclerViewAdapter<FUDeviceAdapter.ViewHolder> {

    private Context mContext;
    private List<MeshUpdatingDevice> mDevices;

    public FUDeviceAdapter(Context context) {
        this.mContext = context;
    }

    public void resetDevices(List<MeshUpdatingDevice> mDevices) {
        this.mDevices = mDevices;
        notifyDataSetChanged();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_fu_device, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.iv_device = itemView.findViewById(R.id.iv_device);
        holder.tv_device_info = itemView.findViewById(R.id.tv_device_info);
        holder.tv_version = itemView.findViewById(R.id.tv_version);
        holder.iv_complete = itemView.findViewById(R.id.iv_complete);
        holder.tv_dev_state = itemView.findViewById(R.id.tv_dev_state);
        return holder;
    }

    @Override
    public int getItemCount() {
        return mDevices == null ? 0 : mDevices.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);

        MeshUpdatingDevice deviceInfo = mDevices.get(position);
        final int deviceType = deviceInfo.isLpn ? 1 : 0;
        holder.iv_device.setImageResource(IconGenerator.getIcon(deviceType, NodeInfo.ON_OFF_STATE_ON));
        String pidInfo = deviceInfo.pidInfo;
        holder.tv_device_info.setText(mContext.getString(R.string.device_state_desc_mesh_ota,
                String.format("%04X", deviceInfo.meshAddress),
                pidInfo));
        boolean support = deviceInfo.isSupported;

        String fwDesc = support ? "FwId:" + (deviceInfo.firmwareId == null ? "NULL" : Arrays.bytesToHexString(deviceInfo.firmwareId)) : "not support";
        holder.tv_version.setText(fwDesc);
        int deviceState = deviceInfo.state;
        if (deviceState == MeshUpdatingDevice.STATE_INITIAL) {
            holder.iv_complete.setImageResource(R.drawable.ic_mesh_ota);
        } else {
            if (deviceState == MeshUpdatingDevice.STATE_FAIL) {
                holder.iv_complete.setImageResource(R.drawable.ic_mesh_ota_fail);
            } else {
                holder.iv_complete.setImageResource(R.drawable.ic_mesh_ota_complete);
            }
        }
        holder.tv_dev_state.setText("state: " + deviceInfo.getStateDesc());
    }

    class ViewHolder extends RecyclerView.ViewHolder {
        ImageView iv_device;
        ImageView iv_complete;
        TextView tv_device_info, tv_version, tv_dev_state;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
