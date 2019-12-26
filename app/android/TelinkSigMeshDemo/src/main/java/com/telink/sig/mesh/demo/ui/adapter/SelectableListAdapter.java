/********************************************************************************************************
 * @file     SelectableListAdapter.java 
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
import android.widget.CheckBox;
import android.widget.CompoundButton;

import com.telink.sig.mesh.demo.R;

/**
 * beans
 */
public class SelectableListAdapter extends BaseSelectableListAdapter<SelectableListAdapter.ViewHolder> {
    SelectableBean[] beans;
    Context mContext;

    public SelectableListAdapter(Context context, SelectableBean[] beans) {
        this.mContext = context;
        this.beans = beans;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_selectable, null, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.cb_month = itemView.findViewById(R.id.cb_month);
        return holder;
    }

    @Override
    public int getItemCount() {
        return beans == null ? 0 : beans.length;
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        SelectableBean month = beans[position];
        holder.cb_month.setText(month.name);
        holder.cb_month.setChecked(month.selected);
        holder.cb_month.setTag(position);
        holder.cb_month.setOnCheckedChangeListener(checkedChangeListener);
    }

    public boolean allSelected() {
        for (SelectableBean month : beans) {
            if (!month.selected) {
                return false;
            }
        }
        return true;
    }

    public void setAll(boolean selected) {
        for (SelectableBean month : beans) {
            month.selected = selected;
        }
        notifyDataSetChanged();
    }

    public int getBinaryResult() {
        int result = 0b0000;
        for (int i = 0; i < beans.length; i++) {
            if (beans[i].selected) {
                result |= (0b0001 << i);
            }
        }
        return result;
    }

    private CompoundButton.OnCheckedChangeListener checkedChangeListener = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            int position = (int) buttonView.getTag();
            beans[position].selected = isChecked;
            if (statusChangedListener != null) {
                statusChangedListener.onStatusChanged(SelectableListAdapter.this);
            }
        }
    };

    class ViewHolder extends RecyclerView.ViewHolder {


        public CheckBox cb_month;


        public ViewHolder(View itemView) {
            super(itemView);
        }
    }

    public static class SelectableBean {
        public String name;
        public boolean selected;

        public SelectableBean(String name) {
            this.name = name;
            this.selected = false;
        }
    }
    
}
