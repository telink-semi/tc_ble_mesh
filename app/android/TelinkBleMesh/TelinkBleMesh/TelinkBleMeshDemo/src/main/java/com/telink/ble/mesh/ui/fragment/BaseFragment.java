/********************************************************************************************************
 * @file BaseFragment.java
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
package com.telink.ble.mesh.ui.fragment;

import android.text.TextUtils;
import android.view.View;
import android.widget.TextView;

import androidx.fragment.app.Fragment;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.BaseActivity;

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


    protected void setSubTitle(View parent, String subTitle) {
        TextView tv_sub_title = parent.findViewById(R.id.tv_sub_title);
        if (tv_sub_title != null) {
            if (TextUtils.isEmpty(subTitle)) {
                tv_sub_title.setVisibility(View.GONE);
            } else {
                tv_sub_title.setVisibility(View.VISIBLE);
                tv_sub_title.setText(subTitle);
            }
        }
    }


    protected void setTitle(View parent, String title, String subTitle) {
        setTitle(parent, title);
        setSubTitle(parent, subTitle);
    }

}
