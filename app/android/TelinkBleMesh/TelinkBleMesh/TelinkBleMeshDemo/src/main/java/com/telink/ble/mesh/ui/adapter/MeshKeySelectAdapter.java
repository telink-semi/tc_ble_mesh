/********************************************************************************************************
 * @file MeshKeySelectAdapter.java
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
import android.widget.CompoundButton;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.MeshKey;
import com.telink.ble.mesh.util.Arrays;

import java.util.ArrayList;
import java.util.List;

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
            selectList.add(i == 0); // select first key
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
