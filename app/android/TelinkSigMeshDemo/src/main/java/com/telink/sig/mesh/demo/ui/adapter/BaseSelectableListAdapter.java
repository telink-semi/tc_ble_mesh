/********************************************************************************************************
 * @file     BaseSelectableListAdapter.java 
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
import com.telink.sig.mesh.model.DeviceInfo;

import java.util.List;

/**
 * beans
 */
public abstract class BaseSelectableListAdapter<V extends RecyclerView.ViewHolder> extends BaseRecyclerViewAdapter<V> {
    SelectStatusChangedListener statusChangedListener;

    public abstract boolean allSelected();

    public abstract void setAll(boolean selected);


    public interface SelectStatusChangedListener {
        void onStatusChanged(BaseSelectableListAdapter adapter);
    }

    public void setStatusChangedListener(SelectStatusChangedListener statusChangedListener) {
        this.statusChangedListener = statusChangedListener;
    }
}
