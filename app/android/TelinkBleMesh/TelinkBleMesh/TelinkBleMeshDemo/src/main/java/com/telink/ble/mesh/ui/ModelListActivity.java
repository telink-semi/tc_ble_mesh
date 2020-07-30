/********************************************************************************************************
 * @file     ModelListActivity.java 
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

/**
 * model setting
 */

@Deprecated
public class ModelListActivity extends BaseActivity {

    /*private ModelListAdapter mAdapter;
    private List<SigMeshModel> modelList;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_model_list);
        initTitle();
        enableBackNav(true);
        RecyclerView recyclerView = findViewById(R.id.rv_models);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));

        modelList = TelinkMeshApplication.getInstance().getMeshInfo().selectedModels;
        mAdapter = new ModelListAdapter(this, modelList);
        recyclerView.setAdapter(mAdapter);
    }

    private void initTitle() {
        Toolbar toolbar = findViewById(R.id.title_bar);
        setTitle("Model List");
        toolbar.inflateMenu(R.menu.setting);
        enableBackNav(true);
        toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                if (item.getItemId() == R.id.item_setting) {
                    startActivity(new Intent(ModelListActivity.this, ModelSettingActivity.class));
                }
                return false;
            }
        });
    }

    @Override
    public void onResume() {
        super.onResume();
        mAdapter.notifyDataSetChanged();
    }*/
}
