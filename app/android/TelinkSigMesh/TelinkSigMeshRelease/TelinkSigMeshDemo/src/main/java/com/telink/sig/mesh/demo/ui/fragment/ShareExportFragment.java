/********************************************************************************************************
 * @file DeviceFragment.java
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
package com.telink.sig.mesh.demo.ui.fragment;

import android.content.Intent;
import android.os.Bundle;
import android.text.method.ReplacementTransformationMethod;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import com.telink.sig.mesh.demo.FileSystem;
import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.model.json.MeshStorage;
import com.telink.sig.mesh.demo.model.json.MeshStorageService;
import com.telink.sig.mesh.demo.ui.JsonPreviewActivity;
import com.telink.sig.mesh.demo.ui.MeshTestActivity;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * share export
 */

public class ShareExportFragment extends BaseFragment implements View.OnClickListener {

    private TextView tv_log;
    private File exportDir;
    private Button btn_open;
    private SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
    private String savedPath;
    private static final String SAVED_NAME = "mesh.json";

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_share_export, null);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        view.findViewById(R.id.btn_export).setOnClickListener(this);
        btn_open = view.findViewById(R.id.btn_open);
        btn_open.setOnClickListener(this);
        tv_log = view.findViewById(R.id.tv_log);
        exportDir = FileSystem.getSettingPath();
//        checkFile();
    }

    /**
     * check if file exists
     */
    private void checkFile() {
        String filename = MeshStorageService.JSON_FILE;
        File file = new File(exportDir, filename);
        if (file.exists()) {
            btn_open.setVisibility(View.VISIBLE);
            savedPath = file.getAbsolutePath();
            String desc = "File already exists: " + file.getAbsolutePath() + " -- " + sdf.format(new Date(file.lastModified())) + "\n";
            tv_log.append(desc);
        } else {
            btn_open.setVisibility(View.GONE);
        }
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_export:
                File file = MeshStorageService.getInstance().saveAs(exportDir, SAVED_NAME, TelinkMeshApplication.getInstance().getMesh());
                savedPath = file.getAbsolutePath();
                String desc = "File exported: " + file.getAbsolutePath() + " -- " + sdf.format(new Date(file.lastModified())) + "\n";
                tv_log.append(desc);
                toastMsg("Export Success!");
                btn_open.setVisibility(View.VISIBLE);
                break;
            case R.id.btn_open:
                startActivity(
                        new Intent(this.getActivity(), JsonPreviewActivity.class)
                                .putExtra(JsonPreviewActivity.FILE_PATH, savedPath)
                );
                break;
        }
    }


}
