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
