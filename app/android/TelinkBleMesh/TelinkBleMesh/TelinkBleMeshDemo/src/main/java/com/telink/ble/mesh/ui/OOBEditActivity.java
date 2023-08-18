/********************************************************************************************************
 * @file OOBEditActivity.java
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
import android.widget.EditText;
import android.widget.TextView;

import androidx.appcompat.widget.Toolbar;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.OobInfo;
import com.telink.ble.mesh.model.db.MeshInfoService;
import com.telink.ble.mesh.ui.widget.HexFormatTextWatcher;
import com.telink.ble.mesh.util.Arrays;

/**
 * add or edit static-OOB
 */
public class OOBEditActivity extends BaseActivity {

    public static final String EXTRA_OOB = "com.telink.ble.mesh.EXTRA_OOB";

    //    private boolean isAddMode = false;
    // oobPair in edit mode
    private long oobId = 0;
    private EditText et_uuid, et_oob;
    private OobInfo oobInfo;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_oob_edit);

        oobId = getIntent().getLongExtra(EXTRA_OOB, 0);
        initView();
        if (oobId != 0) {
            oobInfo = MeshInfoService.getInstance().getOobById(oobId);
            setTitle("Edit OOB");
            et_uuid.setText(Arrays.bytesToHexString(oobInfo.deviceUUID));
            et_oob.setText(Arrays.bytesToHexString(oobInfo.oob));
        } else {
            oobInfo = new OobInfo();
            setTitle("Add OOB");
        }
    }

    private void initView() {
        enableBackNav(true);
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.check);
        toolbar.setOnMenuItemClickListener(item -> {
            if (item.getItemId() == R.id.item_check) {
                if (!validateInput()) {
                    return false;
                }
                Intent intent = new Intent();
                setResult(RESULT_OK, intent);
                finish();
            }
            return false;
        });

        et_uuid = findViewById(R.id.et_uuid);
        et_oob = findViewById(R.id.et_oob);
        TextView tv_uuid_preview = findViewById(R.id.tv_uuid_preview);
        TextView tv_oob_preview = findViewById(R.id.tv_oob_preview);
        et_uuid.addTextChangedListener(new HexFormatTextWatcher(tv_uuid_preview));
        et_oob.addTextChangedListener(new HexFormatTextWatcher(tv_oob_preview));

    }

    private boolean validateInput() {
        String uuidInput = et_uuid.getText().toString();
        if (uuidInput.equals("")) {
            toastMsg("uuid cannot be null");
            return false;
        }

        byte[] uuid = Arrays.hexToBytes(uuidInput);
        if (uuid == null || uuid.length != 16) {
            toastMsg("uuid format error");
            return false;
        }

        String oobInput = et_oob.getText().toString();
        if (oobInput.equals("")) {
            toastMsg("oob cannot be null");
            return false;
        }

        byte[] oob = Arrays.hexToBytes(oobInput);
        if (oob == null || oob.length != 16) {
            toastMsg("oob format error");
            return false;
        }

        oobInfo.timestamp = System.currentTimeMillis();
        oobInfo.oob = oob;
        oobInfo.deviceUUID = uuid;
        MeshInfoService.getInstance().updateOobInfo(oobInfo);
        return true;
    }

}
