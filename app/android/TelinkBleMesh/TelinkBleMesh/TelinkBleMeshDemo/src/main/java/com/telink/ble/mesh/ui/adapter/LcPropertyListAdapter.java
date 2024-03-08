/********************************************************************************************************
 * @file LcPropertyListAdapter.java
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
import android.graphics.drawable.Drawable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.LightingControlActivity;

import java.util.List;

/**
 * provision list adapter
 * Created by Administrator on 2016/10/25.
 */
public class LcPropertyListAdapter extends BaseRecyclerViewAdapter<LcPropertyListAdapter.ViewHolder> {
    private List<LightingControlActivity.LcPropItem> propertyList;
    private Context mContext;

    public LcPropertyListAdapter(Context context, List<LightingControlActivity.LcPropItem> configList) {
        mContext = context;
        this.propertyList = configList;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_lc_property, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_lc_prop_name = itemView.findViewById(R.id.tv_lc_prop_name);
        holder.tv_prop_def = itemView.findViewById(R.id.tv_prop_def);
        holder.tv_prop_val = itemView.findViewById(R.id.tv_prop_val);
        holder.btn_get = itemView.findViewById(R.id.btn_get);
        holder.btn_set = itemView.findViewById(R.id.btn_set);
        holder.ll_detail = itemView.findViewById(R.id.ll_detail);
        holder.iv_expand = itemView.findViewById(R.id.iv_expand);
        holder.ll_expand = itemView.findViewById(R.id.ll_expand);
        return holder;
    }

    @Override
    public int getItemCount() {
        return propertyList == null ? 0 : propertyList.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        LightingControlActivity.LcPropItem lcProp = propertyList.get(position);
        holder.tv_lc_prop_name.setText(lcProp.property.name);
        String def = lcProp.property.def + "\n" + "Characteristic: " + lcProp.property.characteristic;
        holder.tv_prop_def.setText(def);
        if (lcProp.value == -1) {
            holder.tv_prop_val.setText("value: UNKNOWN");
        } else {
            holder.tv_prop_val.setText("value: " + lcProp.value);
        }
//
        holder.ll_expand.setTag(position);
        Drawable drawable = lcProp.expanded ? mContext.getResources().getDrawable(R.drawable.ic_arrow_down) : mContext.getResources().getDrawable(R.drawable.ic_arrow_right);
        holder.iv_expand.setImageDrawable(drawable);
        holder.ll_expand.setOnClickListener(clickListener);
        holder.ll_detail.setVisibility(lcProp.expanded ? View.VISIBLE : View.GONE);

        holder.btn_get.setVisibility(View.VISIBLE);
        holder.btn_get.setTag(position);
        holder.btn_get.setOnClickListener(clickListener);

        holder.btn_set.setVisibility(View.VISIBLE);
        holder.btn_set.setTag(position);
        holder.btn_set.setOnClickListener(clickListener);

    }

    private View.OnClickListener clickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            int position = (int) v.getTag();
            if (v.getId() == R.id.btn_get) {
                ((LightingControlActivity) mContext).onGetTap(position);
            } else if (v.getId() == R.id.btn_set) {
                ((LightingControlActivity) mContext).onSetTap(position);
            } else if (v.getId() == R.id.ll_expand) {
                propertyList.get(position).expanded = !propertyList.get(position).expanded;
                notifyDataSetChanged();
            }
        }
    };


    class ViewHolder extends RecyclerView.ViewHolder {
        public TextView tv_lc_prop_name,
                tv_prop_def, // include characteristic
                tv_prop_val;
        public Button btn_get, btn_set;
        public View ll_detail, ll_expand;
        public ImageView iv_expand;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
