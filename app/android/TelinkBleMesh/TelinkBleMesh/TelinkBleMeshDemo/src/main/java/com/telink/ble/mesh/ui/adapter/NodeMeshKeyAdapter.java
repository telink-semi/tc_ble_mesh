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
import android.widget.ImageView;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.MeshKey;
import com.telink.ble.mesh.model.MeshNetKey;
import com.telink.ble.mesh.ui.NodeNetKeyActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.List;

import androidx.recyclerview.widget.RecyclerView;

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
            ((NodeNetKeyActivity)mContext).onNetKeySelect((MeshNetKey) keyList.get(position), NodeNetKeyActivity.ACTION_DELETE);
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
