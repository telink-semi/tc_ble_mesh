/********************************************************************************************************
 * @file     GroupFragment.java 
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
package com.telink.sig.mesh.demo.ui.fragment;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.ui.adapter.BaseRecyclerViewAdapter;
import com.telink.sig.mesh.demo.ui.GroupSettingActivity;
import com.telink.sig.mesh.demo.ui.adapter.GroupAdapter;
import com.telink.sig.mesh.model.Group;

import java.util.List;

/**
 * 所有分组控制， 开关
 * Created by kee on 2017/8/18.
 */

public class GroupFragment extends BaseFragment {
    private List<Group> groups;

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
        RecyclerView rv_group = (RecyclerView) view.findViewById(R.id.rv_group);
        groups = TelinkMeshApplication.getInstance().getMesh().groups;

        GroupAdapter mAdapter = new GroupAdapter(getActivity(), groups);

        rv_group.setLayoutManager(new LinearLayoutManager(getActivity()));
        rv_group.setAdapter(mAdapter);

        mAdapter.setOnItemLongClickListener(new BaseRecyclerViewAdapter.OnItemLongClickListener() {
            @Override
            public boolean onLongClick(int position) {
                Intent intent = new Intent(getActivity(), GroupSettingActivity.class);
                intent.putExtra("group", groups.get(position));
                startActivity(intent);
                return false;
            }
        });
    }

}
