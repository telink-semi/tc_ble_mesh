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

import androidx.appcompat.app.AlertDialog;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.model.GroupInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.ui.fragment.RemoteControlFragment;

import java.util.List;

/**
 * set remote publication
 */
public class RemotePublishListAdapter extends BaseRecyclerViewAdapter<RemotePublishListAdapter.ViewHolder> {

    RemoteControlFragment fragment;

    NodeInfo nodeInfo;

    private String[] defaultGroups;

    private String[] extendGroups;

    private boolean isLevelServiceEnable;

    public RemotePublishListAdapter(RemoteControlFragment fragment, NodeInfo nodeInfo) {
        this.fragment = fragment;
        this.nodeInfo = nodeInfo;
        List<GroupInfo> gs1 = TelinkMeshApplication.getInstance().getMeshInfo().groups;
        defaultGroups = new String[gs1.size()];
        for (int i = 0; i < gs1.size(); i++) {
            defaultGroups[i] = String.format("%s(0x%04X)", gs1.get(i).name, gs1.get(i).address);
        }
        List<GroupInfo> gs2 = TelinkMeshApplication.getInstance().getMeshInfo().extendGroups;
        extendGroups = new String[gs2.size()];
        for (int i = 0; i < gs2.size(); i++) {
            extendGroups[i] = String.format("%s(0x%04X)", gs2.get(i).name, gs2.get(i).address);
        }

        isLevelServiceEnable = SharedPreferenceHelper.isLevelServiceEnable(fragment.getActivity());
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(fragment.getActivity()).inflate(R.layout.item_remote_pub_set, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.btn_send = itemView.findViewById(R.id.btn_send);
        holder.et_pub_adr = itemView.findViewById(R.id.et_pub_adr);
        holder.et_mdl_id = itemView.findViewById(R.id.et_mdl_id);
        holder.et_ele_adr = itemView.findViewById(R.id.et_ele_adr);
        holder.btn_address = itemView.findViewById(R.id.btn_address);
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
        holder.btn_address.setOnClickListener(v -> showAddressSelectDialog(holder, position));
    }

    private void showAddressSelectDialog(ViewHolder holder, int position) {
        String modelIdInput = holder.et_mdl_id.getText().toString();
        String[] items;
        boolean isLevelModel = modelIdInput.equals("1002");
        if (isLevelModel && isLevelServiceEnable) {
            items = extendGroups;
        } else {
            items = defaultGroups;
        }
        AlertDialog.Builder builder = new AlertDialog.Builder(fragment.getActivity());
        builder.setItems(items, (dialog, which) -> {
            if (isLevelModel) {
                holder.et_pub_adr.setText(String.format("%04X", TelinkMeshApplication.getInstance().getMeshInfo().extendGroups.get(which).address));
//                MeshLogger.d("showAddressSelectDialog 3# item select " + position + " -- " + which);
            } else {
                holder.et_pub_adr.setText(String.format("%04X", TelinkMeshApplication.getInstance().getMeshInfo().groups.get(which).address));
            }
        });
        builder.show();
    }


    private View.OnClickListener sendClick = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            int position = (int) v.getTag();
            fragment.sendPubMsg(position);
        }
    };


    public static class ViewHolder extends RecyclerView.ViewHolder {

        public Button btn_send, btn_address;
        public EditText et_pub_adr, et_mdl_id, et_ele_adr;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
