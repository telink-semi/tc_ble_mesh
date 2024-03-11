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

import android.view.View;
import android.widget.TextView;

import androidx.appcompat.widget.Toolbar;
import androidx.fragment.app.Fragment;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.activity.BaseActivity;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.Objects;

/**
 * Created by kee on 2017/9/25.
 */

public class BaseFragment extends Fragment {

    protected View ll_empty;

    public void toastMsg(CharSequence s) {
        if (getActivity() == null) return;
        ((BaseActivity) getActivity()).toastMsg(s);
    }

    protected void showWaitingDialog(String tip) {
        if (getActivity() == null) return;
        ((BaseActivity) getActivity()).showWaitingDialog(tip);
    }

    protected void dismissWaitingDialog() {
        if (getActivity() == null) return;
        ((BaseActivity) getActivity()).dismissWaitingDialog();
    }

    protected void setTitle(View parent, String title) {
        TextView tv_title = parent.findViewById(R.id.tv_title);
        if (tv_title != null) {
            tv_title.setText(title);
        }
    }

    protected void showError(String error) {
        MeshLogger.e(error);
        Objects.requireNonNull(getActivity()).runOnUiThread(() -> toastMsg(error));
    }

    protected void showSuccess(String error) {
        MeshLogger.d(error);
        Objects.requireNonNull(getActivity()).runOnUiThread(() -> toastMsg(error));
    }

    protected void enableBackNav(boolean enable, View root) {
        Toolbar toolbar = root.findViewById(R.id.title_bar);
        if (enable) {
            toolbar.setNavigationIcon(R.drawable.ic_arrow_left);
            toolbar.setNavigationOnClickListener(v -> Objects.requireNonNull(getActivity()).onBackPressed());
        } else {
            toolbar.setNavigationIcon(null);
        }

    }

    /**
     * show loading progress bar below title
     */
    public void showIndeterminateLoading() {
        if (getActivity() == null) {
            return;
        }
        ((BaseActivity) getActivity()).showIndeterminateLoading();
    }

    /**
     * dismiss loading progress bar below title
     */
    public void dismissIndeterminateLoading() {
        if (getActivity() == null) {
            return;
        }
        ((BaseActivity) getActivity()).dismissIndeterminateLoading();
    }


    protected void updateEmptyVisibility(boolean visible) {
        if (ll_empty == null) return;
        ll_empty.setVisibility(visible ? View.VISIBLE : View.GONE);
    }
}
