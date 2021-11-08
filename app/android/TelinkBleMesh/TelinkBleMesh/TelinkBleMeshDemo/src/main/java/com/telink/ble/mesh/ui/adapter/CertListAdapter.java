/********************************************************************************************************
 * @file OOBListAdapter.java
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

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.PorterDuff;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.OOBPair;
import com.telink.ble.mesh.ui.CertDetailActivity;
import com.telink.ble.mesh.ui.OOBEditActivity;
import com.telink.ble.mesh.ui.OOBInfoActivity;
import com.telink.ble.mesh.util.Arrays;

import java.security.cert.X509Certificate;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;
import java.util.Locale;

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
