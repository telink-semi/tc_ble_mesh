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
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.DeviceConfig;

import java.util.List;

/**
 * provision list adapter
 * Created by Administrator on 2016/10/25.
 */
public class ConfigParamListAdapter extends BaseRecyclerViewAdapter<ConfigParamListAdapter.ViewHolder> {
    private List<DeviceConfig.Parameter> parameterList;
    private Context mContext;

    public ConfigParamListAdapter(Context context, List<DeviceConfig.Parameter> parameterList) {
        mContext = context;
        this.parameterList = parameterList;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_config_param, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_name = itemView.findViewById(R.id.tv_name);
        holder.et_input = itemView.findViewById(R.id.et_input);
        return holder;
    }

    @Override
    public int getItemCount() {
        return parameterList == null ? 0 : parameterList.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        DeviceConfig.Parameter parameter = parameterList.get(position);

        holder.tv_name.setText(parameter.key);
        if (parameter.value != null) {
            holder.et_input.setText(parameter.value.toString());
        } else {
            holder.et_input.setText("");
        }
    }

    public class ViewHolder extends RecyclerView.ViewHolder {
        public TextView tv_name;
        public EditText et_input;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
