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

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.model.Mesh;
import com.telink.sig.mesh.demo.model.json.MeshStorage;
import com.telink.sig.mesh.demo.model.json.MeshStorageService;
import com.telink.sig.mesh.demo.ui.JsonPreviewActivity;
import com.telink.sig.mesh.demo.ui.MeshTestActivity;
import com.telink.sig.mesh.demo.ui.file.FileSelectActivity;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.light.ProvisionDataGenerator;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.util.Arrays;
import com.telink.sig.mesh.util.TelinkLog;

import java.util.ArrayList;
import java.util.List;

/**
 * share import fragment
 */

public class ShareImportFragment extends BaseFragment implements View.OnClickListener {
    private TextView tv_file_select;
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
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tv_file_select:
                startActivityForResult(new Intent(getActivity(), FileSelectActivity.class).putExtra(FileSelectActivity.FILE_SUFFIX, ".bin"), REQUEST_CODE_GET_FILE);
                break;

            case R.id.btn_open:
                if (mPath == null) {
                    return;
                }
                startActivity(new Intent(this.getActivity(), JsonPreviewActivity.class).putExtra(JsonPreviewActivity.FILE_PATH, mPath));
                break;

            case R.id.btn_import:
                if (mPath == null) {
                    toastMsg("Pls select target file");
                    return;
                }

                Mesh newMesh = MeshStorageService.getInstance().importExternal(mPath, TelinkMeshApplication.getInstance().getMesh());
                newMesh.saveOrUpdate(getActivity());
                MeshService.getInstance().idle(true);

                TelinkMeshApplication.getInstance().getMeshLib().meshRetrieveAll();

                TelinkMeshApplication.getInstance().setupMesh(newMesh);
                byte[] pvData = ProvisionDataGenerator.getProvisionData(newMesh.networkKey, newMesh.netKeyIndex, newMesh.ivUpdateFlag, newMesh.ivIndex, newMesh.localAddress);
                MeshService.getInstance().meshProvisionParSetDir(pvData, pvData.length);
                MeshService.getInstance().setLocalAddress(newMesh.localAddress);

//                byte[][] nodeData = MeshStorageService.getInstance().getAllVCNodeInfo();
                List<byte[]> nodeList = new ArrayList<>();

                for (DeviceInfo node : newMesh.devices) {
                    nodeList.add(node.nodeInfo.toVCNodeInfo());
                }

                byte[][] nodeData = nodeList.toArray(new byte[][]{});
                MeshService.getInstance().reattachNodes(nodeData);
                MeshService.getInstance().resetAppKey(newMesh.appKeyIndex, newMesh.netKeyIndex, newMesh.appKey);

                tv_log.append("Mesh storage import success, back to home page to reconnect\n");
                toastMsg("import success");

                /*if (MeshStorageService.getInstance().importExternal(mPath)) {
                    TelinkLog.d("Mesh key: ==" + Arrays.bytesToHexString(TelinkMeshApplication.getInstance().getMeshLib().getMeshKey(), ":"));
                    MeshService.getInstance().idle(true);
                    MeshStorageService.getInstance().save(getActivity());
                    Mesh newMesh = MeshStorageService.getInstance().toLocalMesh();
                    TelinkMeshApplication.getInstance().setupMesh(newMesh);

                    byte[] pvData = ProvisionDataGenerator.getProvisionData(newMesh.networkKey, newMesh.netKeyIndex, newMesh.ivUpdateFlag, newMesh.ivIndex, newMesh.localAddress);
                    MeshService.getInstance().meshProvisionParSetDir(pvData, pvData.length);
                    MeshService.getInstance().setLocalAddress(newMesh.localAddress);

                    byte[][] nodeData = MeshStorageService.getInstance().getAllVCNodeInfo();
                    MeshService.getInstance().reattachNodes(nodeData);
                    MeshService.getInstance().resetAppKey(newMesh.appKeyIndex, newMesh.netKeyIndex, newMesh.appKey);
                    tv_log.append("Mesh storage import success, back to home page to reconnect\n");
                    toastMsg("import success");
                    TelinkLog.d("Mesh key --:" + Arrays.bytesToHexString(TelinkMeshApplication.getInstance().getMeshLib().getMeshKey(), ":"));
                } else {
                    toastMsg("import fail");
                }*/

                break;
        }
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (resultCode != Activity.RESULT_OK || requestCode != REQUEST_CODE_GET_FILE)
            return;

        mPath = data.getStringExtra("path");
        btn_open.setVisibility(View.VISIBLE);
        tv_file_select.setText(mPath);

        tv_log.append("File selected: " + mPath + "\n");
        TelinkLog.d("select: " + mPath);
    }

}
