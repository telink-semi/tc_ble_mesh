/********************************************************************************************************
 * @file SwitchListAdapter.java
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
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.generic.OnOffSetMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.web.entity.MeshNetworkDetail;
import com.telink.ble.mesh.web.entity.MeshNode;

import java.util.List;

/**
 * on/off switch
 * Created by Administrator on 2016/10/25.
 */
public class SwitchListAdapter extends BaseRecyclerViewAdapter<SwitchListAdapter.ViewHolder> {


    List<Integer> mAdrList;
    Context mContext;
    MeshNode mNode;
    int address;

    public SwitchListAdapter(Context context, List<Integer> adrList, MeshNode nodeInfo) {
        mContext = context;
        mAdrList = adrList;
        this.mNode = nodeInfo;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_switch, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_ele = itemView.findViewById(R.id.tv_ele);
        holder.switch_ele = itemView.findViewById(R.id.switch_ele);
        return holder;
    }

    @Override
    public int getItemCount() {
        return mAdrList == null ? 0 : mAdrList.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        int adr = mAdrList.get(position);
        holder.tv_ele.setText("ele adr: " + adr);
        holder.switch_ele.setTag(adr);
        holder.switch_ele.setOnCheckedChangeListener(switchChangeListener);
    }

    private CompoundButton.OnCheckedChangeListener switchChangeListener = (buttonView, isChecked) -> {
        int adr = (int) buttonView.getTag();
        boolean ack = !AppSettings.ONLINE_STATUS_ENABLE;
        int rspMax = ack ? 1 : 0;
        int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
        OnOffSetMessage message = OnOffSetMessage.getSimple(adr, appKeyIndex, (byte) (isChecked ? 1 : 0), ack, rspMax);
        MeshService.getInstance().sendMeshMessage(message);
    };


    class ViewHolder extends RecyclerView.ViewHolder {

        private TextView tv_ele;
        private Switch switch_ele;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
