package com.telink.ble.mesh.ui.test;

import android.os.Bundle;
import android.view.View;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.ble.GattRequest;
import com.telink.ble.mesh.core.ble.UUIDInfo;
import com.telink.ble.mesh.core.message.generic.OnOffStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.GattConnectionEvent;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.parameter.GattConnectionParameters;
import com.telink.ble.mesh.ui.BaseActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

public class ConnectionTestActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {

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
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_connect:
                MeshService.getInstance().startGattConnection(new GattConnectionParameters(8));
                break;

            case R.id.btn_disconnect:
                MeshService.getInstance().idle(true);
                break;

            case R.id.btn_send:
                GattRequest request = GattRequest.newInstance();
                request.characteristicUUID = UUIDInfo.CHARACTERISTIC_FW_VERSION;
                request.serviceUUID = UUIDInfo.SERVICE_DEVICE_INFO;
                request.type = GattRequest.RequestType.READ;
                request.callback = new GattRequest.Callback() {
                    @Override
                    public void success(GattRequest request, Object obj) {
                        MeshLogger.d("success" + Arrays.bytesToHexString((byte[]) obj));
                    }

                    @Override
                    public void error(GattRequest request, String errorMsg) {
                        MeshLogger.d("error: " + errorMsg);
                    }

                    @Override
                    public boolean timeout(GattRequest request) {
                        MeshLogger.d("timeout");
                        return false;
                    }
                };
                MeshService.getInstance().sendGattRequest(request);
                break;

        }
    }

    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(MeshEvent.EVENT_TYPE_DISCONNECTED)){
            MeshLogger.d(TAG + " disconnected");
        }else if (event.getType().equals(GattConnectionEvent.EVENT_TYPE_CONNECT_SUCCESS)){
            MeshLogger.d(TAG + " gatt connect");
        }
    }

}

