/********************************************************************************************************
 * @file BridgingTableAdapter.java
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
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.BridgingTable;

import java.util.List;
import java.util.Locale;

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
