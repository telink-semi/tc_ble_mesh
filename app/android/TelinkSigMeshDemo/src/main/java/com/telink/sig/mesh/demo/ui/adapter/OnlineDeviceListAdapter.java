/********************************************************************************************************
 * @file OnlineDeviceListAdapter.java
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
import android.widget.TextView;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.ui.IconGenerator;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.model.DeviceBindState;
import com.telink.sig.mesh.model.DeviceInfo;

import java.util.List;

/**
 * 设备列表适配器
 * Created by Administrator on 2016/10/25.
 */
public class OnlineDeviceListAdapter extends BaseRecyclerViewAdapter<OnlineDeviceListAdapter.ViewHolder> {
    List<DeviceInfo> mDevices;
    Context mContext;

    public OnlineDeviceListAdapter(Context context, List<DeviceInfo> devices) {
        mContext = context;
        mDevices = devices;
    }

    public void resetDevices(List<DeviceInfo> devices) {
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

        DeviceInfo device = mDevices.get(position);
        final int deviceType = device.nodeInfo != null && device.nodeInfo.cpsData.lowPowerSupport() ? 1 : 0;
        holder.img_icon.setImageResource(IconGenerator.getIcon(deviceType, device.getOnOff()));
        if (device.getOnOff() == -1) {
//            holder.img_icon.setImageResource(R.drawable.icon_light_offline);
            holder.tv_name.setTextColor(mContext.getResources().getColor(R.color.black));
        } else {
            if (device.macAddress.equals(MeshService.getInstance().getCurDeviceMac())) {
                holder.tv_name.setTextColor(mContext.getResources().getColor(R.color.colorPrimary));
            } else {
                holder.tv_name.setTextColor(mContext.getResources().getColor(R.color.black));
            }
        }


//        holder.tv_name.setText(models.get(position).getAddress());
        String info;
        if (device.meshAddress <= 0xFF) {
            info = String.format("%02X", device.meshAddress);
        } else {
            info = String.format("%04X", device.meshAddress);
        }


        if (device.bindState == DeviceBindState.BOUND) {


            info += (device.nodeInfo.cpsData.cid == 0x0211 ? "(Pid-" + String.format("%02X", device.nodeInfo.cpsData.pid) + ")"
                    : "(cid-" + String.format("%02X", device.nodeInfo.cpsData.cid) + ")");


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
