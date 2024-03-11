/********************************************************************************************************
 * @file NodeMeshKeyAdapter.java
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
import com.telink.ble.mesh.model.MeshKey;
import com.telink.ble.mesh.model.MeshNetKey;
import com.telink.ble.mesh.ui.NodeNetKeyActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.List;

/**
 * scheduler list
 * Created by Administrator on 2016/10/25.
 */
public class NodeMeshKeyAdapter<T extends MeshKey> extends BaseRecyclerViewAdapter<NodeMeshKeyAdapter.ViewHolder> {

    private List<T> keyList;
    private Context mContext;
    private boolean editMode;

    public NodeMeshKeyAdapter(Context context, List<T> keyList, boolean editMode) {
        mContext = context;
        this.keyList = keyList;
        this.editMode = editMode;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_node_net_key, parent, false);
        ViewHolder holder = new ViewHolder(itemView);

        holder.tv_key_name = itemView.findViewById(R.id.tv_key_name);
        holder.tv_key_index = itemView.findViewById(R.id.tv_key_index);
        holder.tv_key_val = itemView.findViewById(R.id.tv_key_val);
        holder.iv_delete = itemView.findViewById(R.id.iv_delete);

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
        holder.tv_key_index.setText(netKey.getIndex() + "");
        holder.tv_key_val.setText(Arrays.bytesToHexString(netKey.getKey()));
        if (!editMode || keyList.size() == 0 || netKey.getIndex() == 0) {
            holder.iv_delete.setVisibility(View.GONE);
        } else {
            holder.iv_delete.setVisibility(View.GONE);
//            holder.iv_delete.setVisibility(View.VISIBLE);
//            holder.iv_delete.setTag(position);
//            holder.iv_delete.setOnClickListener(removeClick);
        }
    }

    private View.OnClickListener removeClick = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            int position = (int) v.getTag();
            MeshLogger.d("remove node net key");
            ((NodeNetKeyActivity) mContext).onNetKeySelect((MeshNetKey) keyList.get(position), NodeNetKeyActivity.ACTION_DELETE);
        }
    };

    static class ViewHolder extends RecyclerView.ViewHolder {


        private TextView tv_key_name, tv_key_index, tv_key_val;
        private ImageView iv_delete;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
