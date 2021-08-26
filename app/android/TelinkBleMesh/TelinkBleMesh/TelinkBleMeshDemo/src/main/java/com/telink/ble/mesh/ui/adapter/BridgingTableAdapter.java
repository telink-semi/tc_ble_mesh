/********************************************************************************************************
 * @file OnlineDeviceListAdapter.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *
 *******************************************************************************************************/
package com.telink.ble.mesh.ui.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.BridgingTable;

import java.util.List;
import java.util.Locale;

import androidx.recyclerview.widget.RecyclerView;

public class BridgingTableAdapter extends BaseRecyclerViewAdapter<BridgingTableAdapter.ViewHolder> {
    List<BridgingTable> tables;
    Context mContext;

    public BridgingTableAdapter(Context context, List<BridgingTable> tables) {
        mContext = context;
        this.tables = tables;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_bridging_table, null, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_direction = itemView.findViewById(R.id.tv_direction);
        holder.tv_index_1 = itemView.findViewById(R.id.tv_index_1);
        holder.tv_index_2 = itemView.findViewById(R.id.tv_index_2);
        holder.tv_adr_1 = itemView.findViewById(R.id.tv_adr_1);
        holder.tv_adr_2 = itemView.findViewById(R.id.tv_adr_2);
        return holder;
    }

    @Override
    public int getItemCount() {
        return tables == null ? 0 : tables.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);

        BridgingTable table = tables.get(position);
        BridgingTable.Direction direction = BridgingTable.Direction.getByValue(table.directions);
        holder.tv_direction.setText(String.format(Locale.getDefault(), "%d(%s)", direction.value, direction.desc));
        holder.tv_index_1.setText(String.format("%04X", table.netKeyIndex1));
        holder.tv_index_2.setText(String.format("%04X", table.netKeyIndex2));
        holder.tv_adr_1.setText(String.format("%04X", table.address1));
        holder.tv_adr_2.setText(String.format("%04X", table.address2));
    }

    static class ViewHolder extends RecyclerView.ViewHolder {
        public TextView tv_direction, tv_index_1, tv_index_2, tv_adr_1, tv_adr_2;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
