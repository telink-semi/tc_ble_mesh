/********************************************************************************************************
 * @file DeviceSelectAdapter.java
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
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.ui.IconGenerator;

import java.util.List;

/**
 * select device
 * Created by kee on 2017/8/18.
 */
public class DeviceSelectAdapter extends BaseSelectableListAdapter<DeviceSelectAdapter.ViewHolder> {

    private Context mContext;
    private List<NodeInfo> mDevices;


    public DeviceSelectAdapter(Context context, List<NodeInfo> devices) {
        this.mContext = context;
        this.mDevices = devices;
    }

    public boolean allSelected() {
        for (NodeInfo deviceInfo : mDevices) {
            if (!deviceInfo.selected) {
                return false;
            }
        }
        return true;
    }

    public void setAll(boolean selected) {
        for (NodeInfo deviceInfo : mDevices) {
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

        NodeInfo deviceInfo = mDevices.get(position);

        final int pid = deviceInfo.compositionData != null ? deviceInfo.compositionData.pid : 0;
        holder.iv_device.setImageResource(IconGenerator.getIcon(deviceInfo));
        holder.tv_device_info.setText(mContext.getString(R.string.device_state_desc,
                String.format("%04X", deviceInfo.meshAddress),
                deviceInfo.getOnlineState().toString()));

        holder.cb_device.setTag(position);

        if (!deviceInfo.isOffline()) {
            //  && deviceInfo.getTargetEleAdr(MeshSigModel.SIG_MD_SCENE_S.modelId) != -1
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
                statusChangedListener.onSelectStatusChanged(DeviceSelectAdapter.this);
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
