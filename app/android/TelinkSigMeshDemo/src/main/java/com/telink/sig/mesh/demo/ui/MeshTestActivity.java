/********************************************************************************************************
 * @file     MeshTestActivity.java 
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
import android.text.TextUtils;
import android.view.View;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.model.DeviceInfo;

import java.util.List;


/**
 * Created by kee on 2017/8/17.
 */

public class MeshTestActivity extends BaseActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_mesh_test);
        findViewById(R.id.tv_filter_init).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                MeshService.getInstance().filterInit(TelinkMeshApplication.getInstance().getMesh().localAddress);
            }
        });
        findViewById(R.id.tv_bv03).setOnClickListener(this.bvClick);
        findViewById(R.id.tv_bv04).setOnClickListener(this.bvClick);
        findViewById(R.id.tv_bv05).setOnClickListener(this.bvClick);
        findViewById(R.id.tv_bv06).setOnClickListener(this.bvClick);
        findViewById(R.id.tv_bv07).setOnClickListener(this.bvClick);

    }

    public View.OnClickListener bvClick = new View.OnClickListener() {
        @Override
        public void onClick(View v) {

            List<DeviceInfo> devices = TelinkMeshApplication.getInstance().getMesh().devices;
            String curDevice = MeshService.getInstance().getCurDeviceMac();
            if (TextUtils.isEmpty(curDevice)) {
                toastMsg("device not connected");
                return;
            }
            int addr = -1;
            for (DeviceInfo device : devices) {
                if (curDevice.equals(device.macAddress)) {
                    addr = device.meshAddress;
                    break;
                }
            }

            if (addr == -1) {
                toastMsg("device info not found");
                return;
            }

            switch (v.getId()) {

                case R.id.tv_bv03:
                    MeshService.getInstance().sendMeshTestCmd(addr, 3);
                    break;
                case R.id.tv_bv04:
                    MeshService.getInstance().sendMeshTestCmd(addr, 4);
                    break;

                case R.id.tv_bv05:
                    MeshService.getInstance().sendMeshTestCmd(addr, 5);
                    break;

                case R.id.tv_bv06:
                    MeshService.getInstance().sendMeshTestCmd(addr, 6);
                    break;

                case R.id.tv_bv07:
                    MeshService.getInstance().sendMeshTestCmd(addr, 7);
                    break;

            }
        }
    };
}
