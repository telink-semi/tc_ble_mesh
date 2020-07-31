/********************************************************************************************************
 * @file     BaseFragment.java 
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
package com.telink.ble.mesh.ui.fragment;

import android.view.View;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.BaseActivity;

import androidx.fragment.app.Fragment;

/**
 * Created by kee on 2017/9/25.
 */

public class BaseFragment extends Fragment {
    public void toastMsg(CharSequence s) {
        ((BaseActivity) getActivity()).toastMsg(s);
    }

    protected void showWaitingDialog(String tip) {
        ((BaseActivity) getActivity()).showWaitingDialog(tip);
    }

    protected void dismissWaitingDialog() {
        ((BaseActivity) getActivity()).dismissWaitingDialog();
    }

    protected void setTitle(View parent, String title) {
        TextView tv_title = parent.findViewById(R.id.tv_title);
        if (tv_title != null) {
            tv_title.setText(title);
        }
    }
}
