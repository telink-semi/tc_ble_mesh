/********************************************************************************************************
 * @file CertListAdapter.java
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
import android.graphics.PorterDuff;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.demo.R;

import java.security.cert.X509Certificate;
import java.util.List;

/**
 * oob info list
 */
public class CertListAdapter extends BaseRecyclerViewAdapter<CertListAdapter.ViewHolder> {

    private Context mContext;
    private List<X509Certificate> certificateList;
    private int rootIndex;

    public CertListAdapter(Context context, List<X509Certificate> certificateList, int rootIndex) {
        this.mContext = context;
        this.certificateList = certificateList;
        this.rootIndex = rootIndex;
    }

    public void updateRootIndex(int index) {
        this.rootIndex = index;
        notifyDataSetChanged();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_cert_info, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_cert_info = itemView.findViewById(R.id.tv_cert_info);
        holder.iv_next = itemView.findViewById(R.id.iv_next);
        holder.iv_cert = itemView.findViewById(R.id.iv_cert);
        return holder;
    }

    @Override
    public int getItemCount() {
        return certificateList == null ? 0 : certificateList.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        X509Certificate certificate = certificateList.get(position);
        String info =
                "pubKey: " + certificate.getPublicKey().toString()
                        + "Time-NotBefore:" + certificate.getNotBefore()
                        + "\nTime-NotAfter:" + certificate.getNotAfter()
                        + "\nAlg:" + certificate.getSigAlgName();
        holder.tv_cert_info.setText(info);
        holder.iv_next.setTag(position);
        if (rootIndex == position) {
            holder.iv_cert.setColorFilter(mContext.getResources().getColor(R.color.colorAccent), PorterDuff.Mode.SRC_IN);
        } else {
            holder.iv_cert.setColorFilter(mContext.getResources().getColor(R.color.grey), android.graphics.PorterDuff.Mode.SRC_IN);
        }
//        holder.tv_root.setVisibility(rootIndex == position ? View.VISIBLE : View.GONE);
    }

    class ViewHolder extends RecyclerView.ViewHolder {
        TextView tv_cert_info;
        ImageView iv_next, iv_cert;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
