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
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.appcompat.widget.Toolbar;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.ui.DirectForwardingListActivity;
import com.telink.ble.mesh.ui.FUActivity;
import com.telink.ble.mesh.ui.MeshInfoActivity;
import com.telink.ble.mesh.ui.PrivateBeaconSettingActivity;
import com.telink.ble.mesh.ui.SceneListActivity;
import com.telink.ble.mesh.util.MeshLogger;

/**
 * setting
 */
public class NetworkFragment extends BaseFragment implements View.OnClickListener, EventListener<String> {
    private TextView tv_mesh_name;

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
        tv_mesh_name = view.findViewById(R.id.tv_mesh_name);
        view.findViewById(R.id.view_scene_setting).setOnClickListener(this);
        view.findViewById(R.id.view_mesh_info).setOnClickListener(this);
        view.findViewById(R.id.view_mesh_ota).setOnClickListener(this);
        view.findViewById(R.id.view_df).setOnClickListener(this);
        view.findViewById(R.id.view_private_beacon).setOnClickListener(this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_MESH_RESET, this);
    }

    @Override
    public void onResume() {
        super.onResume();
        MeshLogger.d("network fragment - onResume");
        showMeshName();
    }

    private void showMeshName() {
        tv_mesh_name.setText(TelinkMeshApplication.getInstance().getMeshInfo().meshName);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {

            case R.id.view_scene_setting:
                startActivity(new Intent(getActivity(), SceneListActivity.class));
                break;

            case R.id.view_mesh_ota:
                startActivity(new Intent(getActivity(), FUActivity.class));
                break;

            case R.id.view_mesh_info:
                startActivity(new Intent(getActivity(), MeshInfoActivity.class));
                break;

            case R.id.view_df:
                startActivity(new Intent(getActivity(), DirectForwardingListActivity.class));
                break;

            case R.id.view_private_beacon:
                startActivity(new Intent(getActivity(), PrivateBeaconSettingActivity.class));
                break;
        }
    }

    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(MeshEvent.EVENT_TYPE_MESH_RESET)) {
            showMeshName();
        }
    }
}
