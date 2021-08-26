/********************************************************************************************************
 * @file SchedulerListAdapter.java
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
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.RadioButton;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.MeshKey;
import com.telink.ble.mesh.util.Arrays;

import java.util.ArrayList;
import java.util.List;

import androidx.recyclerview.widget.RecyclerView;

/**
 * scheduler list
 * Created by Administrator on 2016/10/25.
 */
public class MeshKeySelectAdapter<T extends MeshKey> extends BaseRecyclerViewAdapter<MeshKeySelectAdapter.ViewHolder> {

    private List<T> keyList;
    private Context mContext;
    private List<Boolean> selectList;

    public MeshKeySelectAdapter(Context context, List<T> keyList) {
        mContext = context;
        this.keyList = keyList;
        selectList = new ArrayList<>();
        for (int i = 0; i < keyList.size(); i++) {
            selectList.add(false);
        }
    }

    public List<Boolean> getSelectList() {
        return selectList;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_mesh_key_select, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_key_name = itemView.findViewById(R.id.tv_key_name);
        holder.tv_key_index = itemView.findViewById(R.id.tv_key_index);
        holder.tv_key_val = itemView.findViewById(R.id.tv_key_val);
        holder.cb_net_key = itemView.findViewById(R.id.cb_net_key);
        return holder;
    }

    @Override
    public int getItemCount() {
        return keyList == null ? 0 : keyList.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        MeshKey netKey = keyList.get(position);
        holder.tv_key_name.setText(netKey.getName());
        holder.tv_key_index.setText(String.format("0x%02X", netKey.getIndex()));
        holder.tv_key_val.setText(Arrays.bytesToHexString(netKey.getKey()));
        holder.cb_net_key.setChecked(selectList.get(position));
        holder.cb_net_key.setTag(position);
        holder.cb_net_key.setOnCheckedChangeListener(onSelectChanged);
    }

    private CompoundButton.OnCheckedChangeListener onSelectChanged = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            int position = (int) buttonView.getTag();
            selectList.set(position, isChecked);
        }
    };


    static class ViewHolder extends RecyclerView.ViewHolder {
        private TextView tv_key_name, tv_key_index, tv_key_val;
        private CheckBox cb_net_key;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
