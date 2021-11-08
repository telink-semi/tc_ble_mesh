/********************************************************************************************************
 * @file SettingFragment.java
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
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.ui.DebugActivity;
import com.telink.ble.mesh.ui.FUActivity;
import com.telink.ble.mesh.ui.MeshInfoActivity;
import com.telink.ble.mesh.ui.SceneListActivity;
import com.telink.ble.mesh.ui.SettingsActivity;
import com.telink.ble.mesh.ui.ShareActivity;
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

        view.findViewById(R.id.view_model_setting).setOnClickListener(this);
        view.findViewById(R.id.view_scene_setting).setOnClickListener(this);
        view.findViewById(R.id.view_settings).setOnClickListener(this);
        view.findViewById(R.id.view_debug).setOnClickListener(this);
        view.findViewById(R.id.view_share).setOnClickListener(this);
        view.findViewById(R.id.view_mesh_ota).setOnClickListener(this);
        view.findViewById(R.id.view_mesh_info).setOnClickListener(this);
        ll_location_setting = view.findViewById(R.id.ll_location_setting);
        view.findViewById(R.id.btn_location_setting).setOnClickListener(this);
        view.findViewById(R.id.btn_location_ignore).setOnClickListener(this);
        view.findViewById(R.id.view_tests).setOnClickListener(this);

        if (AppSettings.DRAFT_FEATURES_ENABLE) {
            view.findViewById(R.id.view_mesh_ota).setVisibility(View.VISIBLE);
        } else {
            view.findViewById(R.id.view_mesh_ota).setVisibility(View.GONE);
        }

        view.findViewById(R.id.view_tests).setVisibility(View.GONE); // for release
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
            case R.id.view_model_setting:
//                startActivity(new Intent(getActivity(), ModelListActivity.class));
                break;
            case R.id.view_scene_setting:
                startActivity(new Intent(getActivity(), SceneListActivity.class));
                break;
            case R.id.view_settings:
                startActivity(new Intent(getActivity(), SettingsActivity.class));
                break;

            case R.id.view_debug:
                startActivity(new Intent(getActivity(), DebugActivity.class));
                break;

            case R.id.view_share:
                startActivity(new Intent(getActivity(), ShareActivity.class));
                break;
            case R.id.view_mesh_ota:
                startActivity(new Intent(getActivity(), FUActivity.class));
                break;

            case R.id.view_mesh_info:
                startActivity(new Intent(getActivity(), MeshInfoActivity.class));
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
