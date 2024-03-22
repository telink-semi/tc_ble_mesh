/********************************************************************************************************
 * @file LcPropertyListAdapter.java
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

import com.telink.ble.mesh.core.DeviceProperty;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.SensorControlActivity;
import com.telink.ble.mesh.util.Arrays;

import java.util.List;

/**
 * provision list adapter
 * Created by Administrator on 2016/10/25.
 */
public class SensorItemAdapter extends BaseRecyclerViewAdapter<SensorItemAdapter.ViewHolder> {
    private List<SensorControlActivity.SensorItem> sensorItems;
    private Context mContext;

    public SensorItemAdapter(Context context, List<SensorControlActivity.SensorItem> sensorItems) {
        mContext = context;
        this.sensorItems = sensorItems;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_sensor_item, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_prop_id = itemView.findViewById(R.id.tv_prop_id);
        holder.tv_desc = itemView.findViewById(R.id.tv_desc);
        holder.tv_cad = itemView.findViewById(R.id.tv_cad);
        holder.iv_get_desc = itemView.findViewById(R.id.iv_get_desc);
        holder.iv_get_cadence = itemView.findViewById(R.id.iv_get_cadence);
        return holder;
    }

    @Override
    public int getItemCount() {
        return sensorItems == null ? 0 : sensorItems.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        SensorControlActivity.SensorItem sensorItem = sensorItems.get(position);
        DeviceProperty property = DeviceProperty.getById(sensorItem.propertyId);
        String propName = property == null ? "UNKNOWN" : property.name;
        holder.tv_prop_id.setText(String.format("Property ID : 0x%04X(%s)", sensorItem.propertyId, propName));
        if (sensorItem.descriptor == null) {
            holder.tv_desc.setText("[NULL]");
        } else {
            String descInfo = String.format("Positive Tolerance : %04X \n", sensorItem.descriptor.positiveTolerance)
                    + String.format("Positive Tolerance : %04X \n", sensorItem.descriptor.positiveTolerance)
                    + String.format("Sampling Function : %02X \n", sensorItem.descriptor.samplingFunction)
                    + String.format("Measurement Period : %02X \n", sensorItem.descriptor.measurementPeriod)
                    + String.format("Update Interval : %02X \n", sensorItem.descriptor.updateInterval);
            holder.tv_desc.setText(descInfo);
        }

        if (sensorItem.cadence == null) {
            holder.tv_cad.setText("[NULL]");
        } else {
            String cadInfo = String.format("Fast Cadence Period Divisor : %02X \n", sensorItem.cadence.fastCadencePeriodDivisor)
                    + String.format("Status Trigger Type : %02X \n", sensorItem.cadence.statusTriggerType)
                    + "Status Trigger Delta Down : " + Arrays.bytesToHexString(sensorItem.cadence.statusTriggerDeltaDown) + " \n"
                    + "Status Trigger Delta Up : " + Arrays.bytesToHexString(sensorItem.cadence.statusTriggerDeltaUp) + " \n"
                    + String.format("Status Min Interval : %02X \n", sensorItem.cadence.statusMinInterval)
                    + "Fast Cadence Low : " + Arrays.bytesToHexString(sensorItem.cadence.fastCadenceLow) + " \n"
                    + "Fast Cadence High : " + Arrays.bytesToHexString(sensorItem.cadence.fastCadenceLow) + " \n";
            holder.tv_cad.setText(cadInfo);
        }

        holder.iv_get_desc.setVisibility(View.VISIBLE);
        holder.iv_get_desc.setTag(position);
        holder.iv_get_desc.setOnClickListener(clickListener);

        holder.iv_get_cadence.setVisibility(View.VISIBLE);
        holder.iv_get_cadence.setTag(position);
        holder.iv_get_cadence.setOnClickListener(clickListener);

    }

    private View.OnClickListener clickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            int position = (int) v.getTag();
            if (v.getId() == R.id.iv_get_desc) {
                ((SensorControlActivity) mContext).getDescriptor(sensorItems.get(position).propertyId);
            } else if (v.getId() == R.id.iv_get_cadence) {
                ((SensorControlActivity) mContext).getCadence(sensorItems.get(position).propertyId);
            }
        }
    };


    class ViewHolder extends RecyclerView.ViewHolder {
        public TextView tv_desc, tv_prop_id,
                tv_cad;  // cadence
        public ImageView iv_get_desc, iv_get_cadence;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
