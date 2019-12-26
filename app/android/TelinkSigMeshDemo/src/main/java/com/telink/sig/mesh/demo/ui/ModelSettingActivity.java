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
package com.telink.sig.mesh.demo.ui;

import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.View;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.model.SigMeshModel;
import com.telink.sig.mesh.demo.ui.adapter.BaseRecyclerViewAdapter;
import com.telink.sig.mesh.demo.ui.adapter.ModelListAdapter;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * set models
 * Created by kee on 2018/6/5.
 */

public class ModelSettingActivity extends BaseActivity implements View.OnClickListener {


    private ModelListAdapter mAdapter;
    private List<SigMeshModel> modelList;

    private static final int MODE_BIND = 0;
    private static final int MODE_SUB = 1;
    private int mode;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_model_setting);
        setTitle("Model Setting");
        enableBackNav(true);
        RecyclerView recyclerView = findViewById(R.id.rv_models);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));


        mode = getIntent().getIntExtra("mode", 0);

        modelList = new ArrayList<>();

        if (mode == MODE_SUB) {

            modelList.addAll(Arrays.asList(SigMeshModel.getDefaultSubList()));
            mAdapter = new ModelListAdapter(this, modelList);
            mAdapter.setSelectMode(false);
            findViewById(R.id.tv_confirm).setVisibility(View.GONE);
        } else {
            modelList.addAll(Arrays.asList(SigMeshModel.values()));
            List<SigMeshModel> selectModels = TelinkMeshApplication.getInstance().getMesh().selectedModels;

            outer:
            for (SigMeshModel modelInAll : modelList) {
                for (SigMeshModel selectModel : selectModels) {
                    if (modelInAll == selectModel) {
                        modelInAll.selected = true;
                        continue outer;
                    }
                }
            }
            mAdapter = new ModelListAdapter(this, modelList);
            mAdapter.setSelectMode(true);
            mAdapter.setOnItemClickListener(new BaseRecyclerViewAdapter.OnItemClickListener() {
                @Override
                public void onItemClick(int position) {
                    modelList.get(position).selected = !modelList.get(position).selected;
                    mAdapter.notifyDataSetChanged();
                }
            });
            findViewById(R.id.tv_confirm).setOnClickListener(this);
        }
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


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tv_confirm:
                if (mode == MODE_BIND) {
                    List<SigMeshModel> local = TelinkMeshApplication.getInstance().getMesh().selectedModels;
                    local.clear();
                    for (SigMeshModel modelInAll : modelList) {
                        if (modelInAll.selected) {
                            local.add(modelInAll);
                        }
                    }
                    TelinkMeshApplication.getInstance().getMesh().saveOrUpdate(getApplicationContext());
                    toastMsg("Model Saved");
                }

                break;

        }
    }
}
