/********************************************************************************************************
 * @file FileListAdapter.java
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

package com.telink.ble.mesh.ui.file;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by ke on 2016/10/8.
 */
public class FileListAdapter extends BaseAdapter {

    private Context mContext;
    private List<File> mFiles;
    private String targetSuffix;
    private boolean searchMode = false;

    FileListAdapter(Context context, String targetSuffix) {
        this.mContext = context;
        this.targetSuffix = targetSuffix;
        mFiles = new ArrayList<>();
    }

    public void setData(List<File> files) {
        this.mFiles = files;
        this.notifyDataSetChanged();
    }

    public void setSearchMode(boolean searchMode) {
        this.searchMode = searchMode;
    }

    @Override
    public int getCount() {
        return mFiles == null ? 0 : mFiles.size();
    }

    @Override
    public Object getItem(int position) {
        return mFiles.get(position);
    }

    @Override
    public long getItemId(int position) {
        return 0;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolder holder;
        if (convertView == null) {
            convertView = LayoutInflater.from(mContext).inflate(R.layout.item_file, null);
            holder = new ViewHolder();
            holder.tv_name = convertView.findViewById(R.id.tv_name);
            holder.tv_path = convertView.findViewById(R.id.tv_path);
            holder.iv_icon = convertView.findViewById(R.id.iv_icon);
            holder.iv_right = convertView.findViewById(R.id.iv_right);
            convertView.setTag(holder);
        } else {
            holder = (ViewHolder) convertView.getTag();
        }
        if (mFiles.get(position).isDirectory()) {
            // dir
            holder.iv_icon.setImageResource(R.drawable.ic_folder);
            holder.iv_right.setVisibility(View.VISIBLE);
        } else {
            holder.iv_right.setVisibility(View.GONE);
            // file .bin
            if (targetSuffix != null && !targetSuffix.equals("") && mFiles.get(position).getName().endsWith(targetSuffix)) {
                holder.iv_icon.setImageResource(R.drawable.ic_file_checked);
            } else {
                holder.iv_icon.setImageResource(R.drawable.ic_file_unchecked);
            }
        }
        holder.tv_name.setText(mFiles.get(position).getName());
        if (searchMode) {
            holder.tv_path.setVisibility(View.VISIBLE);
            holder.tv_path.setText(mFiles.get(position).getAbsolutePath());
        } else {
            holder.tv_path.setVisibility(View.GONE);
        }
        return convertView;
    }

    class ViewHolder {
        public TextView tv_name;
        public ImageView iv_icon;
        public ImageView iv_right;
        public TextView tv_path;
    }
}