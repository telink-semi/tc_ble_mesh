package com.telink.ble.mesh.ui.fragment;

import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.model.GroupInfo;
import com.telink.ble.mesh.ui.GroupSettingActivity;
import com.telink.ble.mesh.ui.adapter.BaseRecyclerViewAdapter;
import com.telink.ble.mesh.ui.adapter.GroupAdapter;

import java.util.List;

import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * Group Control
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
