/********************************************************************************************************
 * @file GroupSelectAdapter.java
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
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.GroupInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.ui.IconGenerator;

import java.util.ArrayList;
import java.util.List;

/**
 * set device group
 * Created by kee on 2017/8/18.
 */

public class GroupSelectAdapter extends BaseSelectableListAdapter<GroupSelectAdapter.ViewHolder> {

    private Context mContext;
    private List<GroupInfo> mGroups;
    private List<NodeInfo> mAllDevices;

    public GroupSelectAdapter(Context context, List<GroupInfo> groups, List<NodeInfo> devices) {
        this.mContext = context;
        this.mGroups = groups;
        this.mAllDevices = devices;
    }

    public boolean allSelected() {
        for (GroupInfo group : mGroups) {
            if (!group.selected) {
                return false;
            }
        }
        return true;
    }

    public void setAll(boolean selected) {
        for (GroupInfo group : mGroups) {
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

        GroupInfo group = mGroups.get(position);
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
                statusChangedListener.onSelectStatusChanged(GroupSelectAdapter.this);
            }
        }
    };

    private List<NodeInfo> getInnerDevices(GroupInfo group) {
        if (group == null || mAllDevices == null) return null;
        List<NodeInfo> inner = new ArrayList<>();
        for (NodeInfo device : mAllDevices) {
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

        List<NodeInfo> innerDevices;

        SimpleDeviceAdapter(List<NodeInfo> devices) {
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

            NodeInfo deviceInfo = innerDevices.get(position);
            final int pid = deviceInfo.compositionData != null ? deviceInfo.compositionData.pid : 0;
            holder.iv_device.setImageResource(IconGenerator.getIcon(pid, deviceInfo.getOnlineState()));
            holder.tv_device_info.setText(mContext.getString(R.string.device_state_desc,
                    String.format("%04X", deviceInfo.meshAddress),
                    deviceInfo.getOnlineState().toString()));

        }

        @Override
        public int getItemCount() {
            return this.innerDevices == null ? 0 : this.innerDevices.size();
        }
    }


}
