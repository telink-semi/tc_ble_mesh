/********************************************************************************************************
 * @file ConnectionTestActivity.java
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
package com.telink.ble.mesh.ui.test;

import android.os.Bundle;
import android.os.Handler;
import android.view.View;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.ble.GattRequest;
import com.telink.ble.mesh.core.ble.UUIDInfo;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.ConnectionFilter;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.GattConnectionEvent;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.parameter.GattConnectionParameters;
import com.telink.ble.mesh.ui.activity.BaseActivity;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.UUID;

public class ConnectionTestActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {
    private Handler handler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_connection_test);
        initTitle();
        findViewById(R.id.btn_connect).setOnClickListener(this);
        findViewById(R.id.btn_disconnect).setOnClickListener(this);
        findViewById(R.id.btn_send).setOnClickListener(this);

        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);
        TelinkMeshApplication.getInstance().addEventListener(GattConnectionEvent.EVENT_TYPE_CONNECT_SUCCESS, this);
    }


    private void initTitle() {
        enableBackNav(true);
        setTitle("On Off Test");
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        handler.removeCallbacksAndMessages(null);
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_connect:
                ConnectionFilter connectionFilter = new ConnectionFilter(ConnectionFilter.TYPE_MAC_ADDRESS, "D5:43:FC:D1:9D:A5");
                MeshService.getInstance().startGattConnection(new GattConnectionParameters(connectionFilter));
                break;

            case R.id.btn_disconnect:
                MeshService.getInstance().idle(true);
                break;

            case R.id.btn_send:
//                sendTestRequest();
                getVersion();
                break;

        }
    }

    private void getVersion() {
        GattRequest write = GattRequest.newInstance();
        write.serviceUUID = UUIDInfo.SERVICE_DEVICE_INFO;
        write.characteristicUUID = UUIDInfo.CHARACTERISTIC_FW_VERSION;
        write.type = GattRequest.RequestType.READ;
        write.callback = requestCallback;
        write.tag = "read version";
        MeshService.getInstance().sendGattRequest(write);
    }

    private void sendTestRequest() {

        GattRequest enableNotify = GattRequest.newInstance();
        enableNotify.serviceUUID = UUID.fromString("ae5d1e47-5c13-43a0-8635-82ad38a1381f");
        enableNotify.characteristicUUID = UUID.fromString("a3dd50bf-f7a7-4e99-838e-570a086c661b");
        enableNotify.type = GattRequest.RequestType.ENABLE_NOTIFY;
        enableNotify.tag = "enable-notify";
        MeshService.getInstance().sendGattRequest(enableNotify);

        GattRequest request = GattRequest.newInstance();
        request.serviceUUID = UUID.fromString("ae5d1e47-5c13-43a0-8635-82ad38a1381f");
        request.characteristicUUID = UUID.fromString("a3dd50bf-f7a7-4e99-838e-570a086c661b");
        request.descriptorUUID = UUIDInfo.DESCRIPTOR_CFG_UUID;
        request.data = new byte[]{0x01, 0x00};
        request.type = GattRequest.RequestType.WRITE_DESCRIPTOR;
        request.callback = requestCallback;
        request.tag = "write ccc";
        MeshService.getInstance().sendGattRequest(request);

        GattRequest write = GattRequest.newInstance();
        write.serviceUUID = UUID.fromString("ae5d1e47-5c13-43a0-8635-82ad38a1381f");
        write.characteristicUUID = UUID.fromString("a3dd50bf-f7a7-4e99-838e-570a086c661b");
        write.data = new byte[]{0x01};
        write.type = GattRequest.RequestType.WRITE;
        write.callback = requestCallback;
        write.tag = "write";
        MeshService.getInstance().sendGattRequest(write);


    }

    private GattRequest.Callback requestCallback = new GattRequest.Callback() {
        @Override
        public void success(GattRequest request, Object obj) {
            MeshLogger.d("success : " + request.tag);
        }

        @Override
        public void error(GattRequest request, String errorMsg) {
            MeshLogger.d("error : " + request.tag);
        }

        @Override
        public boolean timeout(GattRequest request) {
            MeshLogger.d("timeout : " + request.tag);
            return false;
        }
    };

    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(MeshEvent.EVENT_TYPE_DISCONNECTED)) {
            MeshLogger.d(TAG + " disconnected");
        } else if (event.getType().equals(GattConnectionEvent.EVENT_TYPE_CONNECT_SUCCESS)) {
            MeshLogger.d(TAG + " gatt connect");
        }
    }

}

