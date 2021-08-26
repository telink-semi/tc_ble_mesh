/********************************************************************************************************
 * @file ColorPanelActivity.java
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

import android.graphics.Color;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ScrollView;
import android.widget.SeekBar;
import android.widget.TextView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.lighting.HslSetMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.ui.widget.ColorPanel;
import com.telink.ble.mesh.ui.widget.CompositionColorView;
import com.telink.ble.mesh.util.MeshLogger;

import androidx.core.graphics.ColorUtils;

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

        MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        HslSetMessage hslSetMessage = HslSetMessage.getSimple(address, meshInfo.getDefaultAppKeyIndex(),
                lightness,
                hue,
                sat,
                false, 0);
        MeshService.getInstance().sendMeshMessage(hslSetMessage);
    }
}
