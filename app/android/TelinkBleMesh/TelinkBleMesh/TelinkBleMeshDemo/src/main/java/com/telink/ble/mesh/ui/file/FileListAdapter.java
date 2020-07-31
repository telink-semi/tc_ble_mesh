/********************************************************************************************************
 * @file     FileListAdapter.java 
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
            holder.tv_name = (TextView) convertView.findViewById(R.id.tv_name);
            holder.tv_path = (TextView) convertView.findViewById(R.id.tv_path);
            holder.iv_icon = (ImageView) convertView.findViewById(R.id.iv_icon);
            holder.iv_right = (ImageView) convertView.findViewById(R.id.iv_right);
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