/********************************************************************************************************
 * @file ShareImportFragment.java
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
package com.telink.ble.mesh.ui.fragment;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.RadioButton;
import android.widget.TextView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.json.MeshStorageService;
import com.telink.ble.mesh.ui.JsonPreviewActivity;
import com.telink.ble.mesh.ui.file.FileSelectActivity;
import com.telink.ble.mesh.ui.qrcode.QRCodeScanActivity;
import com.telink.ble.mesh.util.FileSystem;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.File;

/**
 * share import fragment
 */

public class ShareImportFragment extends BaseFragment implements View.OnClickListener {
    private TextView tv_file_select;
    private RadioButton rb_file;
    private TextView tv_log;
    private Button btn_open;
    private static final int REQUEST_CODE_GET_FILE = 1;
    private String mPath;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_share_import, null);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        tv_file_select = view.findViewById(R.id.tv_file_select);
        tv_file_select.setOnClickListener(this);
        btn_open = view.findViewById(R.id.btn_open);
        btn_open.setOnClickListener(this);
        tv_log = view.findViewById(R.id.tv_log);
        view.findViewById(R.id.btn_import).setOnClickListener(this);

        rb_file = view.findViewById(R.id.rb_file);
        rb_file.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                tv_file_select.setVisibility(isChecked ? View.VISIBLE : View.GONE);
            }
        });
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tv_file_select:
                startActivityForResult(new Intent(getActivity(), FileSelectActivity.class).putExtra(FileSelectActivity.KEY_SUFFIX, ".json"), REQUEST_CODE_GET_FILE);
                break;

            case R.id.btn_open:
                if (mPath == null) {
                    return;
                }
                startActivity(new Intent(this.getActivity(), JsonPreviewActivity.class).putExtra(JsonPreviewActivity.FILE_PATH, mPath));
                break;

            case R.id.btn_import:
                if (!rb_file.isChecked()) {
                    startActivity(new Intent(getActivity(), QRCodeScanActivity.class));
//                    getActivity().finish();
                    return;
                }


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
                MeshInfo localMesh = TelinkMeshApplication.getInstance().getMeshInfo();
                MeshInfo newMesh = null;
                try {
                    newMesh = MeshStorageService.getInstance().importExternal(jsonData, localMesh);
                } catch (Exception e) {
                    e.printStackTrace();
                }
                if (newMesh == null) {
                    toastMsg("import failed");
                    return;
                }
                newMesh.saveOrUpdate(getActivity());
                MeshService.getInstance().idle(true);
                TelinkMeshApplication.getInstance().setupMesh(newMesh);
                MeshService.getInstance().setupMeshNetwork(newMesh.convertToConfiguration());
                tv_log.append("Mesh storage import success, back to home page to reconnect\n");
                toastMsg("import success");
                break;
        }
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (resultCode != Activity.RESULT_OK || requestCode != REQUEST_CODE_GET_FILE)
            return;

        mPath = data.getStringExtra(FileSelectActivity.KEY_RESULT);
        btn_open.setVisibility(View.VISIBLE);
        tv_file_select.setText(mPath);

        tv_log.append("File selected: " + mPath + "\n");
        MeshLogger.log("select: " + mPath);
    }

}
