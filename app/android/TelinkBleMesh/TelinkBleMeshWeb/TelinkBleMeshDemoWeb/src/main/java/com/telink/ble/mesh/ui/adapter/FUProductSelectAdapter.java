/********************************************************************************************************
 * @file FUProductSelectAdapter.java
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
import android.util.ArrayMap;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.web.entity.MeshNode;
import com.telink.ble.mesh.web.entity.MeshProductInfo;

import java.util.List;

/**
 * firmware update device list
 * Created by kee on 2017/8/18.
 */
public class FUProductSelectAdapter extends BaseRecyclerViewAdapter<FUProductSelectAdapter.ViewHolder> {

    private Context mContext;


    private ArrayMap<MeshProductInfo, List<MeshNode>> productDevMap;

    private MeshProductInfo selectedProduct = null;


    public FUProductSelectAdapter(Context context) {
        this.mContext = context;
    }

    public void resetData(ArrayMap<MeshProductInfo, List<MeshNode>> productDevMap) {
        this.productDevMap = productDevMap;
        this.notifyDataSetChanged();
    }


    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_fu_product_select, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.iv_expand = itemView.findViewById(R.id.iv_expand);
        holder.tv_product_info = itemView.findViewById(R.id.tv_product_info);
        holder.rv_device = itemView.findViewById(R.id.rv_device);
        return holder;
    }

    @Override
    public int getItemCount() {
        return productDevMap == null ? 0 : productDevMap.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        MeshProductInfo productInfo = productDevMap.keyAt(position);
        List<MeshNode> nodes = productDevMap.valueAt(position);

        holder.iv_expand.setVisibility(View.GONE);
        holder.tv_product_info.setText(String.format("Product: %s %04X", productInfo.getName(), productInfo.getPid()));

        holder.rv_device.setLayoutManager(new LinearLayoutManager(mContext));
        FUDeviceSelectAdapter adapter = new FUDeviceSelectAdapter(mContext, nodes);
        adapter.setStatusChangedListener((adapter1, position1) -> {
            if (selectedProduct == null) {
                selectedProduct = productInfo;
                return;
            }
            if (productInfo != selectedProduct) {
                selectedProduct = productInfo;
                boolean changed = false;
                for (MeshNode node : TelinkMeshApplication.getInstance().getMeshInfo().nodeList) {
                    if (node.selected && node != adapter.get(position1)) {
                        node.selected = false;
                        changed = true;
                    }
                }
                if (changed) {
                    this.notifyDataSetChanged();
                }
            }
        });
        holder.rv_device.setAdapter(adapter);
        holder.rv_device.setAdapter(new FUDeviceSelectAdapter(mContext, nodes)); // , int position
    }


    class ViewHolder extends RecyclerView.ViewHolder {
        ImageView iv_expand;
        TextView tv_product_info;
        RecyclerView rv_device;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
