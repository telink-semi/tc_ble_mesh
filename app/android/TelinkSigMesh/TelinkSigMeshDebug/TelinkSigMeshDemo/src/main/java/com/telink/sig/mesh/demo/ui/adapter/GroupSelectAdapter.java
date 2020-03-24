/********************************************************************************************************
 * @file     GroupSelectAdapter.java 
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
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.TextView;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.ui.IconGenerator;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.Group;

import java.util.ArrayList;
import java.util.List;

/**
 * 指定设备的分组信息
 * Created by kee on 2017/8/18.
 */

public class GroupSelectAdapter extends BaseSelectableListAdapter<GroupSelectAdapter.ViewHolder> {

    private Context mContext;
    private List<Group> mGroups;
    private List<DeviceInfo> mAllDevices;

    public GroupSelectAdapter(Context context, List<Group> groups, List<DeviceInfo> devices) {
        this.mContext = context;
        this.mGroups = groups;
        this.mAllDevices = devices;
    }

    public boolean allSelected() {
        for (Group group : mGroups) {
            if (!group.selected) {
                return false;
            }
        }
        return true;
    }

    public void setAll(boolean selected) {
        for (Group group : mGroups) {
            group.selected = selected;
        }
        notifyDataSetChanged();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_group_select, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_group_name = itemView.findViewById(R.id.tv_group_name);
        holder.cb_group = itemView.findViewById(R.id.cb_group);
        holder.rv_inner_device = itemView.findViewById(R.id.rv_inner_device);
        return holder;
    }

    @Override
    public int getItemCount() {
        return mGroups == null ? 0 : mGroups.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);

        Group group = mGroups.get(position);
        holder.tv_group_name.setText(group.name);
        holder.cb_group.setChecked(group.selected);
        holder.cb_group.setTag(position);
        holder.cb_group.setOnCheckedChangeListener(this.checkedChangeListener);
        holder.rv_inner_device.setLayoutManager(new LinearLayoutManager(mContext));
        holder.rv_inner_device.setAdapter(new SimpleDeviceAdapter(getInnerDevices(group)));
    }

    private CompoundButton.OnCheckedChangeListener checkedChangeListener = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            int position = (int) buttonView.getTag();
            mGroups.get(position).selected = isChecked;
            if (statusChangedListener != null) {
                statusChangedListener.onStatusChanged(GroupSelectAdapter.this);
            }
        }
    };

    private List<DeviceInfo> getInnerDevices(Group group) {
        if (group == null || mAllDevices == null) return null;
        List<DeviceInfo> inner = new ArrayList<>();
        for (DeviceInfo device : mAllDevices) {
            if (device.subList != null && device.subList.size() != 0) {
                for (int subAdr : device.subList) {
                    if (subAdr == group.address) {
                        inner.add(device);
                    }
                }
            }
        }
        return inner;
    }

    class ViewHolder extends RecyclerView.ViewHolder {
        TextView tv_group_name;

        CheckBox cb_group;

        RecyclerView rv_inner_device;
//        CheckBox cb_server;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }

    class SimpleDeviceViewHolder extends RecyclerView.ViewHolder {

        ImageView iv_device;
        TextView tv_device_info;

        public SimpleDeviceViewHolder(View itemView) {
            super(itemView);
        }
    }

    class SimpleDeviceAdapter extends BaseRecyclerViewAdapter<SimpleDeviceViewHolder> {

        List<DeviceInfo> innerDevices;

        SimpleDeviceAdapter(List<DeviceInfo> devices) {
            this.innerDevices = devices;
        }

        @Override
        public SimpleDeviceViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
            View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_simple_device, parent, false);
            SimpleDeviceViewHolder holder = new SimpleDeviceViewHolder(itemView);
            holder.iv_device = itemView.findViewById(R.id.iv_device);
            holder.tv_device_info = itemView.findViewById(R.id.tv_device_info);
            return holder;
        }

        @Override
        public void onBindViewHolder(SimpleDeviceViewHolder holder, int position) {
            super.onBindViewHolder(holder, position);

            DeviceInfo deviceInfo = innerDevices.get(position);
            final int deviceType = deviceInfo.nodeInfo != null && deviceInfo.nodeInfo.cpsData.lowPowerSupport() ? 1 : 0;
            holder.iv_device.setImageResource(IconGenerator.getIcon(deviceType, deviceInfo.getOnOff()));
            holder.tv_device_info.setText(mContext.getString(R.string.device_state_desc,
                    String.format("%04X", deviceInfo.meshAddress),
                    deviceInfo.getOnOffDesc()));

        }

        @Override
        public int getItemCount() {
            return this.innerDevices == null ? 0 : this.innerDevices.size();
        }
    }


}
