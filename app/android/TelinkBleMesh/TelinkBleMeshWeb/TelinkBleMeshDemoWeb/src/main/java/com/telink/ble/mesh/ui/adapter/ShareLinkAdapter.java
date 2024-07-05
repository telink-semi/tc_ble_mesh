/********************************************************************************************************
 * @file ShareLinkAdapter.java
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
import com.telink.ble.mesh.model.web.MeshNetworkShareLink;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Locale;

/**
 * 用户创建的分享链接列表
 */
public class ShareLinkAdapter extends BaseRecyclerViewAdapter<ShareLinkAdapter.ViewHolder> {

    private SimpleDateFormat mDateFormat = new SimpleDateFormat("MM-dd HH:mm:ss", Locale.getDefault());
    private Context mContext;
    private List<MeshNetworkShareLink> shareLinks;
    private ItemMenuClickListener menuClickListener;


    public ShareLinkAdapter(Context context, List<MeshNetworkShareLink> shareLinks, ItemMenuClickListener menuClickListener) {
        this.mContext = context;
        this.shareLinks = shareLinks;
        this.menuClickListener = menuClickListener;
    }

    public void addShareLink(MeshNetworkShareLink link) {
        if (this.shareLinks == null) {
            this.shareLinks = new ArrayList<>();
        }
        this.shareLinks.add(link);
        this.notifyDataSetChanged();
    }

    public void removeShareLink(int position) {
        if (this.shareLinks == null) {
            return;
        }
        this.shareLinks.remove(position);
        this.notifyDataSetChanged();
    }

    public void setShareLinks(List<MeshNetworkShareLink> shareLinks) {
        this.shareLinks = shareLinks;
        this.notifyDataSetChanged();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_share_link, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.iv_link = itemView.findViewById(R.id.iv_link);
        holder.iv_menu = itemView.findViewById(R.id.iv_menu);
        holder.tv_count = itemView.findViewById(R.id.tv_count);
        holder.tv_create_time = itemView.findViewById(R.id.tv_create_time);
        holder.tv_end_time = itemView.findViewById(R.id.tv_end_time);
        return holder;
    }

    @Override
    public int getItemCount() {
        return shareLinks == null ? 0 : shareLinks.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        MeshNetworkShareLink shareLink = shareLinks.get(position);
        String createTime = mDateFormat.format(new Date(shareLink.getCreateTime()));
        String endTime = mDateFormat.format(new Date(shareLink.getEndTime()));
        holder.tv_count.setText(shareLink.getCurrentCount() + "/" + shareLink.getMaxCount());
        holder.tv_create_time.setText(createTime);
        holder.tv_end_time.setText(endTime);
        holder.iv_menu.setOnClickListener(v -> menuClickListener.onMenuClick(position));
    }


    class ViewHolder extends RecyclerView.ViewHolder {
        ImageView iv_link, iv_menu;
        TextView tv_count, tv_create_time, tv_end_time;


        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
