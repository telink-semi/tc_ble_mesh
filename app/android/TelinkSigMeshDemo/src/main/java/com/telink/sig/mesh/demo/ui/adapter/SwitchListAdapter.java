/********************************************************************************************************
 * @file     SwitchListAdapter.java 
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
import android.content.Intent;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.Switch;
import android.widget.TextView;

import com.telink.sig.mesh.demo.AppSettings;
import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.ui.SchedulerSettingActivity;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.model.Scheduler;

import java.util.List;

/**
 * 设备列表适配器
 * Created by Administrator on 2016/10/25.
 */
public class SwitchListAdapter extends BaseRecyclerViewAdapter<SwitchListAdapter.ViewHolder> {


    List<Integer> mAdrList;
    Context mContext;
    int address;

    public SwitchListAdapter(Context context, List<Integer> adrList) {
        mContext = context;
        mAdrList = adrList;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_switch, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_ele = itemView.findViewById(R.id.tv_ele);
        holder.switch_ele = itemView.findViewById(R.id.switch_ele);
        return holder;
    }

    @Override
    public int getItemCount() {
        return mAdrList == null ? 0 : mAdrList.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        int adr = mAdrList.get(position);
        holder.tv_ele.setText("ele adr: " + adr);
        holder.switch_ele.setTag(adr);
        holder.switch_ele.setOnCheckedChangeListener(switchChangeListner);
    }

    private CompoundButton.OnCheckedChangeListener switchChangeListner = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            int adr = (int) buttonView.getTag();
//            MeshService.getInstance().cmdOnOff(adr, (byte) 1, (byte) (isChecked ? 1 : 0), 1);
            MeshService.getInstance().setOnOff(adr, (byte) (isChecked ? 1 : 0), !AppSettings.ONLINE_STATUS_ENABLE, !AppSettings.ONLINE_STATUS_ENABLE ? 1 : 0, 0, (byte) 0, null);
        }
    };


    class ViewHolder extends RecyclerView.ViewHolder {


        private TextView tv_ele;
        private Switch switch_ele;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
