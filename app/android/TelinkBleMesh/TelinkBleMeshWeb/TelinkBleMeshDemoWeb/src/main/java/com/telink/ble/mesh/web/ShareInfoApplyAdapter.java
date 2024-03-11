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
package com.telink.ble.mesh.web;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.web.MeshNetworkShareInfo;
import com.telink.ble.mesh.model.web.NetworkShareState;
import com.telink.ble.mesh.ui.adapter.BaseRecyclerViewAdapter;
import com.telink.ble.mesh.ui.adapter.ItemMenuClickListener;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Locale;

/**
 * 用户发起的申请： 状态信息
 */
public class ShareInfoApplyAdapter extends BaseRecyclerViewAdapter<ShareInfoApplyAdapter.ViewHolder> {
    private SimpleDateFormat mDateFormat = new SimpleDateFormat("MM-dd HH:mm:ss", Locale.getDefault());
    private List<MeshNetworkShareInfo> shareInfoList;
    private Context mContext;
    private ItemMenuClickListener menuClickListener;

    public ShareInfoApplyAdapter(Context context, ItemMenuClickListener menuClickListener) {
        this.mContext = context;
        this.menuClickListener = menuClickListener;
    }

    public void addInfoItem(MeshNetworkShareInfo shareInfo) {
        if (shareInfoList == null) {
            shareInfoList = new ArrayList<>();
        }
        shareInfoList.add(shareInfo);
        notifyDataSetChanged();
    }

    public void resetData(List<MeshNetworkShareInfo> shareInfoList) {
        this.shareInfoList = shareInfoList;
        notifyDataSetChanged();
    }

    public MeshNetworkShareInfo get(int position) {
        if (shareInfoList == null) return null;
        return shareInfoList.get(position);
    }

    public void remove(int position) {
        if (shareInfoList == null) return;
        shareInfoList.remove(position);
        notifyDataSetChanged();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_share_info_apply, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_network_name = itemView.findViewById(R.id.tv_network_name);
        holder.tv_creator = itemView.findViewById(R.id.tv_creator);
        holder.tv_time = itemView.findViewById(R.id.tv_time);
        holder.tv_state = itemView.findViewById(R.id.tv_state);
        holder.iv_menu = itemView.findViewById(R.id.iv_menu);
        return holder;
    }

    @Override
    public int getItemCount() {
        return shareInfoList == null ? 0 : shareInfoList.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        MeshNetworkShareInfo shareInfo = shareInfoList.get(position);
        String createTime = mDateFormat.format(new Date(shareInfo.getCreateTime()));
        holder.tv_network_name.setText(shareInfo.getNetworkName());
        holder.tv_creator.setText(shareInfo.getNetworkCreator());
        String st = NetworkShareState.getByState(shareInfo.getState()) + "";
        holder.tv_state.setText(st);
        holder.tv_state.setTextColor(mContext.getResources().getColor(NetworkShareState.getColor(shareInfo.getState())));
        holder.tv_time.setText(createTime);
        holder.iv_menu.setTag(position);
        holder.iv_menu.setOnClickListener(v -> menuClickListener.onMenuClick((int) v.getTag()));
    }

    public static class ViewHolder extends RecyclerView.ViewHolder {

        private TextView tv_network_name, tv_creator, tv_time, tv_state;
        private ImageView iv_menu;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }


}
