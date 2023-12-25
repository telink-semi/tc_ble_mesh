/********************************************************************************************************
 * @file ColorPanelActivity.java
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

import android.os.Bundle;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.lighting.HslSetMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.ui.widget.CompositionColorView;
import com.telink.ble.mesh.web.entity.MeshNetworkDetail;

/**
 * Created by kee on 2018/8/28.
 */

public class ColorPanelActivity extends BaseActivity {

    private CompositionColorView cps_color;

    private int address;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_color_panel);
        enableBackNav(true);
        setTitle("Color Setting");
        address = getIntent().getIntExtra("address", 0);
        cps_color = findViewById(R.id.cps_color);
        cps_color.setMessageDelegate(new CompositionColorView.ColorMessageDelegate() {
            @Override
            public void onHSLMessage(float[] hsl) {
                sendHslSetMessage(hsl);
            }
        });
    }

    private void sendHslSetMessage(float[] hslValue) {
        int hue = (int) (hslValue[0] * 65535 / 360);
        int sat = (int) (hslValue[1] * 65535);
        int lightness = (int) (hslValue[2] * 65535);

        MeshNetworkDetail meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        HslSetMessage hslSetMessage = HslSetMessage.getSimple(address, meshInfo.getDefaultAppKeyIndex(),
                lightness,
                hue,
                sat,
                false, 0);
        MeshService.getInstance().sendMeshMessage(hslSetMessage);
    }
}
