/********************************************************************************************************
 * @file ShareImportFragment.java
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

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.json.MeshStorageService;
import com.telink.ble.mesh.ui.cdtp.CdtpImportActivity;
import com.telink.ble.mesh.ui.file.FileSelectActivity;
import com.telink.ble.mesh.ui.qrcode.QRCodeScanActivity;
import com.telink.ble.mesh.util.FileSystem;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.File;

/**
 * share import fragment
 */

public class ShareImportActivity extends BaseActivity implements View.OnClickListener {
    private TextView tv_file_select;
    private RadioButton rb_file, rb_cdtp, rb_qrcode;
    private TextView tv_log;
    private Button btn_open;
    private RadioGroup rg_import_type;
    private static final int REQUEST_CODE_GET_FILE = 1;
    private static final int REQUEST_IMPORT = 2;
    private String mPath;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_share_import);
        findViewById(R.id.btn_import).setOnClickListener(this);
        initView();
    }


    public void initView() {
        setTitle("Share Import");
        enableBackNav(true);
        tv_file_select = findViewById(R.id.tv_file_select);
        tv_file_select.setOnClickListener(this);
        btn_open = findViewById(R.id.btn_open);
        btn_open.setOnClickListener(this);
        tv_log = findViewById(R.id.tv_log);
        findViewById(R.id.btn_import).setOnClickListener(this);
        rg_import_type = findViewById(R.id.rg_import_type);
        rb_file = findViewById(R.id.rb_file);
        rb_cdtp = findViewById(R.id.rb_cdtp);
        rb_qrcode = findViewById(R.id.rb_qrcode);

        rb_file.setOnTouchListener(TOUCH_LISTENER);
        rb_cdtp.setOnTouchListener(TOUCH_LISTENER);
        rb_qrcode.setOnTouchListener(TOUCH_LISTENER);

        rb_file.setOnCheckedChangeListener((buttonView, isChecked) -> tv_file_select.setVisibility(isChecked ? View.VISIBLE : View.GONE));
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tv_file_select:
                startActivityForResult(new Intent(this, FileSelectActivity.class).putExtra(FileSelectActivity.KEY_SUFFIX, ".json"), REQUEST_CODE_GET_FILE);
                break;

            case R.id.btn_open:
                if (mPath == null) {
                    return;
                }
                startActivity(new Intent(this, JsonPreviewActivity.class).putExtra(JsonPreviewActivity.FILE_PATH, mPath));
                break;

            case R.id.btn_import:
                switch (rg_import_type.getCheckedRadioButtonId()) {
                    case R.id.rb_file:
                        importFromFile();
                        break;

                    case R.id.rb_cdtp:
                        startActivityForResult(new Intent(this, CdtpImportActivity.class), REQUEST_IMPORT);
                        break;

                    case R.id.rb_qrcode:
                        startActivityForResult(new Intent(this, QRCodeScanActivity.class), REQUEST_IMPORT);
                        break;
                }

                break;
        }
    }

    @SuppressLint("ClickableViewAccessibility")
    private final View.OnTouchListener TOUCH_LISTENER = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            Drawable drawableRight = ((TextView) v).getCompoundDrawables()[2];
            if (event.getAction() == MotionEvent.ACTION_UP && event.getRawX() >= (v.getRight() - drawableRight.getBounds().width())) {
                toastMsg("phone");
                int resId = 0;
                if (v == rb_file) {
                    resId = R.string.tip_share_import_file;
                } else if (v == rb_qrcode) {
                    resId = R.string.tip_share_import_qrcode;
                } else if (v == rb_cdtp) {
                    resId = R.string.tip_share_import_cdtp;
                }
                startActivity(new Intent(ShareImportActivity.this, TipsActivity.class)
                        .putExtra(TipsActivity.INTENT_KEY_TIP_RES_ID, resId)
                );
                return true;
            }
            return false;
        }
    };


    private void importFromFile() {

        if (mPath == null) {
            toastMsg("Pls select target file");
            return;
        }

        File file = new File(mPath);
        if (!file.exists()) {
            toastMsg("file not exist");
            return;
        }
        String jsonData = FileSystem.readString(file);
        if (MeshStorageService.getInstance().importExternal(jsonData, this)) {
            tv_log.append("Mesh storage import success\n");
        } else {
            tv_log.append("Mesh storage import fail\n");
        }
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (resultCode != Activity.RESULT_OK)
            return;
        if (requestCode == REQUEST_CODE_GET_FILE) {


            mPath = data.getStringExtra(FileSelectActivity.KEY_RESULT);
            btn_open.setVisibility(View.VISIBLE);
            tv_file_select.setText(mPath);

            tv_log.append("File selected: " + mPath + "\n");
            MeshLogger.log("select: " + mPath);
        } else if (requestCode == REQUEST_IMPORT) {
            this.finish();
        }

    }

}
