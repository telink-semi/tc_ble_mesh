/********************************************************************************************************
 * @file DeviceAutoProvisionListAdapter.java
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
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.NetworkingDevice;
import com.telink.ble.mesh.model.NetworkingState;
import com.telink.ble.mesh.model.OnlineState;
import com.telink.ble.mesh.ui.activity.IconGenerator;

import java.util.List;

/**
 * provision list adapter
 * Created by Administrator on 2016/10/25.
 */
public class DeviceAutoProvisionListAdapter extends BaseRecyclerViewAdapter<DeviceAutoProvisionListAdapter.ViewHolder> {
    List<NetworkingDevice> mDevices;
    Context mContext;

    public DeviceAutoProvisionListAdapter(Context context, List<NetworkingDevice> devices) {
        mContext = context;
        mDevices = devices;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_device_auto_provision, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_device_info = itemView.findViewById(R.id.tv_device_info);
        holder.tv_state = itemView.findViewById(R.id.tv_state);
        holder.iv_device = itemView.findViewById(R.id.iv_device);
        holder.pb_provision = itemView.findViewById(R.id.pb_provision);
        holder.iv_cert = itemView.findViewById(R.id.iv_cert);
        return holder;
    }

    @Override
    public int getItemCount() {
        return mDevices == null ? 0 : mDevices.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        NetworkingDevice nodeInfo = mDevices.get(position);

//        MeshNode nodeInfo = device.no;
        int pid = (nodeInfo != null && nodeInfo.compositionData != null) ? nodeInfo.compositionData.pid : 0;
        holder.iv_device.setImageResource(IconGenerator.getIcon(pid, OnlineState.ON));
//        holder.tv_name.setText(mDevices.get(position).getAddress());
        String deviceDesc = mContext.getString(R.string.device_prov_desc, "0x" + String.format("%04X", nodeInfo.address), nodeInfo.deviceUUID);
        /*if (!TextUtils.isEmpty(nodeInfo.macAddress)) {
            deviceDesc += " - mac: " + nodeInfo.macAddress;
        }*/
        holder.tv_device_info.setText(deviceDesc);

        holder.tv_state.setText(nodeInfo.state.desc);

        boolean certVisible = MeshUtils.isCertSupported(nodeInfo.oobInfo);
        holder.iv_cert.setVisibility(certVisible ? View.VISIBLE : View.GONE);

        if (nodeInfo.state == NetworkingState.PROVISIONING || nodeInfo.state == NetworkingState.BINDING
                || nodeInfo.state == NetworkingState.TIME_PUB_SETTING) {
            holder.pb_provision.setIndeterminate(true);
        } else {
            holder.pb_provision.setIndeterminate(false);
            if (nodeInfo.state == NetworkingState.PROVISION_FAIL) {
                holder.pb_provision.setSecondaryProgress(100);
                holder.pb_provision.setProgress(0);
                if (nodeInfo.logs.size() > 0) {
                    holder.tv_state.setText(nodeInfo.logs.get(nodeInfo.logs.size() - 1).logMessage);
                }

            } else if (nodeInfo.bindState == 1) {
                holder.pb_provision.setProgress(100);
                holder.pb_provision.setSecondaryProgress(0);
            } else {
                holder.pb_provision.setProgress(50);
                holder.pb_provision.setSecondaryProgress(100);
            }
        }
    }

    class ViewHolder extends RecyclerView.ViewHolder {
        // device icon
        public ImageView iv_device, iv_cert;
        // device mac, provisioning state
        public TextView tv_device_info, tv_state;
        ProgressBar pb_provision;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
