/********************************************************************************************************
 * @file ModelSettingActivity.java
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
package com.telink.ble.mesh.ui;

import android.os.Bundle;
import android.view.View;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.adapter.ModelListAdapter;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * set models
 * Created by kee on 2018/6/5.
 */

public class ModelSettingActivity extends BaseActivity {

    private ModelListAdapter mAdapter;
    private List<MeshSigModel> modelList;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_model_setting);
        setTitle("Subscription Models");
        enableBackNav(true);
        RecyclerView recyclerView = findViewById(R.id.rv_models);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));


        modelList = new ArrayList<>();
        modelList.addAll(Arrays.asList(MeshSigModel.getDefaultSubList()));
        mAdapter = new ModelListAdapter(this, modelList);
        mAdapter.setSelectMode(false);
        findViewById(R.id.tv_confirm).setVisibility(View.GONE);
        recyclerView.setAdapter(mAdapter);
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

}
