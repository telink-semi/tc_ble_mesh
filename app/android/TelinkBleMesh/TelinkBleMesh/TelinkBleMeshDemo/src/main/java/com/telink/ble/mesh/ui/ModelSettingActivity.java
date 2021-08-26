/********************************************************************************************************
 * @file     ModelSettingActivity.java 
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
package com.telink.ble.mesh.ui;

import android.os.Bundle;
import android.view.View;

import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.adapter.ModelListAdapter;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

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
