/********************************************************************************************************
 * @file DirectForwardingListAdapter.java
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
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.core.message.directforwarding.DirectedControlSetMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.ui.IconGenerator;

import java.util.List;

/**
 * todo direct forwarding list
 */
public class DirectToggleListAdapter extends BaseRecyclerViewAdapter<DirectToggleListAdapter.ViewHolder> {
    List<NodeInfo> infoList;
    Context mContext;

    public DirectToggleListAdapter(Context context, List<NodeInfo> infoList) {
        mContext = context;
        this.infoList = infoList;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_direct_toggle, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_device_info = itemView.findViewById(R.id.tv_device_info);
        holder.iv_device = itemView.findViewById(R.id.iv_device);
        holder.cb_dp = itemView.findViewById(R.id.cb_dp);
        holder.cb_df = itemView.findViewById(R.id.cb_df);
        holder.cb_dr = itemView.findViewById(R.id.cb_dr);
        holder.cb_dfr = itemView.findViewById(R.id.cb_dfr);
        holder.ll_forward = itemView.findViewById(R.id.ll_forward);
        holder.ll_relay = itemView.findViewById(R.id.ll_relay);
        holder.ll_proxy = itemView.findViewById(R.id.ll_proxy);
        holder.ll_friend = itemView.findViewById(R.id.ll_friend);
        return holder;
    }

    @Override
    public int getItemCount() {
        return infoList == null ? 0 : infoList.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);

        NodeInfo deviceInfo = infoList.get(position);

        final int pid = deviceInfo.compositionData != null ? deviceInfo.compositionData.pid : 0;
        holder.iv_device.setImageResource(IconGenerator.getIcon(pid, deviceInfo.getOnlineState()));
        String pidInfo = deviceInfo.getPidDesc();

        holder.tv_device_info.setText(mContext.getString(R.string.device_state_desc_mesh_ota,
                String.format("%04X", deviceInfo.meshAddress),
                pidInfo));
        holder.cb_df.setChecked(deviceInfo.directForwardingEnabled);
        holder.cb_dr.setChecked(deviceInfo.directRelay);
        holder.cb_dp.setChecked(deviceInfo.directProxyEnabled);
        holder.cb_dfr.setChecked(deviceInfo.directFriend);

        holder.ll_forward.setTag(position);
        holder.ll_forward.setOnClickListener(checkViewClick);

        holder.ll_relay.setTag(position);
        holder.ll_relay.setOnClickListener(checkViewClick);

        holder.ll_proxy.setTag(position);
        holder.ll_proxy.setOnClickListener(checkViewClick);

        holder.ll_friend.setTag(position);
        holder.ll_friend.setOnClickListener(checkViewClick);
    }

    private View.OnClickListener checkViewClick = v -> {
        int position = (int) v.getTag();
        NodeInfo nodeInfo = infoList.get(position);
        DirectedControlSetMessage setMessage = new DirectedControlSetMessage(nodeInfo.meshAddress);
        if (v.getId() == R.id.ll_forward) {
            setMessage.directedForwarding = (byte) (nodeInfo.directForwardingEnabled ? 0 : 1);//  reverse, changed current state
            setMessage.directedRelay = 0;
            setMessage.setDirectedProxy((byte) 0);
            setMessage.directedFriend = (byte) 0xFF;
            setMessage.setDirectedProxyUseDirectedDefault((byte) 0xFF);
            if (setMessage.directedForwarding == 1) {
                // from disable
            } else {
                // from enable

            }
        } else if (v.getId() == R.id.ll_relay) {
            if (!nodeInfo.directForwardingEnabled) {
                Toast.makeText(mContext, "(relay)check direct forwarding first", Toast.LENGTH_SHORT).show();
                return;
            }
            setMessage.directedForwarding = 1; // get current state
            setMessage.directedRelay = (byte) (nodeInfo.directRelay ? 0 : 1); //  reverse, changed current state
            setMessage.setDirectedProxy((byte) (nodeInfo.directProxyEnabled ? 1 : 0));
            setMessage.directedFriend = (byte) (nodeInfo.directFriend ? 1 : 0);

        } else if (v.getId() == R.id.ll_proxy) {
            if (!nodeInfo.directForwardingEnabled) {
                Toast.makeText(mContext, "(proxy)check direct forwarding first", Toast.LENGTH_SHORT).show();
                return;
            }
            setMessage.directedForwarding = 1;
            setMessage.setDirectedProxy((byte) (nodeInfo.directProxyEnabled ? 0 : 1));
            setMessage.directedRelay = (byte) (nodeInfo.directRelay ? 1 : 0);
            setMessage.directedFriend = (byte) (nodeInfo.directFriend ? 1 : 0);
        } else if (v.getId() == R.id.ll_friend) {
            if (!nodeInfo.directForwardingEnabled) {
                Toast.makeText(mContext, "(friend)check direct forwarding first", Toast.LENGTH_SHORT).show();
                return;
            }
            setMessage.directedForwarding = 1;
            setMessage.directedFriend = (byte) (nodeInfo.directFriend ? 0 : 1);
            setMessage.setDirectedProxy((byte) (nodeInfo.directProxyEnabled ? 1 : 0));
            setMessage.directedRelay = (byte) (nodeInfo.directRelay ? 1 : 0);
        }

        MeshService.getInstance().sendMeshMessage(setMessage);
    };

    class ViewHolder extends RecyclerView.ViewHolder {
        ImageView iv_device;
        TextView tv_device_info;
        CheckBox cb_df, cb_dr, cb_dp, cb_dfr;
        View ll_forward, ll_relay, ll_proxy, ll_friend;


        public ViewHolder(View itemView) {
            super(itemView);
        }
    }


}
