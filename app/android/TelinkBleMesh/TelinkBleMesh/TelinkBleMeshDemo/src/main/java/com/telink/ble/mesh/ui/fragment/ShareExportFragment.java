/********************************************************************************************************
 * @file ShareExportFragment.java
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

import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.TextView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.MeshNetKey;
import com.telink.ble.mesh.model.json.MeshStorageService;
import com.telink.ble.mesh.ui.JsonPreviewActivity;
import com.telink.ble.mesh.ui.ShareActivity;
import com.telink.ble.mesh.ui.adapter.MeshKeySelectAdapter;
import com.telink.ble.mesh.ui.qrcode.QRCodeShareActivity;
import com.telink.ble.mesh.util.FileSystem;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * share export
 */

public class ShareExportFragment extends BaseFragment implements View.OnClickListener {

    private TextView tv_log;
    private RadioButton rb_file;
    private File exportDir;
    private Button btn_open;
    private SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
    private String savedPath;
    private static final String SAVED_NAME = "mesh.json";
    private MeshKeySelectAdapter adapter;

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
        initView(view);
    }

    private void initView(View parent) {
        rb_file = parent.findViewById(R.id.rb_file);
        RecyclerView rv_net_key_select = parent.findViewById(R.id.rv_net_key_select);
        rv_net_key_select.setLayoutManager(new LinearLayoutManager(getActivity()));
        MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        adapter = new MeshKeySelectAdapter(getActivity(), meshInfo.meshNetKeyList);
        rv_net_key_select.setAdapter(adapter);
    }

    /**
     * check if file exists
     */
    private void checkFile() {
        String filename = MeshStorageService.JSON_FILE;
        File file = new File(exportDir, filename);
        if (file.exists()) {
//            btn_open.setVisibility(View.VISIBLE);
            btn_open.setVisibility(View.GONE);
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
                List<MeshNetKey> selectedKeys = getSelectedNetKeys();
                if (selectedKeys.size() == 0) {
                    toastMsg("select at least one net key");
                    return;
                }
                if (rb_file.isChecked()){
                    MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
                    File file = MeshStorageService.getInstance().exportMeshToJson(
                            exportDir,
                            MeshStorageService.JSON_FILE,
                            meshInfo,
                            selectedKeys);

                    savedPath = file.getAbsolutePath();
                    String desc = "File exported: " + file.getAbsolutePath() + " -- " + sdf.format(new Date(file.lastModified())) + "\n";
//                tv_log.append(desc);
                    tv_log.setText(desc);
                    toastMsg("Export Success!");
//                btn_open.setVisibility(View.VISIBLE);
                    btn_open.setVisibility(View.GONE);
                }else {
                    int[] keyIndexes = new int[selectedKeys.size()];
                    for (int i = 0; i < keyIndexes.length; i++) {
                        keyIndexes[i] = selectedKeys.get(i).index;
                    }
                    startActivity(new Intent(getActivity(), QRCodeShareActivity.class).putExtra("selectedIndexes", keyIndexes));
                }

                break;
            case R.id.btn_open:
                startActivity(
                        new Intent(this.getActivity(), JsonPreviewActivity.class)
                                .putExtra(JsonPreviewActivity.FILE_PATH, savedPath)
                );
                break;
        }
    }

    private List<MeshNetKey> getSelectedNetKeys() {
        List<MeshNetKey> meshNetKeyList = new ArrayList<>();
        final List<MeshNetKey> netKeyList = TelinkMeshApplication.getInstance().getMeshInfo().meshNetKeyList;
        final List<Boolean> selectList = adapter.getSelectList();
        for (int i = 0; i < selectList.size(); i++) {
            if (selectList.get(i)) {
                meshNetKeyList.add(netKeyList.get(i));
            }
        }
        return meshNetKeyList;
    }

}
