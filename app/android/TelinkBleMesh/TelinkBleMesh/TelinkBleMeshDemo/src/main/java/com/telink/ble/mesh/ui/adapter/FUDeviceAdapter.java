/********************************************************************************************************
 * @file FUDeviceAdapter.java
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
import android.widget.ImageView;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.firmwareupdate.AdditionalInformation;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.OnlineState;
import com.telink.ble.mesh.ui.IconGenerator;
import com.telink.ble.mesh.util.Arrays;

import java.util.List;

/**
 * mesh ota adapter
 * Created by kee on 2017/8/18.
 */
public class FUDeviceAdapter extends BaseRecyclerViewAdapter<FUDeviceAdapter.ViewHolder> {

    private Context mContext;
    private List<MeshUpdatingDevice> mDevices;
    private MeshInfo meshInfo;

    public FUDeviceAdapter(Context context) {
        this.mContext = context;
        meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
    }

    public void resetDevices(List<MeshUpdatingDevice> mDevices) {
        this.mDevices = mDevices;
        notifyDataSetChanged();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_fu_device, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.iv_device = itemView.findViewById(R.id.iv_device);
        holder.tv_device_info = itemView.findViewById(R.id.tv_device_info);
        holder.tv_version = itemView.findViewById(R.id.tv_version);
        holder.iv_complete = itemView.findViewById(R.id.iv_complete);
        holder.tv_dev_state = itemView.findViewById(R.id.tv_dev_state);
        return holder;
    }

    @Override
    public int getItemCount() {
        return mDevices == null ? 0 : mDevices.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);

        MeshUpdatingDevice deviceInfo = mDevices.get(position);
        NodeInfo nodeInfo = meshInfo.getDeviceByMeshAddress(deviceInfo.meshAddress);
        holder.iv_device.setImageResource(IconGenerator.getIcon(nodeInfo, OnlineState.ON));
        String pidInfo = deviceInfo.pidInfo;
        holder.tv_device_info.setText(mContext.getString(R.string.device_state_desc_mesh_ota,
                String.format("%04X", deviceInfo.meshAddress),
                pidInfo));
        boolean support = deviceInfo.isSupported;

        String fwDesc = support ? "FwId:" + (deviceInfo.firmwareId == null ? "NULL" : Arrays.bytesToHexString(deviceInfo.firmwareId)) : "not support";
        holder.tv_version.setText(fwDesc);
        int deviceState = deviceInfo.state;

        if (deviceState == MeshUpdatingDevice.STATE_FAIL) {
            holder.iv_complete.setImageResource(R.drawable.ic_mesh_ota_fail);
        } else if (deviceState == MeshUpdatingDevice.STATE_SUCCESS) {
            holder.iv_complete.setImageResource(R.drawable.ic_mesh_ota_complete);
        } else {
            holder.iv_complete.setImageResource(R.drawable.ic_mesh_ota);
        }


        holder.tv_dev_state.setText(String.format("state: %s\nadditional: %s", deviceInfo.getStateDesc(), getAdditionalDesc(deviceInfo.additionalInformation)));
    }

    private String getAdditionalDesc(AdditionalInformation additionalInformation) {
        if (additionalInformation == null) return "NULL";
        switch (additionalInformation) {
            case No_changes:
                return "No changes to CPS";
            case CPS_CHANGED_1:
                return "CPS changed, remote pv is not supported";
            case CPS_CHANGED_2:
                return "CPS changed, remote pv is supported";
            case NODE_UNPROVISIONED:
                return "Node unprovisioned, device will be removed after successful update";
        }
        return "";
    }

    class ViewHolder extends RecyclerView.ViewHolder {
        ImageView iv_device;
        ImageView iv_complete;
        TextView tv_device_info, tv_version, tv_dev_state;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
