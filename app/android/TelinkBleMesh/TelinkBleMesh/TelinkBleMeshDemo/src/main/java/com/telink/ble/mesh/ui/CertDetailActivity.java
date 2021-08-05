/********************************************************************************************************
 * @file OOBInfoActivity.java
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
package com.telink.ble.mesh.ui;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.MenuItem;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.OOBPair;
import com.telink.ble.mesh.ui.adapter.BaseRecyclerViewAdapter;
import com.telink.ble.mesh.ui.adapter.CertListAdapter;
import com.telink.ble.mesh.ui.file.FileSelectActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.util.ArrayList;
import java.util.List;

/**
 * show static OOB list
 */

public class CertDetailActivity extends BaseActivity {

    public static final String KEY_EXTRA_CERT_DATA = "CERT_DATA";

    public static final String KEY_EXTRA_CERT_INFO = "CERT_INFO";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_cert_detail);
        setTitle("Cert Detail");
        enableBackNav(true);
        TextView tv_cert_detail = findViewById(R.id.tv_cert_detail);
        X509Certificate certificate = (X509Certificate) getIntent().getSerializableExtra(KEY_EXTRA_CERT_INFO);
        if (certificate == null) {
            tv_cert_detail.append("cert info error");
            return;
        }
        byte[] certData = getIntent().getByteArrayExtra(KEY_EXTRA_CERT_DATA);
        tv_cert_detail.append("raw: " + Arrays.bytesToHexString(certData) + "\n");
        tv_cert_detail.append(certificate.toString());

    }
}
