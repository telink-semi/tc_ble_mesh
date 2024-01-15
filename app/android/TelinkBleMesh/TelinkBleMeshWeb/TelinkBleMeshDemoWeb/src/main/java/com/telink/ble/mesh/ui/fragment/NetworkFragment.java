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

import android.content.Intent;
import android.os.Bundle;
import android.provider.Settings;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.appcompat.widget.Toolbar;

import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.activity.DebugActivity;
import com.telink.ble.mesh.ui.activity.FUActivity;
import com.telink.ble.mesh.ui.activity.MainActivity;
import com.telink.ble.mesh.ui.activity.MeshInfoActivity;
import com.telink.ble.mesh.ui.activity.SceneListActivity;
import com.telink.ble.mesh.ui.activity.SettingsActivity;
import com.telink.ble.mesh.ui.activity.ShareApplicantsActivity;
import com.telink.ble.mesh.ui.activity.ShareExportActivity;
import com.telink.ble.mesh.util.ContextUtil;

/**
 * setting
 */

public class NetworkFragment extends BaseFragment implements View.OnClickListener {
    View ll_location_setting;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_network, null);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        setTitle(view, "Network");
        Toolbar toolbar = view.findViewById(R.id.title_bar);
        toolbar.setNavigationIcon(null);
        toolbar.inflateMenu(R.menu.menu_single);
        toolbar.setOnMenuItemClickListener(item -> {
            ((MainActivity) getActivity()).showConfirmDialog("reload network data from cloud?",
                    (dialog, which) -> ((MainActivity) getActivity()).getNetworkInfo());
            return false;
        });


        view.findViewById(R.id.view_scene_setting).setOnClickListener(this);
        view.findViewById(R.id.view_debug).setOnClickListener(this);
        view.findViewById(R.id.view_share).setOnClickListener(this);
        view.findViewById(R.id.view_applicants).setOnClickListener(this);
        view.findViewById(R.id.view_mesh_info).setOnClickListener(this);
        ll_location_setting = view.findViewById(R.id.ll_location_setting);
        view.findViewById(R.id.btn_location_setting).setOnClickListener(this);
        view.findViewById(R.id.btn_location_ignore).setOnClickListener(this);
        view.findViewById(R.id.view_mesh_ota).setOnClickListener(this);
        view.findViewById(R.id.view_df).setOnClickListener(this);
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
            case R.id.view_scene_setting:
                startActivity(new Intent(getActivity(), SceneListActivity.class));
                break;

            case R.id.view_debug:
                startActivity(new Intent(getActivity(), DebugActivity.class));
                break;

            case R.id.view_share:
                startActivity(new Intent(getActivity(), ShareExportActivity.class)
                        .putExtra("networkId", TelinkMeshApplication.getInstance().getMeshInfo().id)
                        .putExtra("networkName", TelinkMeshApplication.getInstance().getMeshInfo().name));
                break;

            case R.id.view_applicants:
                startActivity(new Intent(getActivity(), ShareApplicantsActivity.class)
                        .putExtra("networkId", TelinkMeshApplication.getInstance().getMeshInfo().id)
                        .putExtra("networkName", TelinkMeshApplication.getInstance().getMeshInfo().name));
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

            case R.id.view_df:
//                startActivity(new Intent(getActivity(), DirectForwardingListActivity.class));
                break;
        }
    }

}
