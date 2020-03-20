package com.telink.ble.mesh.ui;

import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.ui.adapter.SchedulerListAdapter;

import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * 本地缓存闹钟列表
 * Created by kee on 2018/9/18.
 */
public class SchedulerListActivity extends BaseActivity implements View.OnClickListener {
    private SchedulerListAdapter mAdapter;
    private NodeInfo mDevice;
    private View ll_empty;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_common_list);
        ll_empty = findViewById(R.id.ll_empty);
        findViewById(R.id.btn_add).setOnClickListener(this);
        int address = getIntent().getIntExtra("address", -1);
        mDevice = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(address);

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
