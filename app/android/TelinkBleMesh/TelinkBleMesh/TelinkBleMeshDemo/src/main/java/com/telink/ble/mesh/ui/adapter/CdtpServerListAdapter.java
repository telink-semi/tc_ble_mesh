/********************************************************************************************************
 * @file DeviceProvisionListAdapter.java
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

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.AdvertisingDevice;
import com.telink.ble.mesh.ui.cdtp.CdtpServerListActivity;

import java.util.List;

/**
 * cdtp server list adapter
 */
public class CdtpServerListAdapter extends BaseRecyclerViewAdapter<CdtpServerListAdapter.ViewHolder> {
    private List<AdvertisingDevice> mDevices;
    private Context mContext;

    public CdtpServerListAdapter(Context context, List<AdvertisingDevice> devices) {
        mContext = context;
        mDevices = devices;
    }

    public void setProcessing(boolean processing) {
        notifyDataSetChanged();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_cdtp_server, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_device_info = itemView.findViewById(R.id.tv_device_info);
        holder.tv_rssi = itemView.findViewById(R.id.tv_rssi);
        holder.btn_select = itemView.findViewById(R.id.btn_select);
        holder.tv_device_name = itemView.findViewById(R.id.tv_device_name);
        return holder;
    }

    @Override
    public int getItemCount() {
        return mDevices == null ? 0 : mDevices.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        AdvertisingDevice device = mDevices.get(position);
        holder.btn_select.setOnClickListener(v -> {
            ((CdtpServerListActivity) mContext).setResult(Activity.RESULT_OK, new Intent().putExtra("device", device.device));
            ((CdtpServerListActivity) mContext).finish();
        });
        holder.tv_rssi.setText(String.format("Server(rssi: %sdBm)", device.rssi));
        holder.tv_device_info.setText(device.device.getAddress());
        holder.tv_device_name.setText("" + device.device.getName());
    }


    static class ViewHolder extends RecyclerView.ViewHolder {
        TextView tv_device_info, tv_rssi, tv_device_name;
        Button btn_select;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
