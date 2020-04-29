package com.telink.ble.mesh.ui.adapter;

import android.content.Context;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.util.Arrays;

import java.util.List;

import androidx.recyclerview.widget.RecyclerView;

/**
 * provision list adapter
 * Created by Administrator on 2016/10/25.
 */
public class DeviceProvisionListAdapter extends BaseRecyclerViewAdapter<DeviceProvisionListAdapter.ViewHolder> {
    List<NodeInfo> mDevices;
    Context mContext;

    public DeviceProvisionListAdapter(Context context, List<NodeInfo> devices) {
        mContext = context;
        mDevices = devices;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_device_provision, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_device_info = itemView.findViewById(R.id.tv_device_info);
        holder.tv_state = itemView.findViewById(R.id.tv_state);
        holder.iv_device = itemView.findViewById(R.id.iv_device);
        holder.pb_provision = itemView.findViewById(R.id.pb_provision);
        return holder;
    }

    @Override
    public int getItemCount() {
        return mDevices == null ? 0 : mDevices.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        NodeInfo device = mDevices.get(position);
        int iconRes = R.drawable.ic_bulb_on;
        if (device.compositionData != null && device.compositionData.lowPowerSupport()) {
            iconRes = R.drawable.ic_low_power;
        }
        holder.iv_device.setImageResource(iconRes);

//        holder.tv_name.setText(mDevices.get(position).getAddress());
        String deviceDesc = mContext.getString(R.string.device_prov_desc, String.format("%04X", device.meshAddress), Arrays.bytesToHexString(device.deviceUUID));
        if (!TextUtils.isEmpty(device.macAddress)) {
            deviceDesc += " - mac: " + device.macAddress;
        }
        holder.tv_device_info.setText(deviceDesc);
        holder.tv_state.setText(device.getStateDesc());

        if (device.state == NodeInfo.STATE_PROVISIONING || device.state == NodeInfo.STATE_BINDING) {
            holder.pb_provision.setIndeterminate(true);
        } else {
            holder.pb_provision.setIndeterminate(false);
            if (device.state == NodeInfo.STATE_PROVISION_FAIL) {
                holder.pb_provision.setSecondaryProgress(100);
                holder.pb_provision.setProgress(0);
            } else if (device.state == NodeInfo.STATE_BIND_SUCCESS) {
                holder.pb_provision.setProgress(100);
                holder.pb_provision.setSecondaryProgress(0);
            } else {
                holder.pb_provision.setProgress(50);
                holder.pb_provision.setSecondaryProgress(100);
            }
        }
    }

    class ViewHolder extends RecyclerView.ViewHolder {
        // device icon
        public ImageView iv_device;
        // device mac, provisioning state
        public TextView tv_device_info, tv_state;
        ProgressBar pb_provision;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
