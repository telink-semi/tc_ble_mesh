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
import android.widget.CheckBox;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.Element;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.ui.SceneSettingActivity;

/**
 * select device element that contains scene server model
 */
public class ElementSelectAdapter extends BaseRecyclerViewAdapter<ElementSelectAdapter.ViewHolder> {

    private Context mContext;
    private NodeInfo nodeInfo;


    public ElementSelectAdapter(Context context, NodeInfo nodeInfo) {
        this.mContext = context;
        this.nodeInfo = nodeInfo;
    }

    public boolean isSelected(int position) {
        return nodeInfo.compositionData.elements.get(position).selected;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_element_select, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_element_info = itemView.findViewById(R.id.tv_element_info);
        holder.cb_element = itemView.findViewById(R.id.cb_element);
        holder.tv_not_sp = itemView.findViewById(R.id.tv_not_sp);
        holder.iv_refresh_scene = itemView.findViewById(R.id.iv_refresh_scene);
        return holder;
    }

    @Override
    public int getItemCount() {
        return nodeInfo.compositionData == null ? 0 : nodeInfo.compositionData.elements.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        Element element = nodeInfo.compositionData.elements.get(position);
        int adr = position + nodeInfo.meshAddress;

        holder.tv_element_info.setText(String.format("element: 0x%04X", adr));
        boolean support = element.containModel(MeshSigModel.SIG_MD_SCENE_S.modelId);
        holder.cb_element.setVisibility(support ? View.VISIBLE : View.GONE);

        holder.tv_not_sp.setVisibility(!support ? View.VISIBLE : View.GONE);
        holder.cb_element.setChecked(element.selected);
        if (element.selected) {
            holder.iv_refresh_scene.setVisibility(View.VISIBLE);
            holder.iv_refresh_scene.setOnClickListener(v -> {
                ((SceneSettingActivity) mContext).toastMsg("refresh scene state");
                ((SceneSettingActivity) mContext).setScene(
                        nodeInfo.meshAddress + position,
                        true,
                        nodeInfo);
            });
        } else {
            holder.iv_refresh_scene.setVisibility(View.INVISIBLE);
        }

    }

    static class ViewHolder extends RecyclerView.ViewHolder {
        TextView tv_element_info, tv_not_sp;
        CheckBox cb_element;
        ImageView iv_refresh_scene;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }


}
