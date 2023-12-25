/********************************************************************************************************
 * @file DeviceSelectAdapter.java
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
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.cardview.widget.CardView;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.activity.IconGenerator;
import com.telink.ble.mesh.web.entity.MeshNode;

import java.util.ArrayList;
import java.util.List;

/**
 * device in scene
 */
public class SceneDeviceAdapter extends BaseRecyclerViewAdapter<SceneDeviceAdapter.ViewHolder> {

    private Context mContext;

    // all nodes in mesh network
    private List<MeshNode> allNodes;

    private Callback cb;

    public SceneDeviceAdapter(Context context, List<MeshNode> allNodes, Callback cb) {
        this.mContext = context;
        this.allNodes = allNodes;
        this.cb = cb;
    }

    public void updateInnerNodes(List<MeshNode> innerNodes) {
        for (MeshNode node : allNodes) {
            if (innerNodes.contains(node)) {
                node.selected = true;
            }
        }
        notifyDataSetChanged();
    }

    public void updateNodeState(MeshNode node) {
        /*boolean selected = node.selected;
        if (selected) {
            selectedNodes.add(node);
            unselectedNodes.remove(node);
        } else {
            selectedNodes.remove(node);
            unselectedNodes.add(node);
        }*/
        notifyDataSetChanged();
    }


    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_scene_device, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.iv_device = itemView.findViewById(R.id.iv_device);
        holder.tv_device_info = itemView.findViewById(R.id.tv_device_info);
        holder.btn_action = itemView.findViewById(R.id.btn_action);
        holder.cv_root = itemView.findViewById(R.id.cv_root);
        return holder;
    }

    @Override
    public int getItemCount() {
        return allNodes.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        MeshNode node = allNodes.get(position);

        final int pid = node.compositionData() != null ? node.compositionData().pid : 0;
        holder.iv_device.setImageResource(IconGenerator.getIcon(pid, node.getOnlineState()));
        holder.tv_device_info.setText(mContext.getString(R.string.device_state_desc,
                String.format("%04X", node.address),
                node.getOnlineState().toString()));
        if (node.getTargetEleAdr(MeshSigModel.SIG_MD_SCENE_S.modelId) == -1) {
            holder.tv_device_info.append("not support");
        }

        if (node.selected) {
            holder.btn_action.setText("remove");
            holder.cv_root.setForeground(mContext.getDrawable(R.drawable.bg_card_selected));
        } else {
            holder.btn_action.setText("add");
            holder.cv_root.setForeground(null);
        }

        if (!node.isOffline()) {
            holder.btn_action.setEnabled(true);
            holder.btn_action.setOnClickListener(v -> cb.onNodeSceneStateChanged(node));
        } else {
            holder.btn_action.setEnabled(false);
        }
    }

    class ViewHolder extends RecyclerView.ViewHolder {
        FrameLayout cv_root;
        ImageView iv_device;
        TextView tv_device_info;
        Button btn_action;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }

    public interface Callback {
        void onNodeSceneStateChanged(MeshNode node);
    }
}
