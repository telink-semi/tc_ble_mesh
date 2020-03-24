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
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.TextView;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.model.LogInfo;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.model.Group;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.time.format.DateTimeFormatter;
import java.util.List;
import java.util.Locale;

/**
 * 分组列表 in GroupFragment
 * Created by kee on 2017/8/21.
 */

public class LogInfoAdapter extends BaseRecyclerViewAdapter<LogInfoAdapter.ViewHolder> {

    private Context mContext;
    private List<LogInfo> mLogs;
    private SimpleDateFormat mDateFormat = new SimpleDateFormat("MM-dd HH:mm:ss.SSS", Locale.getDefault());

    public LogInfoAdapter(Context context, List<LogInfo> logs) {
        this.mContext = context;
        this.mLogs = logs;
    }

    public void refreshLogs(List<LogInfo> logs) {
        this.mLogs = logs;
        this.notifyDataSetChanged();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_log_info, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_name = itemView.findViewById(R.id.tv_log);
        return holder;
    }

    @Override
    public int getItemCount() {
        return mLogs == null ? 0 : mLogs.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        LogInfo logInfo = mLogs.get(position);
        String info = mDateFormat.format(logInfo.datetime) + "/" + logInfo.tag + " : " + logInfo.log;
        holder.tv_name.setText(info);
    }


    class ViewHolder extends RecyclerView.ViewHolder {
        TextView tv_name;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
