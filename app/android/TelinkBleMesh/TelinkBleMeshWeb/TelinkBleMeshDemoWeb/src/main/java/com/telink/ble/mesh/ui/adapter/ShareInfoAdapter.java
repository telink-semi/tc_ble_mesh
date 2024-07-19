/********************************************************************************************************
 * @file ShareInfoAdapter.java
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

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.web.MeshNetworkShareInfo;
import com.telink.ble.mesh.model.web.NetworkShareState;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Locale;

/**
 * 创建者管理 用户申请的分享信息， 可以禁用， 修改权限等
 */
public class ShareInfoAdapter extends BaseRecyclerViewAdapter<ShareInfoAdapter.ViewHolder> {

    private SimpleDateFormat mDateFormat = new SimpleDateFormat("MM-dd HH:mm:ss", Locale.getDefault());
    private Context mContext;
    private List<MeshNetworkShareInfo> shareInfoList;
    private ItemMenuClickListener menuClickListener;


    public ShareInfoAdapter(Context context, ItemMenuClickListener menuClickListener) {
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


    public void updateState(int position, int state) {
        if (shareInfoList == null) {
            return;
        }
        shareInfoList.get(position).setState(state);
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
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_share_info_manage, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.iv_menu = itemView.findViewById(R.id.iv_menu);
        holder.tv_applicant = itemView.findViewById(R.id.tv_applicant);
        holder.tv_time = itemView.findViewById(R.id.tv_time);
        holder.tv_state = itemView.findViewById(R.id.tv_state);

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
        holder.tv_applicant.setText(shareInfo.getApplyUserName());
        holder.tv_time.setText(createTime);
        NetworkShareState state = NetworkShareState.getByState(shareInfo.getState());
        holder.tv_state.setText(state + "");
        holder.tv_state.setTextColor(mContext.getResources().getColor(NetworkShareState.getColor(shareInfo.getState())));
        holder.iv_menu.setOnClickListener(v -> menuClickListener.onMenuClick(position));
    }


    class ViewHolder extends RecyclerView.ViewHolder {
        ImageView iv_menu;
        TextView tv_applicant, tv_time, tv_state;


        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
