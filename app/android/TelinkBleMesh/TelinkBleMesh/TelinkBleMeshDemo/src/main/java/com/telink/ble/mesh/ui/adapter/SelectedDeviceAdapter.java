/********************************************************************************************************
 * @file SelectedDeviceAdapter.java
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
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.OnlineState;
import com.telink.ble.mesh.ui.IconGenerator;

import java.util.List;

/**
 * mesh ota adapter
 * Created by kee on 2017/8/18.
 */
public class SelectedDeviceAdapter extends BaseRecyclerViewAdapter<SelectedDeviceAdapter.ViewHolder> {

    private Context mContext;
    private List<NodeInfo> mDevices;

    public SelectedDeviceAdapter(Context context) {
        this.mContext = context;
    }

    public void resetDevices(List<NodeInfo> nodeInfos) {
        this.mDevices = nodeInfos;
        notifyDataSetChanged();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_simple_device, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.iv_device = itemView.findViewById(R.id.iv_device);
        holder.tv_device_info = itemView.findViewById(R.id.tv_device_info);
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
        final int pid = deviceInfo.compositionData == null ? 0 : deviceInfo.compositionData.pid;
        holder.iv_device.setImageResource(IconGenerator.getIcon(pid, OnlineState.ON));

        holder.tv_device_info.setText(String.format("Node-%04X", deviceInfo.meshAddress));
    }

    class ViewHolder extends RecyclerView.ViewHolder {
        ImageView iv_device;
        TextView tv_device_info;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
