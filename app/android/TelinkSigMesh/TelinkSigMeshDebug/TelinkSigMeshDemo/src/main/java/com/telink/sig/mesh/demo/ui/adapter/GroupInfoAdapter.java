/********************************************************************************************************
 * @file     GroupInfoAdapter.java 
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
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.TextView;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.model.Group;

import java.util.List;

/**
 * 指定设备的分组信息
 * Created by kee on 2017/8/18.
 */

public class GroupInfoAdapter extends BaseRecyclerViewAdapter<GroupInfoAdapter.ViewHolder> {

    private Context mContext;
    private List<Group> mGroups;
    private boolean enable;

    public GroupInfoAdapter(Context context, List<Group> groups) {
        this.mContext = context;
        this.mGroups = groups;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_device_group, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_name = (TextView) itemView.findViewById(R.id.tv_group_name);
        holder.cb_client = (CheckBox) itemView.findViewById(R.id.cb_client);
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
