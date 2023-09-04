/********************************************************************************************************
 * @file ShareExportFragment.java
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

package com.telink.ble.mesh.ui.fragment;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.MeshNetKey;
import com.telink.ble.mesh.model.json.MeshStorageService;
import com.telink.ble.mesh.ui.JsonPreviewActivity;
import com.telink.ble.mesh.ui.adapter.MeshKeySelectAdapter;
import com.telink.ble.mesh.ui.cdtp.CdtpExportToGatewayActivity;
import com.telink.ble.mesh.ui.cdtp.CdtpExportToPhoneActivity;
import com.telink.ble.mesh.ui.qrcode.QRCodeShareActivity;
import com.telink.ble.mesh.util.FileSystem;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

/**
 * @see com.telink.ble.mesh.ui.ShareExportActivity
 * share export
 * @deprecated
 */
public class ShareExportFragment extends BaseFragment implements View.OnClickListener {

    private TextView tv_log;
    private RadioGroup rg_share_type;
    private RadioButton rb_file, rb_qrcode, rb_cdtp_to_phone, rb_cdtp_to_gw;
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
        btn_open.setVisibility(View.GONE);
        tv_log = view.findViewById(R.id.tv_log);
        exportDir = FileSystem.getSettingPath();
//        checkFile();
        initView(view);
    }

    @SuppressLint("ClickableViewAccessibility")
    private void initView(View parent) {
        rg_share_type = parent.findViewById(R.id.rg_share_type);
        rb_file = parent.findViewById(R.id.rb_file);
        rb_qrcode = parent.findViewById(R.id.rb_qrcode);
        rb_cdtp_to_phone = parent.findViewById(R.id.rb_cdtp_to_phone);
        rb_cdtp_to_gw = parent.findViewById(R.id.rb_cdtp_to_gw);

        rb_file.setOnTouchListener(TOUCH_LISTENER);
        rb_qrcode.setOnTouchListener(TOUCH_LISTENER);
        rb_cdtp_to_phone.setOnTouchListener(TOUCH_LISTENER);
        rb_cdtp_to_gw.setOnTouchListener(TOUCH_LISTENER);

        RecyclerView rv_net_key_select = parent.findViewById(R.id.rv_net_key_select);
        rv_net_key_select.setLayoutManager(new LinearLayoutManager(getActivity()));
        MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        adapter = new MeshKeySelectAdapter<>(getActivity(), meshInfo.meshNetKeyList);
        rv_net_key_select.setAdapter(adapter);
    }

    @SuppressLint("ClickableViewAccessibility")
    private final View.OnTouchListener TOUCH_LISTENER = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            Drawable drawableRight = ((TextView) v).getCompoundDrawables()[2];
            if (event.getAction() == MotionEvent.ACTION_UP && event.getRawX() >= (v.getRight() - drawableRight.getBounds().width())) {
                toastMsg("phone");
                if (v == rb_file) {

                } else if (v == rb_qrcode) {

                } else if (v == rb_cdtp_to_phone) {

                } else if (v == rb_cdtp_to_gw) {

                }
                return true;
            }
            return false;
        }
    };

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

                /*if(meshInfo.nodes.size() == 0){
                    toastMsg("not allow to share empty network");
                    return;
                }*/
                List<MeshNetKey> selectedKeys = getSelectedNetKeys();
                if (selectedKeys.size() == 0) {
                    toastMsg("select at least one net key");
                    return;
                }

                switch (rg_share_type.getCheckedRadioButtonId()) {
                    case R.id.rb_file:
                        exportToFile(selectedKeys);
                        break;
                    case R.id.rb_qrcode:
                        exportToQrcode(selectedKeys);
                        break;
                    case R.id.rb_cdtp_to_phone:
                        exportToPhone(selectedKeys);
                        break;
                    case R.id.rb_cdtp_to_gw:
                        exportToGw(selectedKeys);
                        break;
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

    private void exportToFile(List<MeshNetKey> selectedKeys) {
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
    }

    private void exportToQrcode(List<MeshNetKey> selectedKeys) {
        startActivity(new Intent(getActivity(), QRCodeShareActivity.class).putExtra("selectedIndexes",
                getIndexes(selectedKeys)));
    }

    private void exportToPhone(List<MeshNetKey> selectedKeys) {
        startActivity(new Intent(getActivity(), CdtpExportToPhoneActivity.class).putExtra("selectedIndexes", getIndexes(selectedKeys)));
    }


    private void exportToGw(List<MeshNetKey> selectedKeys) {
        startActivity(new Intent(getActivity(), CdtpExportToGatewayActivity.class).putExtra("selectedIndexes", getIndexes(selectedKeys)));
    }

    private int[] getIndexes(List<MeshNetKey> selectedKeys) {
        int[] keyIndexes = new int[selectedKeys.size()];
        for (int i = 0; i < keyIndexes.length; i++) {
            keyIndexes[i] = selectedKeys.get(i).index;
        }
        return keyIndexes;
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
