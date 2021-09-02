/********************************************************************************************************
 * @file DeviceConfigListAdapter.java
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
package com.telink.ble.mesh.ui.adapter;

import android.content.Context;
import android.content.res.ColorStateList;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.DeviceConfig;
import com.telink.ble.mesh.model.NetworkingDevice;
import com.telink.ble.mesh.model.NetworkingState;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.OnlineState;
import com.telink.ble.mesh.ui.DeviceConfigActivity;
import com.telink.ble.mesh.ui.DeviceProvisionActivity;
import com.telink.ble.mesh.ui.IconGenerator;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.LogInfo;

import java.util.List;

/**
 * provision list adapter
 * Created by Administrator on 2016/10/25.
 */
public class DeviceConfigListAdapter extends BaseRecyclerViewAdapter<DeviceConfigListAdapter.ViewHolder> {
    private List<DeviceConfig> configList;
    private Context mContext;

    public DeviceConfigListAdapter(Context context, List<DeviceConfig> configList) {
        mContext = context;
        this.configList = configList;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_device_config, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_cfg_name = itemView.findViewById(R.id.tv_cfg_name);
        holder.tv_cfg_def = itemView.findViewById(R.id.tv_cfg_def);
        holder.tv_cfg_val = itemView.findViewById(R.id.tv_cfg_val);
        holder.btn_get = itemView.findViewById(R.id.btn_get);
        holder.btn_set = itemView.findViewById(R.id.btn_set);
        holder.ll_detail = itemView.findViewById(R.id.ll_detail);
        holder.iv_expand = itemView.findViewById(R.id.iv_expand);
        holder.ll_expand = itemView.findViewById(R.id.ll_expand);
        return holder;
    }

    @Override
    public int getItemCount() {
        return configList == null ? 0 : configList.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        DeviceConfig deviceConfig = configList.get(position);

        holder.tv_cfg_name.setText(deviceConfig.configState.name);
        // "definition" +
        holder.tv_cfg_def.setText(deviceConfig.configState.definition);
        holder.tv_cfg_val.setText("value: " + deviceConfig.desc);

        holder.ll_expand.setTag(position);
        Drawable drawable = deviceConfig.expanded ? mContext.getResources().getDrawable(R.drawable.ic_arrow_down) : mContext.getResources().getDrawable(R.drawable.ic_arrow_right);
        holder.iv_expand.setImageDrawable(drawable);
        holder.ll_expand.setOnClickListener(clickListener);
        holder.ll_detail.setVisibility(deviceConfig.expanded ? View.VISIBLE : View.GONE);
        if (deviceConfig.configState.isGetSupported) {
            holder.btn_get.setVisibility(View.VISIBLE);
            holder.btn_get.setTag(position);
            holder.btn_get.setOnClickListener(clickListener);
        } else {
            holder.btn_get.setVisibility(View.INVISIBLE);
        }

        if (deviceConfig.configState.isSetSupported) {
            holder.btn_set.setVisibility(View.VISIBLE);
            holder.btn_set.setTag(position);
            holder.btn_set.setOnClickListener(clickListener);
        } else {
            holder.btn_set.setVisibility(View.INVISIBLE);
        }

    }

    private View.OnClickListener clickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            int position = (int) v.getTag();
            if (v.getId() == R.id.btn_get) {
                ((DeviceConfigActivity) mContext).onGetTap(position);
            } else if (v.getId() == R.id.btn_set) {
                ((DeviceConfigActivity) mContext).onSetTap(position);
            } else if (v.getId() == R.id.ll_expand) {
                configList.get(position).expanded = !configList.get(position).expanded;
                notifyDataSetChanged();
            }
        }
    };


    class ViewHolder extends RecyclerView.ViewHolder {
        public TextView tv_cfg_name, tv_cfg_def, tv_cfg_val;
        public Button btn_get, btn_set;
        public View ll_detail, ll_expand;
        public ImageView iv_expand;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
