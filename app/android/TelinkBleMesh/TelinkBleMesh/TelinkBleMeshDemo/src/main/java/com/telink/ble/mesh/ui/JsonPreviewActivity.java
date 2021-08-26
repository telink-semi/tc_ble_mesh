/********************************************************************************************************
 * @file     JsonPreviewActivity.java 
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

import android.content.Intent;
import android.os.Bundle;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.util.FileSystem;

import java.io.File;

/**
 * json preview
 * Created by kee on 2018/9/18.
 */
public class JsonPreviewActivity extends BaseActivity {
    public static final String FILE_PATH = "com.telink.ble.mesh.FILE_PATH";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_json_preview);
        setTitle("JsonPreview");
        enableBackNav(true);

        TextView tv_json = findViewById(R.id.tv_json);
        final Intent intent = getIntent();
        String info = FileSystem.readString(new File(intent.getStringExtra(FILE_PATH)));
        tv_json.setText(info);
    }
}
