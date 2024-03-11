/********************************************************************************************************
 * @file SchedulerListActivity.java
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
package com.telink.ble.mesh.ui.activity;

import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONException;
import com.google.android.material.textfield.TextInputEditText;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.web.MeshScheduler;
import com.telink.ble.mesh.ui.adapter.SchedulerListAdapter;
import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.ResponseInfo;
import com.telink.ble.mesh.web.TelinkHttpClient;
import com.telink.ble.mesh.web.WebUtils;
import com.telink.ble.mesh.web.entity.MeshNode;

import java.io.IOException;
import java.util.HashMap;
import java.util.List;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.Response;

/**
 * schedulers in node
 * Created by kee on 2018/9/18.
 */
public class SchedulerListActivity extends BaseActivity implements View.OnClickListener {

    public final static int REQUEST_CODE_SCHEDULER_SETTING = 1;

    public final static String URL_GET_SCHEDULER_LIST = TelinkHttpClient.URL_BASE + "mesh-scheduler/getSchedulerList";

    public final static String URL_CREATE_SCHEDULER = TelinkHttpClient.URL_BASE + "mesh-scheduler/createScheduler";

    private SchedulerListAdapter mAdapter;

    private MeshNode mDevice;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_scheduler_list);
        findViewById(R.id.fab_scheduler).setOnClickListener(this);
        int address = getIntent().getIntExtra("address", -1);
        mDevice = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(address);
        setTitle("Scheduler List");
        enableBackNav(true);

        enableBackNav(true);
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.menu_single);

        toolbar.setOnMenuItemClickListener(item -> {
            if (item.getItemId() == R.id.item_single) {
                getSchedulerList();
            }
            return false;
        });

        mAdapter = new SchedulerListAdapter(this, mDevice.address);
        RecyclerView rv_scheduler = findViewById(R.id.rv_scheduler);
        rv_scheduler.setLayoutManager(new LinearLayoutManager(this));
        rv_scheduler.setAdapter(mAdapter);
        ll_empty = findViewById(R.id.ll_empty);
        getSchedulerList();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    TextInputEditText et_input;

    private void onAddClick() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Please input scheduler name ")
                .setView(R.layout.dialog_single_input)
                .setPositiveButton("Confirm", (dialog, which) -> createScheduler(et_input.getText().toString()))
                .setNegativeButton("Cancel", (dialog, which) -> dialog.cancel());
        AlertDialog dialog = builder.show();
        et_input = dialog.findViewById(R.id.et_input);
    }


    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.fab_scheduler) {
            onAddClick();
        }
    }

    public void getSchedulerList() {
        showIndeterminateLoading();
        HashMap<String, String> params = new HashMap<>();
        params.put("nodeId", mDevice.id + "");
        TelinkHttpClient.getInstance().get(URL_GET_SCHEDULER_LIST, params, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                dismissIndeterminateLoading();
                MeshLogger.e("call fail");
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, SchedulerListActivity.this);
                if (responseInfo == null) {
                    return;
                }
                try {
                    List<MeshScheduler> list = JSON.parseArray(responseInfo.data, MeshScheduler.class);
                    runOnUiThread(() -> {
                        mAdapter.resetData(list);
                        updateEmptyVisibility(mAdapter.getItemCount() == 0);
                    });
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        });
    }


    private void createScheduler(String name) {
        showIndeterminateLoading();
        if (TextUtils.isEmpty(name)) {
            toastMsg("pls input network name");
            return;
        }
        FormBody formBody = new FormBody.Builder()
                .add("name", name)
                .add("nodeId", mDevice.id + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_CREATE_SCHEDULER, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.d("create network call onFailure");
                dismissIndeterminateLoading();
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, SchedulerListActivity.this);
                if (responseInfo == null) {
                    return;
                }
                MeshLogger.d("create network call response");
                try {

                    MeshScheduler scheduler = JSON.parseObject(responseInfo.data, MeshScheduler.class);
                    if (scheduler == null) {
                        showError("network null in response");
                        return;
                    }
                    runOnUiThread(() -> {
                        mAdapter.addItem(scheduler);
                        updateEmptyVisibility(false);
                    });
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        MeshLogger.d("SchedulerListActivity - onActivityResult : " + requestCode + " -- " + resultCode);
        if (requestCode == REQUEST_CODE_SCHEDULER_SETTING && resultCode == RESULT_OK) {
            getSchedulerList();
        }
    }
}
