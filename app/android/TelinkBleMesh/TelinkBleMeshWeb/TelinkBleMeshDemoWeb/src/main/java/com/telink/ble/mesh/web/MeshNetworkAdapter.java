/********************************************************************************************************
 * @file MeshNetworkAdapter.java
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
package com.telink.ble.mesh.web;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.adapter.BaseRecyclerViewAdapter;
import com.telink.ble.mesh.ui.adapter.ItemMenuClickListener;
import com.telink.ble.mesh.web.entity.MeshNetwork;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Locale;

public class MeshNetworkAdapter extends BaseRecyclerViewAdapter<MeshNetworkAdapter.ViewHolder> {
    private SimpleDateFormat mDateFormat = new SimpleDateFormat("MM-dd HH:mm:ss", Locale.getDefault());
    private List<MeshNetwork> networkList;
    private Context mContext;
    private ItemMenuClickListener menuClickListener;

    public MeshNetworkAdapter(Context context, ItemMenuClickListener menuClickListener) {
        this.mContext = context;
        this.menuClickListener = menuClickListener;
    }

    public MeshNetwork get(int position) {
        if (this.networkList == null) {
            return null;
        }
        return this.networkList.get(position);
    }


    public void add(MeshNetwork network) {
        if (this.networkList == null) {
            networkList = new ArrayList<>();
        }
        this.networkList.add(network);
        this.notifyDataSetChanged();
    }


    public void remove(int position) {
        if (this.networkList == null) {
            return;
        }
        this.networkList.remove(position);
        this.notifyDataSetChanged();
    }


    public void resetData(List<MeshNetwork> networkList) {
        this.networkList = networkList;
        this.notifyDataSetChanged();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_mesh_network, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_name = itemView.findViewById(R.id.tv_name);
        holder.tv_uuid = itemView.findViewById(R.id.tv_uuid);
        holder.tv_create_user = itemView.findViewById(R.id.tv_create_user);
        holder.tv_time = itemView.findViewById(R.id.tv_time);
        holder.iv_menu = itemView.findViewById(R.id.iv_menu);
        return holder;
    }

    @Override
    public int getItemCount() {
        return networkList == null ? 0 : networkList.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        MeshNetwork network = networkList.get(position);

        holder.tv_name.setText(network.getName());
        holder.tv_uuid.setText(network.getUuid());
        holder.tv_create_user.setText(network.getCreateUserId() + "");
        String createTime = mDateFormat.format(new Date(network.getCreateTime()));
        holder.tv_time.setText(createTime);
//        holder.iv_menu.setTag(position);
        holder.iv_menu.setOnClickListener(v -> menuClickListener.onMenuClick(position));
    }

    public static class ViewHolder extends RecyclerView.ViewHolder {

        private TextView tv_name, tv_uuid, tv_create_user, tv_time;
        private ImageView iv_menu;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }


}
