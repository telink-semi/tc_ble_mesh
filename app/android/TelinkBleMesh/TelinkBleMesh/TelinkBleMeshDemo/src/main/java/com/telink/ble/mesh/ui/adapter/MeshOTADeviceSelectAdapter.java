package com.telink.ble.mesh.ui.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.ui.IconGenerator;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;

import java.util.List;
import java.util.Map;
import java.util.Set;

import androidx.recyclerview.widget.RecyclerView;

/**
 * mesh ota adapter
 * Created by kee on 2017/8/18.
 */
public class MeshOTADeviceSelectAdapter extends BaseSelectableListAdapter<MeshOTADeviceSelectAdapter.ViewHolder> {

    private Context mContext;
    private List<NodeInfo> mDevices;
    private boolean started = false;
    private Set<MeshUpdatingDevice> completeNodes;
    private Map<Integer, String> versions;

    public MeshOTADeviceSelectAdapter(Context context, List<NodeInfo> devices, Map<Integer, String> versions) {
        this.mContext = context;
        this.mDevices = devices;
        this.versions = versions;
    }

    public void setStarted(boolean started) {
        this.started = started;
    }

    public boolean allSelected() {
        for (NodeInfo deviceInfo : mDevices) {
            if (!deviceInfo.selected) {
                return false;
            }
        }
        return true;
    }

    public void resetCompleteNodes(Set<MeshUpdatingDevice> completeNodes) {
        this.completeNodes = completeNodes;
        notifyDataSetChanged();
    }

    public void setAll(boolean selected) {
        for (NodeInfo deviceInfo : mDevices) {
            deviceInfo.selected = selected;
        }
        notifyDataSetChanged();
    }


    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_device_select_mesh_ota, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.iv_device = itemView.findViewById(R.id.iv_device);
        holder.tv_device_info = itemView.findViewById(R.id.tv_device_info);
        holder.tv_version = itemView.findViewById(R.id.tv_version);
        holder.cb_device = itemView.findViewById(R.id.cb_device);
        holder.iv_complete = itemView.findViewById(R.id.iv_complete);
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
        final int deviceType = deviceInfo.compositionData != null && deviceInfo.compositionData.lowPowerSupport() ? 1 : 0;
        holder.iv_device.setImageResource(IconGenerator.getIcon(deviceType, deviceInfo.getOnOff()));
        String pidInfo = deviceInfo.getPidDesc();

        holder.tv_device_info.setText(mContext.getString(R.string.device_state_desc_mesh_ota,
                String.format("%04X", deviceInfo.meshAddress),
                pidInfo));
        holder.tv_version.setText("Ver:" + getDeviceVersion(deviceInfo));
        holder.cb_device.setTag(position);
        holder.cb_device.setChecked(deviceInfo.selected);
        if (deviceInfo.getOnOff() != -1) {
            holder.cb_device.setEnabled(!started);
            holder.cb_device.setOnCheckedChangeListener(this.checkedChangeListener);
        } else {
            holder.cb_device.setEnabled(!started && deviceInfo.selected);
        }
        int deviceState = getDeviceState(deviceInfo);

        if (deviceState == MeshUpdatingDevice.STATE_INITIAL) {
            holder.iv_complete.setVisibility(View.GONE);
        } else {
            holder.iv_complete.setVisibility(View.VISIBLE);
            if (deviceState == MeshUpdatingDevice.STATE_FAIL) {
                holder.iv_complete.setColorFilter(R.color.grey);
                holder.iv_complete.setImageResource(R.drawable.ic_mesh_ota_fail);
            } else {
                holder.iv_complete.setColorFilter(R.color.colorPrimary);
                holder.iv_complete.setImageResource(R.drawable.ic_mesh_ota_complete);
            }
        }
    }

    private String getDeviceVersion(NodeInfo deviceInfo) {
        if (versions == null) return "NULL";
        for (Integer adr : versions.keySet()) {
            if (adr == deviceInfo.meshAddress) {
                return versions.get(adr);
            }
        }
        return "NULL";
    }

    private int getDeviceState(NodeInfo deviceInfo) {
        if (completeNodes == null || completeNodes.size() == 0) return 0;
        for (MeshUpdatingDevice node : completeNodes) {
            if (node.getMeshAddress() == deviceInfo.meshAddress) {
                return node.getState();
            }
        }
        return 0;
    }

    private CompoundButton.OnCheckedChangeListener checkedChangeListener = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            int position = (int) buttonView.getTag();
            mDevices.get(position).selected = isChecked;
            if (statusChangedListener != null) {
                statusChangedListener.onStatusChanged(MeshOTADeviceSelectAdapter.this);
            }
        }
    };

    class ViewHolder extends RecyclerView.ViewHolder {
        ImageView iv_device;
        ImageView iv_complete;
        TextView tv_device_info, tv_version;
        CheckBox cb_device;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}