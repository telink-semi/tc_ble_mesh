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

import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.Toast;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.MeshAppKey;
import com.telink.ble.mesh.util.Arrays;

import java.util.List;

import androidx.recyclerview.widget.RecyclerView;

public class AppKeyInfoAdapter extends BaseRecyclerViewAdapter<AppKeyInfoAdapter.ViewHolder> {

    private List<MeshAppKey> keyList;
    private Context mContext;

    public AppKeyInfoAdapter(Context context, List<MeshAppKey> keyList) {
        mContext = context;
        this.keyList = keyList;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_app_key_info, parent, false);
        ViewHolder holder = new ViewHolder(itemView);

        holder.tv_key_name = itemView.findViewById(R.id.tv_key_name);
        holder.tv_key_index = itemView.findViewById(R.id.tv_key_index);
        holder.tv_key_val = itemView.findViewById(R.id.tv_key_val);
        holder.ll_key_val = itemView.findViewById(R.id.ll_key_val);
        holder.tv_bound = itemView.findViewById(R.id.tv_bound);
        return holder;
    }

    @Override
    public int getItemCount() {
        return keyList == null ? 0 : keyList.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        MeshAppKey appKey = keyList.get(position);
        holder.tv_key_name.setText(appKey.getName());
        holder.tv_key_index.setText(String.format("0x%02X", appKey.getIndex()));
        holder.tv_key_val.setText(Arrays.bytesToHexString(appKey.getKey()));
        holder.ll_key_val.setTag(position);
        holder.ll_key_val.setOnLongClickListener(keyLongClick);
        holder.tv_bound.setText(String.format("0x%02X", appKey.boundNetKeyIndex));
    }

    private View.OnLongClickListener keyLongClick = new View.OnLongClickListener() {
        @Override
        public boolean onLongClick(View v) {
            int position = (int) v.getTag();
            String keyStr = Arrays.bytesToHexString(keyList.get(position).getKey());
            if (copyTextToClipboard(keyStr)) {
                Toast.makeText(mContext, "key copied", Toast.LENGTH_SHORT).show();
            }
            return false;
        }
    };

    private boolean copyTextToClipboard(String text) {
        ClipboardManager clipboard = (ClipboardManager) mContext.getSystemService(Context.CLIPBOARD_SERVICE);
        ClipData clip = ClipData.newPlainText("com.telink.bluetooth.light", text);
        if (clipboard != null) {
            clipboard.setPrimaryClip(clip);
            return true;
        } else {
            return false;
        }
    }


    static class ViewHolder extends RecyclerView.ViewHolder {


        private TextView tv_key_name, tv_key_index, tv_key_val, tv_bound;
        private View ll_key_val;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
