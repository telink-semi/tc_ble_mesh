/********************************************************************************************************
 * @file NetKeyInfoAdapter.java
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

import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.Toast;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.MeshNetKey;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.web.entity.MeshNetworkKey;

import java.util.List;

public class NetKeyInfoAdapter extends BaseRecyclerViewAdapter<NetKeyInfoAdapter.ViewHolder> {

    private List<MeshNetworkKey> keyList;
    private Context mContext;

    public NetKeyInfoAdapter(Context context, List<MeshNetworkKey> keyList) {
        mContext = context;
        this.keyList = keyList;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_net_key_info, parent, false);
        ViewHolder holder = new ViewHolder(itemView);

        holder.tv_key_name = itemView.findViewById(R.id.tv_key_name);
        holder.tv_key_index = itemView.findViewById(R.id.tv_key_index);
        holder.tv_key_val = itemView.findViewById(R.id.tv_key_val);
        holder.ll_key_val = itemView.findViewById(R.id.ll_key_val);
        return holder;
    }

    @Override
    public int getItemCount() {
        return keyList == null ? 0 : keyList.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        MeshNetworkKey netKey = keyList.get(position);
        holder.tv_key_name.setText(netKey.getName());
        holder.tv_key_index.setText(String.format("0x%02X", netKey.getIndex()));
        holder.tv_key_val.setText(netKey.getKey());
        holder.ll_key_val.setTag(position);
        holder.ll_key_val.setOnLongClickListener(keyLongClick);
    }

    private View.OnLongClickListener keyLongClick = new View.OnLongClickListener() {
        @Override
        public boolean onLongClick(View v) {
            int position = (int) v.getTag();
            String keyStr = keyList.get(position).getKey();
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


        private TextView tv_key_name, tv_key_index, tv_key_val;
        private View ll_key_val;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
