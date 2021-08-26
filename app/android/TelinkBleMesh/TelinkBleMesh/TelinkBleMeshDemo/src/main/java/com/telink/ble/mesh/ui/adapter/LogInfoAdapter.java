/********************************************************************************************************
 * @file     LogInfoAdapter.java 
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
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.util.LogInfo;
import com.telink.ble.mesh.util.MeshLogger;

import java.text.SimpleDateFormat;
import java.util.List;
import java.util.Locale;

import androidx.recyclerview.widget.RecyclerView;

/**
 * groups in GroupFragment
 * Created by kee on 2017/8/21.
 */

public class LogInfoAdapter extends BaseRecyclerViewAdapter<LogInfoAdapter.ViewHolder> {

    private Context mContext;
    private List<LogInfo> logInfoList;
    private SimpleDateFormat mDateFormat = new SimpleDateFormat("MM-dd HH:mm:ss.SSS", Locale.getDefault());

    public LogInfoAdapter(Context context, List<LogInfo> logInfoList) {
        this.mContext = context;
        this.logInfoList = logInfoList;
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
        return logInfoList.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        LogInfo logInfo = logInfoList.get(position);
        String info = mDateFormat.format(logInfo.millis) + "/" + logInfo.tag + " : " + logInfo.logMessage;
        holder.tv_name.setTextColor(mContext.getResources().getColor(getColorResId(logInfo.level)));
        holder.tv_name.setText(info);
    }


    class ViewHolder extends RecyclerView.ViewHolder {
        TextView tv_name;
        public ViewHolder(View itemView) {
            super(itemView);
        }
    }


    private int getColorResId(int level) {
        switch (level) {
            case MeshLogger.LEVEL_VERBOSE:
                return R.color.log_v;
            case MeshLogger.LEVEL_INFO:
                return R.color.log_i;
            case MeshLogger.LEVEL_WARN:
                return R.color.log_w;
            case MeshLogger.LEVEL_ERROR:
                return R.color.log_e;
            case MeshLogger.LEVEL_DEBUG:
            default:
                return R.color.log_d;

        }

    }
}
