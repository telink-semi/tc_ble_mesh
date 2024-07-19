/********************************************************************************************************
 * @file CertListActivity.java
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
//package com.telink.ble.mesh.ui;
//
//import android.app.Activity;
//import android.content.Intent;
//import android.os.Bundle;
//import android.view.MenuItem;
//import android.widget.Toast;
//
//import androidx.annotation.Nullable;
//import androidx.appcompat.app.AlertDialog;
//import androidx.appcompat.widget.Toolbar;
//import androidx.recyclerview.widget.LinearLayoutManager;
//import androidx.recyclerview.widget.RecyclerView;
//
//import com.telink.ble.mesh.demo.R;
//import com.telink.ble.mesh.model.CertCacheService;
//import com.telink.ble.mesh.ui.adapter.BaseRecyclerViewAdapter;
//import com.telink.ble.mesh.ui.adapter.CertListAdapter;
//import com.telink.ble.mesh.ui.file.FileSelectActivity;
//import com.telink.ble.mesh.util.MeshLogger;
//
//import java.io.ByteArrayInputStream;
//import java.io.File;
//import java.io.FileInputStream;
//import java.io.IOException;
//import java.security.cert.CertificateFactory;
//import java.security.cert.X509Certificate;
//import java.util.ArrayList;
//import java.util.List;
//
///**
// * show static OOB list
// */
//
//public class CertListActivity extends BaseActivity {
//    private static final int REQUEST_CODE_SELECT_CERT = 1;
//
//    private CertListAdapter mAdapter;
//    private List<byte[]> certDataList = new ArrayList<>();
//    private List<X509Certificate> certificateList = new ArrayList<>();
//
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        if (!validateNormalStart(savedInstanceState)) {
//            return;
//        }
//        setContentView(R.layout.activity_oob_info);
//        setTitle("Cert List");
//        enableBackNav(true);
//        Toolbar toolbar = findViewById(R.id.title_bar);
//        toolbar.inflateMenu(R.menu.cert_list);
//        toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
//            @Override
//            public boolean onMenuItemClick(MenuItem item) {
//                if (item.getItemId() == R.id.item_cert_add) {
//                    startActivityForResult(new Intent(CertListActivity.this, FileSelectActivity.class).putExtra(FileSelectActivity.KEY_SUFFIX, "der")
//                                    .putExtra(FileSelectActivity.KEY_TITLE, "select cert(.der)")
//                            , REQUEST_CODE_SELECT_CERT);
//                } else if (item.getItemId() == R.id.item_cert_clear) {
//                    showClearDialog();
//                }
//                return false;
//            }
//        });
//
//        parseCacheCerts();
//        int rootIndex = CertCacheService.getInstance().getRootIndex();
//        mAdapter = new CertListAdapter(this, certificateList, rootIndex);
//        mAdapter.setOnItemClickListener(new BaseRecyclerViewAdapter.OnItemClickListener() {
//            @Override
//            public void onItemClick(int position) {
//                startActivityForResult(
//                        new Intent(CertListActivity.this, CertDetailActivity.class)
//                                .putExtra(CertDetailActivity.KEY_EXTRA_CERT_INFO, certificateList.get(position))
//                                .putExtra(CertDetailActivity.KEY_EXTRA_CERT_DATA, certDataList.get(position))
//                        , OOBInfoActivity.REQUEST_CODE_EDIT_OOB
//                );
//            }
//        });
//        mAdapter.setOnItemLongClickListener(position -> {
//            showActionsDialog(position);
//            return false;
//        });
//        RecyclerView rv_oob = findViewById(R.id.rv_oob);
//        rv_oob.setLayoutManager(new LinearLayoutManager(this));
//        rv_oob.setAdapter(mAdapter);
//    }
//
//    private void parseCacheCerts() {
//        certDataList.addAll(CertCacheService.getInstance().get());
//        for (byte[] certData : certDataList) {
//            try {
//                CertificateFactory factory = CertificateFactory.getInstance("X.509");
//                X509Certificate certificate = (X509Certificate) factory.generateCertificate(new ByteArrayInputStream(certData));
//                certificateList.add(certificate);
//            } catch (Exception e) {
//                e.printStackTrace();
//            }
//        }
//    }
//
//    private void showActionsDialog(final int position) {
//        AlertDialog.Builder deleteDialog = new AlertDialog.Builder(this);
//        final boolean root = position == CertCacheService.getInstance().getRootIndex();
//        String[] title = new String[]{"delete cert", "set as ROOT cert" + (root ? "(cancel)" : "")};
//        deleteDialog.setTitle("select action at: " + position);
//        deleteDialog.setItems(title, (dialog, which) -> {
//            if (which == 0) {
//                // delete
//                CertCacheService.getInstance().delete(getApplicationContext(), position);
//                certDataList.remove(position);
//                certificateList.remove(position);
//                mAdapter.updateRootIndex(CertCacheService.getInstance().getRootIndex());
//                mAdapter.notifyDataSetChanged();
//            } else if (which == 1) {
//                // set root
//                CertCacheService.getInstance().setRootIndex(getApplicationContext(), position);
//                mAdapter.updateRootIndex(root ? -1 : position);
//            }
//            dialog.dismiss();
//        });
//        deleteDialog.show();
//    }
//
//
//    private void showClearDialog() {
//        showConfirmDialog("Delete all certs? ", (dialog, which) -> {
//            certDataList.clear();
//            certificateList.clear();
//            CertCacheService.getInstance().clear(getApplicationContext());
//            mAdapter.notifyDataSetChanged();
//        });
//    }
//
//
//    @Override
//    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
//        super.onActivityResult(requestCode, resultCode, data);
//        if (resultCode != Activity.RESULT_OK || data == null)
//            return;
//        if (requestCode == REQUEST_CODE_SELECT_CERT) {
//            final String path = data.getStringExtra(FileSelectActivity.KEY_RESULT);
//            MeshLogger.log("select: " + path);
//            X509Certificate parseResult = parseCert(path);
//            if (parseResult != null) {
//                certificateList.add(parseResult);
//                mAdapter.notifyDataSetChanged();
//                Toast.makeText(CertListActivity.this, "Cert Import Success", Toast.LENGTH_SHORT).show();
//            } else {
//                Toast.makeText(CertListActivity.this, "Cert Import Fail: check the file format", Toast.LENGTH_SHORT).show();
//            }
//        }
//    }
//
//
//    /**
//     * parse oob database
//     */
//    public X509Certificate parseCert(String filePath) {
//        if (filePath == null) return null;
//        File file = new File(filePath);
//        if (!file.exists())
//            return null;
//        FileInputStream is = null;
//        try {
//            is = new FileInputStream(file);
//            byte[] certData = new byte[is.available()];
//            is.read(certData);
//            CertificateFactory factory = CertificateFactory.getInstance("X.509");
//            X509Certificate certificate = (X509Certificate) factory.generateCertificate(new ByteArrayInputStream(certData));
//            if (certificate != null) {
//                certDataList.add(certData);
//                CertCacheService.getInstance().addCert(this, certData);
//            }
//            return certificate;
//        } catch (Exception e) {
//            e.printStackTrace();
//        } finally {
//            if (is != null) {
//                try {
//                    is.close();
//                } catch (IOException e) {
//                    e.printStackTrace();
//                }
//            }
//        }
//        return null;
//    }
//
//}
