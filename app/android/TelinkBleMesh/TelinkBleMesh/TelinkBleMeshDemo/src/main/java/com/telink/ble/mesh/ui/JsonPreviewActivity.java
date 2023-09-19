/********************************************************************************************************
 * @file JsonPreviewActivity.java
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
