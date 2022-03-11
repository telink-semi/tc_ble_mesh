/********************************************************************************************************
 * @file RemotePublishListAdapter.java
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

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.ui.fragment.RemoteControlFragment;

/**
 * set remote publication
 */
public class RemotePublishListAdapter extends BaseRecyclerViewAdapter<RemotePublishListAdapter.ViewHolder> {

    RemoteControlFragment fragment;

    NodeInfo nodeInfo;

    public RemotePublishListAdapter(RemoteControlFragment fragment, NodeInfo nodeInfo) {
        this.fragment = fragment;
        this.nodeInfo = nodeInfo;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(fragment.getActivity()).inflate(R.layout.item_remote_pub_set, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.btn_send = itemView.findViewById(R.id.btn_send);
        holder.et_pub_adr = itemView.findViewById(R.id.et_pub_adr);
        holder.et_mdl_id = itemView.findViewById(R.id.et_mdl_id);
        holder.et_ele_adr = itemView.findViewById(R.id.et_ele_adr);
        return holder;
    }

    @Override
    public int getItemCount() {
        return nodeInfo.compositionData.elements.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        CompositionData.Element element = nodeInfo.compositionData.elements.get(position);
        int eleAdr = nodeInfo.meshAddress + position;
        holder.et_ele_adr.setText(String.format("%04X", eleAdr));


        if (element.sigModels.size() > 0) {
            int modelId = 0x1001; // on/off client model
//            modelId = element.sigModels.get(0);
            holder.et_mdl_id.setText(String.format("%04X", modelId));
        } else {
            holder.et_mdl_id.setText("no model in element");
        }

        holder.et_pub_adr.setText(String.format("%04X", 0xC000 + position));
        holder.btn_send.setTag(position);
        holder.btn_send.setOnClickListener(sendClick);
    }


    private View.OnClickListener sendClick = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            int position = (int) v.getTag();
            fragment.sendPubMsg(position);
        }
    };


    public static class ViewHolder extends RecyclerView.ViewHolder {

        public Button btn_send;
        public EditText et_pub_adr, et_mdl_id, et_ele_adr;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
