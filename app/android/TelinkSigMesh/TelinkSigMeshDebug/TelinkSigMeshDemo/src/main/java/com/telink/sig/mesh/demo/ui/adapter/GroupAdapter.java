/********************************************************************************************************
 * @file GroupAdapter.java
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
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.TextView;

import com.telink.sig.mesh.demo.AppSettings;
import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.model.Group;

import java.util.List;

/**
 * 分组列表 in GroupFragment
 * Created by kee on 2017/8/21.
 */

public class GroupAdapter extends BaseRecyclerViewAdapter<GroupAdapter.ViewHolder> {

    private Context mContext;
    private List<Group> mGroups;

    public GroupAdapter(Context context, List<Group> groups) {
        this.mContext = context;
        this.mGroups = groups;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_group, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_name = itemView.findViewById(R.id.tv_group_name);
        holder.switch_on_off = itemView.findViewById(R.id.switch_on_off);
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

        holder.switch_on_off.setTag(position);
        holder.switch_on_off.setOnCheckedChangeListener(this.switchListener);
    }

    private CompoundButton.OnCheckedChangeListener switchListener = new CompoundButton.OnCheckedChangeListener() {

        @Override
        public void onCheckedChanged(CompoundButton v, boolean isChecked) {
            int position = (int) v.getTag();
            if (AppSettings.ONLINE_STATUS_ENABLE){
                MeshService.getInstance().setOnOff(mGroups.get(position).address, (byte) (isChecked ? 1 : 0), false, 0, 0, (byte) 0, null);
            }else {
                int repMax = TelinkMeshApplication.getInstance().getMesh().getOnlineCountInGroup(mGroups.get(position).address);
//            MeshService.getInstance().cmdOnOff(mGroups.get(position).address, (byte) repMax, isChecked ? (byte) 1 : 0, 1);
                MeshService.getInstance().setOnOff(mGroups.get(position).address, (byte) (isChecked ? 1 : 0), true, repMax, 0, (byte) 0, null);
            }
        }
    };

    class ViewHolder extends RecyclerView.ViewHolder {
        TextView tv_name;
        Switch switch_on_off;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
