/********************************************************************************************************
 * @file SceneSettingActivity.java
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
package com.telink.sig.mesh.demo.ui;

import android.app.Fragment;
import android.app.FragmentManager;
import android.os.Bundle;
import android.support.design.widget.TabLayout;
import android.support.v13.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.ui.fragment.ShareExportFragment;
import com.telink.sig.mesh.demo.ui.fragment.ShareImportFragment;

/**
 * share tips
 */
public class ShareTipActivity extends BaseActivity {
    public static final String INTENT_KEY_TIP_RES_ID = "ShareTipActivity.TIP_RES_ID";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
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
