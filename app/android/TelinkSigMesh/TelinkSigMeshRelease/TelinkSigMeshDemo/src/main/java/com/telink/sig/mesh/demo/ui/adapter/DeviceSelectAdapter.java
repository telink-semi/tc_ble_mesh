/********************************************************************************************************
 * @file DeviceSelectAdapter.java
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
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.TextView;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.ui.IconGenerator;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.Group;
import com.telink.sig.mesh.model.SigMeshModel;

import java.util.List;

/**
 * 指定设备的分组信息
 * Created by kee on 2017/8/18.
 */
public class DeviceSelectAdapter extends BaseSelectableListAdapter<DeviceSelectAdapter.ViewHolder> {

    private Context mContext;
    private List<DeviceInfo> mDevices;


    public DeviceSelectAdapter(Context context, List<DeviceInfo> devices) {
        this.mContext = context;
        this.mDevices = devices;
    }

    public boolean allSelected() {
        for (DeviceInfo deviceInfo : mDevices) {
            if (!deviceInfo.selected) {
                return false;
            }
        }
        return true;
    }

    public void setAll(boolean selected) {
        for (DeviceInfo deviceInfo : mDevices) {
            deviceInfo.selected = selected;
        }
        notifyDataSetChanged();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_device_select, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.iv_device = itemView.findViewById(R.id.iv_device);
        holder.tv_device_info = itemView.findViewById(R.id.tv_device_info);
        holder.cb_device = itemView.findViewById(R.id.cb_device);
        return holder;
    }

    @Override
    public int getItemCount() {
        return mDevices == null ? 0 : mDevices.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);

        DeviceInfo deviceInfo = mDevices.get(position);
        final int deviceType = deviceInfo.nodeInfo != null && deviceInfo.nodeInfo.cpsData.lowPowerSupport() ? 1 : 0;
        holder.iv_device.setImageResource(IconGenerator.getIcon(deviceType, deviceInfo.getOnOff()));
        holder.tv_device_info.setText(mContext.getString(R.string.device_state_desc,
                String.format("%04X", deviceInfo.meshAddress),
                deviceInfo.getOnOffDesc()));

        holder.cb_device.setTag(position);

        if (deviceInfo.getOnOff() != -1 && deviceInfo.getTargetEleAdr(SigMeshModel.SIG_MD_SCENE_S.modelId) != -1) {
            holder.cb_device.setChecked(deviceInfo.selected);
            holder.cb_device.setEnabled(true);
            holder.cb_device.setOnCheckedChangeListener(this.checkedChangeListener);
        } else {
            holder.cb_device.setChecked(false);
            holder.cb_device.setEnabled(false);
        }
    }

    private CompoundButton.OnCheckedChangeListener checkedChangeListener = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            int position = (int) buttonView.getTag();
            mDevices.get(position).selected = isChecked;
            if (statusChangedListener != null) {
                statusChangedListener.onStatusChanged(DeviceSelectAdapter.this);
            }
        }
    };

    class ViewHolder extends RecyclerView.ViewHolder {
        ImageView iv_device;
        TextView tv_device_info;
        CheckBox cb_device;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
