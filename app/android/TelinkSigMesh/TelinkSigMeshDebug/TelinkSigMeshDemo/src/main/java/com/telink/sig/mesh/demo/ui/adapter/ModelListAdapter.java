/********************************************************************************************************
 * @file     ModelListAdapter.java 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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
package com.telink.sig.mesh.demo.ui.adapter;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.model.SigMeshModel;

import java.util.List;

/**
 * models list adapter
 */
public class ModelListAdapter extends BaseRecyclerViewAdapter<ModelListAdapter.ViewHolder> {
    List<SigMeshModel> models;
    Context mContext;
    boolean selectMode = false;

    public ModelListAdapter(Context context, List<SigMeshModel> modelList) {
        mContext = context;
        models = modelList;
    }

    public void setSelectMode(boolean selectMode) {
        this.selectMode = selectMode;
        this.notifyDataSetChanged();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_setting_model, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_id = (TextView) itemView.findViewById(R.id.tv_model_id);
        holder.tv_desc = (TextView) itemView.findViewById(R.id.tv_model_desc);
        holder.iv_select = itemView.findViewById(R.id.iv_select);
        return holder;
    }

    @Override
    public int getItemCount() {
        return models == null ? 0 : models.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);

        SigMeshModel model = models.get(position);

        holder.tv_id.setText("id: 0x" + Integer.toHexString(model.modelId));
        holder.tv_desc.setText("modelName: " + model.modelName);
        if (selectMode && model.selected) {
            holder.iv_select.setVisibility(View.VISIBLE);
        } else {
            holder.iv_select.setVisibility(View.INVISIBLE);
        }

    }

    class ViewHolder extends RecyclerView.ViewHolder {


        public ImageView iv_select;
        public TextView tv_id, tv_desc;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
