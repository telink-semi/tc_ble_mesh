/********************************************************************************************************
 * @file     DeviceListAdapter.java 
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
package com.telink.sig.mesh.demo.ui.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.telink.sig.mesh.ble.AdvertisingDevice;
import com.telink.sig.mesh.demo.R;

import java.util.List;


/**
 * 设备列表适配器
 * Created by Administrator on 2016/10/25.
 */
public class DeviceListAdapter extends BaseAdapter {
    List<AdvertisingDevice> mDevices;
    Context mContext;

    public DeviceListAdapter(Context context, List<AdvertisingDevice> devices) {
        mContext = context;
        mDevices = devices;
    }

    @Override
    public int getCount() {
        return mDevices == null ? 0 : mDevices.size();
    }


    @Override
    public Object getItem(int position) {
        return mDevices.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolder holder;
        if (convertView == null) {
            convertView = LayoutInflater.from(mContext).inflate(R.layout.item_device_list, null);
            holder = new ViewHolder();
            holder.tv_name = (TextView) convertView.findViewById(R.id.tv_name);
            holder.tv_mac = (TextView) convertView.findViewById(R.id.tv_mac);
            convertView.setTag(holder);
        } else {
            holder = (ViewHolder) convertView.getTag();
        }

        holder.tv_name.setText(mDevices.get(position).device.getName());
        holder.tv_mac.setText(mDevices.get(position).device.getAddress());
        return convertView;
    }

    class ViewHolder {
        public TextView tv_name;
        public TextView tv_mac;
    }
}
