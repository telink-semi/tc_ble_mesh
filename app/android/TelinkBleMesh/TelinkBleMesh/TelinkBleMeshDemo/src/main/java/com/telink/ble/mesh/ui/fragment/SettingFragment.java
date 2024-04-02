/********************************************************************************************************
 * @file SettingFragment.java
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

import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.provider.Settings;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;

import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.CertListActivity;
import com.telink.ble.mesh.ui.DebugActivity;
import com.telink.ble.mesh.ui.LogActivity;
import com.telink.ble.mesh.ui.NetworkListActivity;
import com.telink.ble.mesh.ui.OobListActivity;
import com.telink.ble.mesh.ui.SettingsActivity;
import com.telink.ble.mesh.ui.test.IntervalTestActivity;
import com.telink.ble.mesh.ui.test.ResponseTestActivity;
import com.telink.ble.mesh.util.ContextUtil;

/**
 * setting
 */

public class SettingFragment extends BaseFragment implements View.OnClickListener {
    View ll_location_setting;
    private final String[] TEST_ACTION = {
            "Response Test",
            "Interval Test"};
    private AlertDialog cmdDialog;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_setting, null);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        Toolbar toolbar = view.findViewById(R.id.title_bar);
        toolbar.setNavigationIcon(null);
        toolbar.inflateMenu(R.menu.setting_tab);
        toolbar.getMenu().findItem(R.id.item_version).setTitle(getVersion());
        setTitle(view, "Setting");

        view.findViewById(R.id.view_manage_network).setOnClickListener(this);
        view.findViewById(R.id.view_settings).setOnClickListener(this);
        view.findViewById(R.id.view_debug).setOnClickListener(this);
        ll_location_setting = view.findViewById(R.id.ll_location_setting);
        view.findViewById(R.id.btn_location_setting).setOnClickListener(this);
        view.findViewById(R.id.btn_location_ignore).setOnClickListener(this);
        view.findViewById(R.id.view_oob).setOnClickListener(this);
        view.findViewById(R.id.view_log).setOnClickListener(this);
        view.findViewById(R.id.view_tests).setOnClickListener(this);

        view.findViewById(R.id.view_tests).setVisibility(View.GONE); // for release

        View view_cert = view.findViewById(R.id.view_cert);
        view_cert.setOnClickListener(this);
    }

    @Override
    public void onResume() {
        super.onResume();
        if (ContextUtil.isLocationEnable(getActivity()) || SharedPreferenceHelper.isLocationIgnore(getActivity())) {
            ll_location_setting.setVisibility(View.GONE);
        } else {
            ll_location_setting.setVisibility(View.VISIBLE);
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.view_manage_network:
                startActivity(new Intent(getActivity(), NetworkListActivity.class));
                break;

            case R.id.view_settings:
                startActivity(new Intent(getActivity(), SettingsActivity.class));
                break;

            case R.id.view_debug:
                startActivity(new Intent(getActivity(), DebugActivity.class));
                break;

            case R.id.btn_location_setting:
                Intent enableLocationIntent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
                startActivityForResult(enableLocationIntent, 1);
                break;
            case R.id.btn_location_ignore:
                SharedPreferenceHelper.setLocationIgnore(getActivity(), true);
                ll_location_setting.setVisibility(View.GONE);
                break;

            case R.id.view_tests:
                showActionDialog();
                break;

            case R.id.view_oob:
                startActivity(new Intent(getActivity(), OobListActivity.class));
                break;
            case R.id.view_cert:
                startActivity(new Intent(getActivity(), CertListActivity.class));
                break;

            case R.id.view_log:
                startActivity(new Intent(getActivity(), LogActivity.class));
                break;

        }
    }

    private void showActionDialog() {
        if (cmdDialog == null) {
            AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
            builder.setItems(TEST_ACTION, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    if (which == 0) {
                        startActivity(new Intent(getActivity(), ResponseTestActivity.class));
                    } else if (which == 1) {
                        startActivity(new Intent(getActivity(), IntervalTestActivity.class));
                    }
                    cmdDialog.dismiss();
                }
            });
            builder.setTitle("Select Test Actions");
            cmdDialog = builder.create();
        }
        cmdDialog.show();
    }

    private String getVersion() {
        try {
            PackageManager manager = this.getActivity().getPackageManager();
            PackageInfo info = manager.getPackageInfo(this.getActivity().getPackageName(), 0);
            String version = info.versionName;
            return "V" + version;
        } catch (Exception e) {
            e.printStackTrace();
            return "";
        }
    }
}
