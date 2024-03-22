/********************************************************************************************************
 * @file NodeElementSelectAdapter.java
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

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.ui.IconGenerator;
import com.telink.ble.mesh.ui.SceneSettingActivity;

import java.util.List;

/**
 * select device element that contains scene server model
 */
public class NodeElementSelectAdapter extends BaseRecyclerViewAdapter<NodeElementSelectAdapter.ViewHolder> {

    private Context mContext;
    private List<NodeInfo> mDevices;


    public NodeElementSelectAdapter(Context context, List<NodeInfo> devices) {
        this.mContext = context;
        this.mDevices = devices;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_node_element_select, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.iv_device = itemView.findViewById(R.id.iv_device);
        holder.tv_device_info = itemView.findViewById(R.id.tv_device_info);
        holder.rv_element = itemView.findViewById(R.id.rv_element);
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

        final int pid = deviceInfo.compositionData != null ? deviceInfo.compositionData.pid : 0;
        holder.iv_device.setImageResource(IconGenerator.getIcon(deviceInfo));
        /*holder.tv_device_info.setText(mContext.getString(R.string.device_state_desc,
                String.format("%04X", deviceInfo.meshAddress),
                deviceInfo.getOnlineState().toString()));*/
        holder.tv_device_info.setText(String.format("address: %04X", deviceInfo.meshAddress));

        ElementSelectAdapter elementSelectAdapter = new ElementSelectAdapter(mContext, deviceInfo);
        elementSelectAdapter.setOnItemClickListener(position1 ->
                ((SceneSettingActivity) mContext).setScene(
                        deviceInfo.meshAddress + position1,
                        !elementSelectAdapter.isSelected(position1),
                        deviceInfo
                ));
        holder.rv_element.setLayoutManager(new LinearLayoutManager(mContext));
        holder.rv_element.setAdapter(elementSelectAdapter);
    }

    static class ViewHolder extends RecyclerView.ViewHolder {
        ImageView iv_device;
        TextView tv_device_info;
        RecyclerView rv_element;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
