/********************************************************************************************************
 * @file ColorPanel.java
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
package com.telink.ble.mesh.ui.widget;

import android.content.Context;
import android.text.TextUtils;
import android.util.AttributeSet;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.model.DeviceConfig;
import com.telink.ble.mesh.ui.adapter.ConfigParamListAdapter;
import com.telink.ble.mesh.util.Arrays;

import java.util.List;

/**
 * Created by kee on 2018/7/10.
 */

public class EditableRecyclerView extends RecyclerView {


    public EditableRecyclerView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        init(context);
    }

    public EditableRecyclerView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    public EditableRecyclerView(Context context) {
        super(context);
        init(context);
    }

    private void init(Context context) {

    }

    List<DeviceConfig.Parameter> configList;
    ConfigParamListAdapter adapter;

    public void setData(Context context, List<DeviceConfig.Parameter> configList) {
        this.configList = configList;
        adapter = new ConfigParamListAdapter(context, configList);
        this.setLayoutManager(new LinearLayoutManager(context));
        this.setAdapter(adapter);
    }

    public boolean checkInputs() {
        ConfigParamListAdapter.ViewHolder holder;
        String input;
        for (int i = 0; i < configList.size(); i++) {

            holder = (ConfigParamListAdapter.ViewHolder) this.findViewHolderForAdapterPosition(i);
            if (holder != null) {
                input = holder.et_input.getText().toString().trim();
                if (TextUtils.isEmpty(input)) {
                    return false;
                }
                configList.get(i).value = Arrays.hexToBytes(input);
            }
        }
        return true;
    }
//    public void
}

