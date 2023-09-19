/********************************************************************************************************
 * @file GroupInfoAdapter.java
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
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.GroupInfo;

import java.util.List;

/**
 * groups
 * Created by kee on 2017/8/18.
 */

public class GroupInfoAdapter extends BaseRecyclerViewAdapter<GroupInfoAdapter.ViewHolder> {

    private Context mContext;
    private List<GroupInfo> mGroups;
    private boolean enable;

    public GroupInfoAdapter(Context context, List<GroupInfo> groups) {
        this.mContext = context;
        this.mGroups = groups;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_device_group, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_name = itemView.findViewById(R.id.tv_group_name);
        holder.cb_client = itemView.findViewById(R.id.cb_client);
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
        holder.tv_name.setText(group.name);
        holder.cb_client.setChecked(group.selected);
        holder.cb_client.setEnabled(enable);
    }

    public void setEnabled(boolean enable) {
        this.enable = enable;
        notifyDataSetChanged();
    }

    private View.OnClickListener clientClick = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            int position = (int) v.getTag();


            /*Group group = mGroups.get(position);
            if (mDeviceInfo.subList != null) {
                if (((CheckBox) v).isChecked()) {
                    mDeviceInfo.subList.add((Integer) (group.getAddress()));
                } else {
                    mDeviceInfo.subList.remove((Integer) (group.getAddress()));
                }
            }*/
        }
    };


    class ViewHolder extends RecyclerView.ViewHolder {
        TextView tv_name;
        CheckBox cb_client;
//        CheckBox cb_server;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
