/********************************************************************************************************
 * @file MeshInfoActivity.java
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
package com.telink.ble.mesh.ui;

import android.os.Bundle;
import android.widget.TextView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.ui.adapter.AppKeyInfoAdapter;
import com.telink.ble.mesh.ui.adapter.MeshKeyInfoAdapter;
import com.telink.ble.mesh.ui.adapter.NetKeyInfoAdapter;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * Created by kee on 2021/01/13.
 */
public class MeshInfoActivity extends BaseActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_mesh_info);
        setTitle("Mesh Info");
        updateUI();
        enableBackNav(true);
    }

    private void updateUI() {
        MeshInfo mesh = TelinkMeshApplication.getInstance().getMeshInfo();

        TextView
                tv_iv_index,
                tv_sno,
                tv_local_adr;
        tv_iv_index = findViewById(R.id.tv_iv_index);
        tv_iv_index.setText(String.format("0x%08X", mesh.ivIndex));
        tv_sno = findViewById(R.id.tv_sno);
        tv_sno.setText(String.format("0x%06X", mesh.sequenceNumber));
        tv_local_adr = findViewById(R.id.tv_local_adr);
        tv_local_adr.setText(String.format("0x%04X", mesh.localAddress));
        NetKeyInfoAdapter netKeyAdapter = new NetKeyInfoAdapter(this, mesh.meshNetKeyList);
        AppKeyInfoAdapter appKeyAdapter = new AppKeyInfoAdapter(this, mesh.appKeyList);

        RecyclerView rv_net_key = findViewById(R.id.rv_net_key);
//        rv_net_key.setNestedScrollingEnabled(false);
        rv_net_key.setLayoutManager(new LinearLayoutManager(this));
        rv_net_key.setAdapter(netKeyAdapter);
        RecyclerView rv_app_key = findViewById(R.id.rv_app_key);
        rv_app_key.setNestedScrollingEnabled(false);
        rv_app_key.setLayoutManager(new LinearLayoutManager(this));
        rv_app_key.setAdapter(appKeyAdapter);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }


}
