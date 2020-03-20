/********************************************************************************************************
 * @file DeviceProvisionListAdapter.java
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
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.model.ProvisioningDevice;
import com.telink.sig.mesh.model.DeviceInfo;

import java.util.List;

/**
 * provision list adapter
 * Created by Administrator on 2016/10/25.
 */
public class DeviceProvisionListAdapter extends BaseRecyclerViewAdapter<DeviceProvisionListAdapter.ViewHolder> {
    List<ProvisioningDevice> mDevices;
    Context mContext;

    public DeviceProvisionListAdapter(Context context, List<ProvisioningDevice> devices) {
        mContext = context;
        mDevices = devices;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_device_provision, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_device_info = itemView.findViewById(R.id.tv_device_info);
        holder.tv_state = itemView.findViewById(R.id.tv_state);
        holder.iv_device = itemView.findViewById(R.id.iv_device);
        holder.pb_provision = itemView.findViewById(R.id.pb_provision);
        return holder;
    }

    @Override
    public int getItemCount() {
        return mDevices == null ? 0 : mDevices.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        ProvisioningDevice device = mDevices.get(position);
        int iconRes = R.drawable.ic_bulb_on;
        if (device.nodeInfo != null && device.nodeInfo.cpsData.lowPowerSupport()) {
            iconRes = R.drawable.ic_low_power;
        }
        holder.iv_device.setImageResource(iconRes);

//        holder.tv_name.setText(mDevices.get(position).getAddress());
        holder.tv_device_info.setText(mContext.getString(R.string.device_prov_desc, String.format("%04X", device.unicastAddress), device.macAddress));
        holder.tv_state.setText(device.getStateDesc());

        if (device.state == ProvisioningDevice.STATE_PROVISIONING || device.state == ProvisioningDevice.STATE_BINDING) {
            holder.pb_provision.setIndeterminate(true);
        } else {
            holder.pb_provision.setIndeterminate(false);
            if (device.state == ProvisioningDevice.STATE_PROVISION_FAIL) {
                holder.pb_provision.setSecondaryProgress(100);
                holder.pb_provision.setProgress(0);
            } else if (device.state >= ProvisioningDevice.STATE_BIND_SUCCESS) {
                holder.pb_provision.setProgress(100);
                holder.pb_provision.setSecondaryProgress(0);
            } else {
                holder.pb_provision.setProgress(50);
                holder.pb_provision.setSecondaryProgress(100);
            }
        }

        /*if (device.bindState == DeviceBindState.BINDING) {
//            holder.pb_binding.setVisibility(View.VISIBLE);
            holder.pb_binding.setIndeterminate(true);
        } else {
//            holder.pb_binding.setVisibility(View.INVISIBLE);
            holder.pb_binding.setIndeterminate(false);
            if (device.bindState == DeviceBindState.BOUND) {
                holder.pb_binding.setProgress(100);
            } else {
                holder.pb_binding.setProgress(0);
            }
        }*/
//        holder.tv_device_info.setText(Integer.toHexString(device.meshAddress).toUpperCase() + " - \n" + device.macAddress + " - \n" + (device.bindState));
    }

    class ViewHolder extends RecyclerView.ViewHolder {
        // device icon
        public ImageView iv_device;
        // device mac, provisioning state
        public TextView tv_device_info, tv_state;
        ProgressBar pb_provision;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
