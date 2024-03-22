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
import android.text.TextUtils;
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
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.OnlineState;
import com.telink.ble.mesh.ui.IconGenerator;
import com.telink.ble.mesh.util.Arrays;

import java.util.List;

/**
 * provision list adapter
 * Created by Administrator on 2016/10/25.
 */
public class DeviceRemoteProvisionListAdapter extends BaseRecyclerViewAdapter<DeviceRemoteProvisionListAdapter.ViewHolder> {
    List<NetworkingDevice> mDevices;
    Context mContext;

    public DeviceRemoteProvisionListAdapter(Context context, List<NetworkingDevice> devices) {
        mContext = context;
        mDevices = devices;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_device_remote_provision, parent, false);
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
        NetworkingDevice device = mDevices.get(position);
        NodeInfo nodeInfo = device.nodeInfo;
        holder.iv_device.setImageResource(IconGenerator.getIcon(nodeInfo, OnlineState.ON));
//        holder.tv_name.setText(mDevices.get(position).getAddress());
        String deviceDesc = mContext.getString(R.string.device_prov_desc, "0x" + String.format("%04X", nodeInfo.meshAddress), Arrays.bytesToHexString(nodeInfo.deviceUUID));
        if (!TextUtils.isEmpty(nodeInfo.macAddress)) {
            deviceDesc += "\n - mac: " + nodeInfo.macAddress;
        }
        if (device.serverAddress == 0) {
            deviceDesc += "\n - [GATT]";
        } else {
            deviceDesc += String.format("\n - server: %04X", device.serverAddress);
        }
        deviceDesc += "\n - rssi: " + device.rssi + "dBm";

        holder.tv_device_info.setText(deviceDesc);

        holder.tv_state.setText(device.state.desc);

        boolean certVisible = MeshUtils.isCertSupported(device.oobInfo);
        holder.iv_cert.setVisibility(certVisible ? View.VISIBLE : View.GONE);

        if (device.state == NetworkingState.PROVISIONING || device.state == NetworkingState.BINDING
                || device.state == NetworkingState.TIME_PUB_SETTING) {
            holder.pb_provision.setIndeterminate(true);
        } else {
            holder.pb_provision.setIndeterminate(false);
            if (device.state == NetworkingState.PROVISION_FAIL) {
                holder.pb_provision.setSecondaryProgress(100);
                holder.pb_provision.setProgress(0);
                if (device.logs.size() > 0) {
                    holder.tv_state.setText(device.logs.get(device.logs.size() - 1).logMessage);
                }

            } else if (device.nodeInfo.bound) {
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
