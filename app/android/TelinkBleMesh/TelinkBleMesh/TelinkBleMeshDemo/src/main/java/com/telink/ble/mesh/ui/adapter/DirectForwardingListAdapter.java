/********************************************************************************************************
 * @file DirectForwardingListAdapter.java
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
import android.util.ArraySet;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.DirectForwardingInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.OnlineState;
import com.telink.ble.mesh.ui.IconGenerator;

import java.util.ArrayList;
import java.util.List;

public class DirectForwardingListAdapter extends BaseRecyclerViewAdapter<DirectForwardingListAdapter.ViewHolder> {
    List<DirectForwardingInfo> infoList;
    Context mContext;

    public DirectForwardingListAdapter(Context context, List<DirectForwardingInfo> infoList) {
        mContext = context;
        this.infoList = infoList;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_df_info, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_origin = itemView.findViewById(R.id.tv_origin);
        holder.tv_target = itemView.findViewById(R.id.tv_target);
        holder.tv_backward = itemView.findViewById(R.id.tv_backward);
        holder.rv_nodes = itemView.findViewById(R.id.rv_nodes);

        return holder;
    }

    @Override
    public int getItemCount() {
        return infoList == null ? 0 : infoList.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);

        DirectForwardingInfo info = infoList.get(position);

        int localAdr = TelinkMeshApplication.getInstance().getMeshInfo().localAddress;
//        holder.tv_origin.setText(mContext.getString(R.string.df_origin_desc, String.format("%04X", info.originAdr)));
        holder.tv_origin.setText(String.format("0x%04X", info.originAdr));
        holder.tv_target.setText(String.format("0x%04X", info.target));

        holder.rv_nodes.setLayoutManager(new LinearLayoutManager(mContext));
        holder.rv_nodes.setAdapter(new SimpleDeviceAdapter(info.nodesOnRoute));
    }

    class ViewHolder extends RecyclerView.ViewHolder {
        TextView tv_origin, tv_target, tv_backward;
        RecyclerView rv_nodes;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }


    static class SimpleDeviceViewHolder extends RecyclerView.ViewHolder {

        ImageView iv_device;
        TextView tv_device_info;

        SimpleDeviceViewHolder(View itemView) {
            super(itemView);
        }
    }

    class SimpleDeviceAdapter extends BaseRecyclerViewAdapter<SimpleDeviceViewHolder> {

        ArrayList<Integer> innerDevices;

        SimpleDeviceAdapter(ArrayList<Integer> devices) {
            this.innerDevices = devices;
        }

        @Override
        public SimpleDeviceViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
            View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_df_device, parent, false);
            SimpleDeviceViewHolder holder = new SimpleDeviceViewHolder(itemView);
            holder.iv_device = itemView.findViewById(R.id.iv_device);
            holder.tv_device_info = itemView.findViewById(R.id.tv_device_info);
            return holder;
        }

        @Override
        public void onBindViewHolder(SimpleDeviceViewHolder holder, int position) {
            super.onBindViewHolder(holder, position);

            NodeInfo nodeInfo = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(innerDevices.get(position));
            int pid = nodeInfo.compositionData != null ? nodeInfo.compositionData.pid : 0;
            holder.iv_device.setImageResource(IconGenerator.getIcon(pid, OnlineState.ON));
//            holder.iv_device.setVisibility(View.GONE);
            holder.tv_device_info.setText(String.format("Node-%04X", innerDevices.get(position)));
        }

        @Override
        public int getItemCount() {
            return this.innerDevices == null ? 0 : this.innerDevices.size();
        }
    }

}
