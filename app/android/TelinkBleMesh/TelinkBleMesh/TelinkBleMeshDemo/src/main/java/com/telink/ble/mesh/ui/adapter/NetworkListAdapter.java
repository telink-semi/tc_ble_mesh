/********************************************************************************************************
 * @file AppKeyInfoAdapter.java
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
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.MeshInfo;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

public class NetworkListAdapter extends BaseRecyclerViewAdapter<NetworkListAdapter.ViewHolder> {
    private SimpleDateFormat mDateFormat = new SimpleDateFormat("MM-dd HH:mm:ss", Locale.getDefault());
    private List<MeshInfo> networkList;
    private Context mContext;
    private ItemMenuClickListener menuClickListener;
    private long curMeshId;

    public NetworkListAdapter(Context context, ItemMenuClickListener menuClickListener) {
        this.mContext = context;
        this.menuClickListener = menuClickListener;
        this.curMeshId = TelinkMeshApplication.getInstance().getMeshInfo().id;
    }

    public MeshInfo get(int position) {
        if (this.networkList == null) {
            return null;
        }
        return this.networkList.get(position);
    }

    public void resetCurMeshId() {
        this.curMeshId = TelinkMeshApplication.getInstance().getMeshInfo().id;
        this.notifyDataSetChanged();
    }


    public void add(MeshInfo network) {
        if (this.networkList == null) {
            networkList = new ArrayList<>();
        }
        this.networkList.add(network);
        this.notifyDataSetChanged();
    }

    public long getCurMeshId() {
        return curMeshId;
    }

    public void remove(int position) {
        if (this.networkList == null) {
            return;
        }
        this.networkList.remove(position);
        this.notifyDataSetChanged();
    }

    public boolean isCurrentMesh(int position) {
        return this.networkList.get(position).id == curMeshId;
    }


    public void resetData(List<MeshInfo> networkList) {
        this.networkList = networkList;
        this.notifyDataSetChanged();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_network, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_name = itemView.findViewById(R.id.tv_name);
        holder.tv_uuid = itemView.findViewById(R.id.tv_uuid);
//        holder.tv_create_user = itemView.findViewById(R.id.tv_create_user);
        holder.tv_time = itemView.findViewById(R.id.tv_time);
        holder.iv_menu = itemView.findViewById(R.id.iv_menu);
        holder.layout_root = itemView.findViewById(R.id.layout_root);
        return holder;
    }

    @Override
    public int getItemCount() {
        return networkList == null ? 0 : networkList.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        MeshInfo network = networkList.get(position);

        holder.tv_name.setText(network.meshName);
        holder.tv_uuid.setText(network.meshUUID);
//        holder.tv_create_user.setText(network.getCreateUserId() + "");
//        String createTime = mDateFormat.format(new Date(network.getCreateTime()));
        holder.tv_time.setText(network.timestamp);
        holder.iv_menu.setOnClickListener(v -> menuClickListener.onMenuClick(position));
        if (curMeshId == network.id) {
            holder.layout_root.setForeground(mContext.getDrawable(R.drawable.bg_card_selected));
        } else {
            holder.layout_root.setForeground(null);
        }
    }

    public static class ViewHolder extends RecyclerView.ViewHolder {

        private TextView tv_name, tv_uuid, tv_time;
        private ImageView iv_menu;
        private FrameLayout layout_root;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }


}
