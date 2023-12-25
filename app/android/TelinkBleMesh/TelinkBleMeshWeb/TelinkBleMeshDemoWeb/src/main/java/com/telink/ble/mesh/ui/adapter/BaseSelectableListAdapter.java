/********************************************************************************************************
 * @file BaseSelectableListAdapter.java
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


import androidx.recyclerview.widget.RecyclerView;

/**
 * beans
 */
public abstract class BaseSelectableListAdapter<V extends RecyclerView.ViewHolder> extends BaseRecyclerViewAdapter<V> {
    SelectStatusChangedListener statusChangedListener;

    public abstract boolean allSelected();

    public abstract void setAll(boolean selected);


    public interface SelectStatusChangedListener {
        void onSelectStatusChanged(BaseSelectableListAdapter adapter, int position);
    }

    public void setStatusChangedListener(SelectStatusChangedListener statusChangedListener) {
        this.statusChangedListener = statusChangedListener;
    }
}
