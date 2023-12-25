/********************************************************************************************************
 * @file BaseRecyclerViewAdapter.java
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

import android.view.View;

import androidx.recyclerview.widget.RecyclerView;

/**
 * Created by kee on 2017/12/25.
 */

public abstract class BaseRecyclerViewAdapter<VH extends RecyclerView.ViewHolder> extends RecyclerView.Adapter<VH> implements View.OnClickListener, View.OnLongClickListener {

    private OnItemClickListener onItemClickListener;
    private OnItemLongClickListener onItemLongClickListener;

    @Override
    public void onBindViewHolder(VH holder, int position) {
        holder.itemView.setTag(position);
        holder.itemView.setOnClickListener(this);
        holder.itemView.setOnLongClickListener(this);
    }

    @Override
    public void onClick(View v) {
        if (onItemClickListener != null) {
            onItemClickListener.onItemClick((Integer) v.getTag());
        }
    }

    @Override
    public boolean onLongClick(View v) {
        if (onItemLongClickListener != null) {
            return onItemLongClickListener.onLongClick((Integer) v.getTag());
        }
        return false;
    }

    public void setOnItemClickListener(OnItemClickListener onItemClickListener) {
        this.onItemClickListener = onItemClickListener;
    }

    public void setOnItemLongClickListener(OnItemLongClickListener onItemLongClickListener) {
        this.onItemLongClickListener = onItemLongClickListener;
    }

    public interface OnItemClickListener {
        void onItemClick(int position);
    }

    public interface OnItemLongClickListener {
        boolean onLongClick(int position);
    }

    public interface OnEmptyVisibleChangedListener {
        boolean onEmptyVisibleChanged(boolean visible);
    }
}
