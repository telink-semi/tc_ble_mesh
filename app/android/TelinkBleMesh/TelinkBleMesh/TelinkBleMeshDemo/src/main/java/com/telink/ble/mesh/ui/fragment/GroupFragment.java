/********************************************************************************************************
 * @file GroupFragment.java
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

import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.GroupInfo;
import com.telink.ble.mesh.ui.GroupSettingActivity;
import com.telink.ble.mesh.ui.adapter.BaseRecyclerViewAdapter;
import com.telink.ble.mesh.ui.adapter.GroupAdapter;

import java.util.List;

/**
 * Group Control
 * show groups in mesh
 * Created by kee on 2017/8/18.
 */

public class GroupFragment extends BaseFragment {
    private List<GroupInfo> groups;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_group, null);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        Toolbar toolbar = view.findViewById(R.id.title_bar);
        toolbar.setNavigationIcon(null);
        setTitle(view, "Group");
        RecyclerView rv_group = view.findViewById(R.id.rv_group);
        groups = TelinkMeshApplication.getInstance().getMeshInfo().groups;

        GroupAdapter mAdapter = new GroupAdapter(getActivity(), groups);

        rv_group.setLayoutManager(new LinearLayoutManager(getActivity()));
        rv_group.setAdapter(mAdapter);

        mAdapter.setOnItemLongClickListener(position -> {
            Intent intent = new Intent(getActivity(), GroupSettingActivity.class);
            intent.putExtra("group", groups.get(position));
            startActivity(intent);
            return false;
        });
    }

}
