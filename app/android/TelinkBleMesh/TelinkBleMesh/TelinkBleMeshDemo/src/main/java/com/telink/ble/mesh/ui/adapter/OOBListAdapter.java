/********************************************************************************************************
 * @file     OOBListAdapter.java 
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
package com.telink.ble.mesh.ui.adapter;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.OOBPair;
import com.telink.ble.mesh.ui.OOBEditActivity;
import com.telink.ble.mesh.ui.OOBInfoActivity;
import com.telink.ble.mesh.util.Arrays;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;
import java.util.Locale;

import androidx.recyclerview.widget.RecyclerView;

/**
 * oob info list
 */
public class OOBListAdapter extends BaseRecyclerViewAdapter<OOBListAdapter.ViewHolder> {

    private Context mContext;
    private List<OOBPair> mOOBPairs;
    private DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.CHINA);

    public OOBListAdapter(Context context) {
        this.mContext = context;
        this.mOOBPairs = TelinkMeshApplication.getInstance().getMeshInfo().oobPairs;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_oob_info, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_oob_info = itemView.findViewById(R.id.tv_oob_info);
        holder.iv_delete = itemView.findViewById(R.id.iv_delete);
        holder.iv_edit = itemView.findViewById(R.id.iv_edit);
        return holder;
    }

    @Override
    public int getItemCount() {
        return mOOBPairs == null ? 0 : mOOBPairs.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        OOBPair oobPair = mOOBPairs.get(position);
        String extraInfo = dateFormat.format(new Date(oobPair.timestamp)) + " - " +
                (oobPair.importMode == OOBPair.IMPORT_MODE_FILE ? "from file" : "manual input");
        holder.tv_oob_info.setText(mContext.getString(R.string.oob_info
                , Arrays.bytesToHexString(oobPair.deviceUUID)
                , Arrays.bytesToHexString(oobPair.oob)
                , extraInfo));
        holder.iv_delete.setTag(position);
        holder.iv_delete.setOnClickListener(iconClickListener);
        holder.iv_edit.setTag(position);
        holder.iv_edit.setOnClickListener(iconClickListener);
    }

    private View.OnClickListener iconClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            int position;
            if (v.getId() == R.id.iv_delete) {
                position = (int) v.getTag();
                MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
                meshInfo.oobPairs.remove(position);
//                notifyDataSetChanged();
                notifyItemRemoved(position);
                meshInfo.saveOrUpdate(mContext);
            } else if (v.getId() == R.id.iv_edit) {
                position = (int) v.getTag();
                ((Activity) mContext).startActivityForResult(
                        new Intent(mContext, OOBEditActivity.class)
                                .putExtra(OOBEditActivity.EXTRA_POSITION, position)
                                .putExtra(OOBEditActivity.EXTRA_OOB, mOOBPairs.get(position))
                        , OOBInfoActivity.REQUEST_CODE_EDIT_OOB
                );
            }
        }
    };

    class ViewHolder extends RecyclerView.ViewHolder {
        TextView tv_oob_info;
        ImageView iv_delete, iv_edit;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
