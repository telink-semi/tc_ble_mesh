/********************************************************************************************************
 * @file     DevicesInGroupAdapter.java 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.util.Arrays;

import java.util.List;

import androidx.recyclerview.widget.RecyclerView;

/**
 * devices in group
 */
public class DevicesInGroupAdapter extends BaseRecyclerViewAdapter<DevicesInGroupAdapter.ViewHolder> {
    List<NodeInfo> mDevices;
    Context mContext;

    public DevicesInGroupAdapter(Context context, List<NodeInfo> devices) {
        mContext = context;
        mDevices = devices;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_online_device, null, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_name = (TextView) itemView.findViewById(R.id.tv_name);
        holder.img_icon = (ImageView) itemView.findViewById(R.id.img_icon);
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

        if (device.getOnOff() == -1) {
            holder.img_icon.setImageResource(R.drawable.icon_light_offline);
            holder.tv_name.setTextColor(mContext.getResources().getColor(R.color.black));
        } else {
            if (device.meshAddress == (MeshService.getInstance().getDirectConnectedNodeAddress())) {
                holder.tv_name.setTextColor(mContext.getResources().getColor(R.color.colorPrimary));
            } else {
                holder.tv_name.setTextColor(mContext.getResources().getColor(R.color.black));
            }

            if (device.getOnOff() == 0)
                holder.img_icon.setImageResource(R.drawable.icon_light_off);
            else {
                holder.img_icon.setImageResource(R.drawable.icon_light_on);
            }
        }


//        holder.tv_name.setText(models.get(position).getAddress());
        String info = Integer.toHexString(device.meshAddress).toUpperCase();

        if (device.bound) {
            /*info += " : " +
                    (device.lum < 0 ? 0 : device.lum) + " : " +
                    device.temp;*/

        } else {
            info += "(unbound)";
        }

        info += "\n" + Arrays.bytesToHexString(device.deviceUUID, ":");
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
