/********************************************************************************************************
 * @file SchedulerListActivity.java
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
package com.telink.sig.mesh.demo.ui;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.ui.adapter.BaseRecyclerViewAdapter;
import com.telink.sig.mesh.demo.ui.adapter.SchedulerListAdapter;
import com.telink.sig.mesh.model.DeviceInfo;

/**
 * 本地缓存闹钟列表
 * Created by kee on 2018/9/18.
 */
public class SchedulerListActivity extends BaseActivity implements View.OnClickListener {
    private SchedulerListAdapter mAdapter;
    private DeviceInfo mDevice;
    private View ll_empty;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_common_list);
        ll_empty = findViewById(R.id.ll_empty);
        findViewById(R.id.btn_add).setOnClickListener(this);
        int address = getIntent().getIntExtra("address", -1);
        mDevice = TelinkMeshApplication.getInstance().getMesh().getDeviceByMeshAddress(address);

        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.common_list);
        setTitle("Scheduler List");
        enableBackNav(true);
        toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                if (item.getItemId() == R.id.item_add) {
                    onAddClick();
                }
                return false;
            }
        });


        mAdapter = new SchedulerListAdapter(this, mDevice.schedulers, mDevice.meshAddress);
        /*mAdapter.setOnItemClickListener(new BaseRecyclerViewAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(int position) {
                Intent intent = new Intent(SchedulerListActivity.this, SchedulerSettingActivity.class);
                intent.putExtra("address", mDevice.meshAddress)
                        .putExtra("scheduler", mDevice.schedulers.get(position));

                startActivity(intent);
            }
        });*/
        RecyclerView rv_scheduler = findViewById(R.id.rv_common);
        rv_scheduler.setLayoutManager(new LinearLayoutManager(this));
        rv_scheduler.setAdapter(mAdapter);
    }


    @Override
    protected void onResume() {
        super.onResume();
        if (mDevice.schedulers == null || mDevice.schedulers.size() == 0) {
            ll_empty.setVisibility(View.VISIBLE);
        } else {
            ll_empty.setVisibility(View.GONE);
        }
        mAdapter.notifyDataSetChanged();
    }

    private void onAddClick() {
        startActivity(new Intent(SchedulerListActivity.this, SchedulerSettingActivity.class).putExtra("address", mDevice.meshAddress));
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_add) {
            onAddClick();
        }
    }
}
