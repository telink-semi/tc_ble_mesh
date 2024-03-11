/********************************************************************************************************
 * @file CertDetailActivity.java
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
package com.telink.ble.mesh.ui.activity;

import android.os.Bundle;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.util.Arrays;

import java.security.cert.X509Certificate;

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
