/********************************************************************************************************
 * @file     GroupAdapter.java 
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
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.model.GroupInfo;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.core.message.generic.OnOffSetMessage;
import com.telink.ble.mesh.foundation.MeshService;

import java.util.List;

import androidx.recyclerview.widget.RecyclerView;

/**
 * groups in GroupFragment
 * Created by kee on 2017/8/21.
 */

public class GroupAdapter extends BaseRecyclerViewAdapter<GroupAdapter.ViewHolder> {

    private Context mContext;
    private List<GroupInfo> mGroups;

    public GroupAdapter(Context context, List<GroupInfo> groups) {
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
        GroupInfo group = mGroups.get(position);
        holder.tv_name.setText(group.name);

        holder.switch_on_off.setTag(position);
        holder.switch_on_off.setOnCheckedChangeListener(this.switchListener);
    }

    private CompoundButton.OnCheckedChangeListener switchListener = new CompoundButton.OnCheckedChangeListener() {

        @Override
        public void onCheckedChanged(CompoundButton v, boolean isChecked) {
            int position = (int) v.getTag();
            boolean ack;
            int rspMax;
            MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
            if (AppSettings.ONLINE_STATUS_ENABLE) {
                ack = false;
                rspMax = 0;
            } else {
                ack = true;
                rspMax = meshInfo.getOnlineCountInGroup(mGroups.get(position).address);
            }
            OnOffSetMessage message = OnOffSetMessage.getSimple(mGroups.get(position).address,
                    meshInfo.getDefaultAppKeyIndex(),
                    (byte) (isChecked ? 1 : 0),
                    ack,
                    rspMax);
            MeshService.getInstance().sendMeshMessage(message);
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
