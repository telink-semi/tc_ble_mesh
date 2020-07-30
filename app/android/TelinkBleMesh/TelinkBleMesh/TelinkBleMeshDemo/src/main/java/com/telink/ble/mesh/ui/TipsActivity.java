/********************************************************************************************************
 * @file     TipsActivity.java 
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
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;

/**
 * tips
 */
public class TipsActivity extends BaseActivity {
    public static final String INTENT_KEY_TIP_RES_ID = "TipsActivity.TIP_RES_ID";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_share_tip);
        setTitle("Tip");
        int tipResId = getIntent().getIntExtra(INTENT_KEY_TIP_RES_ID, R.string.share_tip);
        TextView tv_tip = findViewById(R.id.tv_tip);
        tv_tip.setText(tipResId);
        enableBackNav(true);
    }

    public void onKnownClick(View view) {
        finish();
    }

}
