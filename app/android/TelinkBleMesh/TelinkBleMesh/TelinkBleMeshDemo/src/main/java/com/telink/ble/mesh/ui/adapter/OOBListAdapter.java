/********************************************************************************************************
 * @file OOBListAdapter.java
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

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.OobInfo;
import com.telink.ble.mesh.model.db.MeshInfoService;
import com.telink.ble.mesh.ui.OOBEditActivity;
import com.telink.ble.mesh.ui.OOBInfoActivity;
import com.telink.ble.mesh.util.Arrays;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;
import java.util.Locale;

/**
 * oob info list
 */
public class OOBListAdapter extends BaseRecyclerViewAdapter<OOBListAdapter.ViewHolder> {

    private Context mContext;
    private List<OobInfo> mOobInfos;
    private DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.CHINA);

    public OOBListAdapter(Context context) {
        this.mContext = context;
        this.mOobInfos = MeshInfoService.getInstance().getOobList();
    }

    public void clear() {
        if (this.mOobInfos != null) {
            this.mOobInfos.clear();
            notifyDataSetChanged();
        }
    }

    public OobInfo get(int position) {
        return this.mOobInfos.get(position);
    }

    public void add(OobInfo oobInfo) {
        MeshInfoService.getInstance().addOobInfo(oobInfo);
        this.mOobInfos.add(oobInfo);
        this.notifyDataSetChanged();
    }

    public void remove(int position) {
        MeshInfoService.getInstance().removeOobInfo(mOobInfos.get(position));
        this.mOobInfos.remove(position);
        this.notifyDataSetChanged();
    }

    public void resetData() {
        this.mOobInfos = MeshInfoService.getInstance().getOobList();
        this.notifyDataSetChanged();
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
        return mOobInfos == null ? 0 : mOobInfos.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        OobInfo oobInfo = mOobInfos.get(position);
        String extraInfo = dateFormat.format(new Date(oobInfo.timestamp)) + " - " +
                (oobInfo.importMode == OobInfo.IMPORT_MODE_FILE ? "from file" : "manual input");
        holder.tv_oob_info.setText(mContext.getString(R.string.oob_info
                , Arrays.bytesToHexString(oobInfo.deviceUUID)
                , Arrays.bytesToHexString(oobInfo.oob)
                , extraInfo));
        holder.iv_delete.setTag(position);
        holder.iv_delete.setOnClickListener((View v) -> remove(position));
        holder.iv_edit.setTag(position);
        holder.iv_edit.setOnClickListener(iconClickListener);
    }

    private View.OnClickListener iconClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            int position;
            if (v.getId() == R.id.iv_edit) {
                position = (int) v.getTag();
                ((Activity) mContext).startActivityForResult(
                        new Intent(mContext, OOBEditActivity.class)
                                .putExtra(OOBEditActivity.EXTRA_OOB, mOobInfos.get(position).id)
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
