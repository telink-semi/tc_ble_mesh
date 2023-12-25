/********************************************************************************************************
 * @file DeviceConfigListAdapter.java
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
import android.graphics.drawable.Drawable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.DeviceConfig;
import com.telink.ble.mesh.ui.activity.DeviceConfigActivity;

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
