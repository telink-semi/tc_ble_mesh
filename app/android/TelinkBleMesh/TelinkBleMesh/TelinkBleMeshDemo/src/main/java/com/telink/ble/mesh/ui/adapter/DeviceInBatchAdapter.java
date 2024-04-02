/********************************************************************************************************
 * @file DeviceProvisionListAdapter.java
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
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.ui.DeviceBatchSettingActivity;
import com.telink.ble.mesh.ui.IconGenerator;

import java.util.List;

/**
 * provision list adapter
 * Created by Administrator on 2016/10/25.
 */
public class DeviceInBatchAdapter extends BaseRecyclerViewAdapter<DeviceInBatchAdapter.ViewHolder> {
    private List<NodeInfo> mDevices;
    private Context mContext;

    public DeviceInBatchAdapter(Context context, List<NodeInfo> devices) {
        mContext = context;
        mDevices = devices;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_device_in_batch, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.iv_device = itemView.findViewById(R.id.iv_device);
        holder.tv_device_adr = itemView.findViewById(R.id.tv_device_adr);
        holder.tv_device_name = itemView.findViewById(R.id.tv_device_name);
        holder.iv_remove = itemView.findViewById(R.id.iv_remove);
        holder.iv_rename = itemView.findViewById(R.id.iv_rename);
        return holder;
    }

    @Override
    public int getItemCount() {
        return mDevices == null ? 0 : mDevices.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        NodeInfo deviceInfo = mDevices.get(position);
        holder.iv_device.setImageResource(IconGenerator.getIcon(deviceInfo));
        String deviceDesc = String.format("address: %04X mac: %s", deviceInfo.meshAddress, deviceInfo.macAddress);
        holder.tv_device_adr.setText(deviceDesc);
        if (deviceInfo.meshAddress == MeshService.getInstance().getDirectConnectedNodeAddress()) {
            holder.tv_device_adr.setTextColor(mContext.getResources().getColor(R.color.colorPrimary));
        } else {
            holder.tv_device_adr.setTextColor(mContext.getResources().getColor(R.color.black));
        }
        holder.tv_device_name.setText(deviceInfo.getName());
        holder.iv_rename.setOnClickListener(v -> ((DeviceBatchSettingActivity) mContext).showNameInputDialog(deviceInfo));
        holder.iv_remove.setOnClickListener(v -> ((DeviceBatchSettingActivity) mContext).showKickConfirmDialog(deviceInfo));
    }


    class ViewHolder extends RecyclerView.ViewHolder {
        // device icon
        public ImageView iv_device;
        // device mac, name
        public TextView tv_device_adr, tv_device_name;
        public ImageView iv_remove, iv_rename;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
