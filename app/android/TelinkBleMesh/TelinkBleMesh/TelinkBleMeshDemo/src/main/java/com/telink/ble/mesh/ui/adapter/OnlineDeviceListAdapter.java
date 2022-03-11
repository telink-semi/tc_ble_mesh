/********************************************************************************************************
 * @file OnlineDeviceListAdapter.java
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
import com.telink.ble.mesh.ui.IconGenerator;

import java.util.List;

/**
 * online devices
 * Created by Administrator on 2016/10/25.
 */
public class OnlineDeviceListAdapter extends BaseRecyclerViewAdapter<OnlineDeviceListAdapter.ViewHolder> {
    List<NodeInfo> mDevices;
    Context mContext;

    public OnlineDeviceListAdapter(Context context, List<NodeInfo> devices) {
        mContext = context;
        mDevices = devices;
    }

    public void resetDevices(List<NodeInfo> devices) {
        this.mDevices = devices;
        notifyDataSetChanged();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_online_device, null, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_name = itemView.findViewById(R.id.tv_name);
        holder.img_icon = itemView.findViewById(R.id.img_icon);
        return holder;
    }

    @Override
    public int getItemCount() {
        return mDevices == null ? 0 : mDevices.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);

        NodeInfo device = mDevices.get(position);
        final int pid = device.compositionData != null ? device.compositionData.pid : 0;
        holder.img_icon.setImageResource(IconGenerator.getIcon(pid, device.getOnlineState()));

        if (device.meshAddress == MeshService.getInstance().getDirectConnectedNodeAddress()) {
            holder.tv_name.setTextColor(mContext.getResources().getColor(R.color.colorPrimary));
        } else {
            holder.tv_name.setTextColor(mContext.getResources().getColor(R.color.black));
        }

        /*if (device.getOnOff() == -1) {
//            holder.img_icon.setImageResource(R.drawable.icon_light_offline);
            holder.tv_name.setTextColor(mContext.getResources().getColor(R.color.black));
        } else {
            if (device.macAddress != null && device.macAddress.equals(MeshService.getInstance().getCurDeviceMac())) {
                holder.tv_name.setTextColor(mContext.getResources().getColor(R.color.colorPrimary));
            } else {
                holder.tv_name.setTextColor(mContext.getResources().getColor(R.color.black));
            }
        }*/


//        holder.tv_name.setText(models.get(position).getAddress());
        String info;
        if (device.meshAddress <= 0xFF) {
            info = String.format("%02X", device.meshAddress);
        } else {
            info = String.format("%04X", device.meshAddress);
        }


        if (device.bound) {


            info += (device.compositionData.cid == 0x0211 ? "(Pid-" + String.format("%02X", device.compositionData.pid) + ")"
                    : "(cid-" + String.format("%02X", device.compositionData.cid) + ")");


            /*if (device.nodeInfo.cpsData.lowPowerSupport()) {
                info += "LPN";
            }*/
            /*" : " +(device.getOnOff() == 1 ? Math.max(0, device.lum) : 0) + " : " +
                    device.temp*/
        } else {
            info += "(unbound)";
        }
        holder.tv_name.setText(info);
    }

    class ViewHolder extends RecyclerView.ViewHolder {


        public ImageView img_icon;
        public TextView tv_name;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
