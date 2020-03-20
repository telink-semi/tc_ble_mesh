package com.telink.ble.mesh.ui;

import android.os.Bundle;
import android.widget.TextView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.entity.AdvertisingDevice;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.event.ProvisioningEvent;
import com.telink.ble.mesh.foundation.event.ScanEvent;
import com.telink.ble.mesh.foundation.parameter.ScanParameters;

/**
 * remote provision
 * actions:
 * 1.scan -- success ->
 * 2.connect -- success ->
 * 3.provision -- success ->
 * 4.KeyBind -- success ->
 * send scan
 * Created by kee on 2019/3/25.
 */

// todo mesh interface
// implements View.OnClickListener, EventListener<String>
public class RPActivity extends BaseActivity implements EventListener<String> {

    private TextView tvInfo;
    private int bridgeNodeAddress = 0;

    /**
     * proxy connected
     */
    private boolean proxyConnected = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        tvInfo = new TextView(this);
        tvInfo.setText("init");
        setContentView(tvInfo);
        MeshService.getInstance().idle(false);

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    private void actionStart() {
        if (MeshService.getInstance().isProxyLogin()) {
            proxyConnected = true;
            startRemoteScan();
        } else {
            startScan();
        }
    }

    /**
     * scanning for unprovisioned device
     */
    private void startScan() {
        ScanParameters scanParameters = ScanParameters.getDefault(false, true);
        MeshService.getInstance().startScan(scanParameters);

    }

    private void onDeviceFound(ScanEvent scanEvent) {
        AdvertisingDevice device = scanEvent.getAdvertisingDevice();

    }

    private void startRemoteScan() {

    }

    private void appendInfo(String text) {
        tvInfo.append("\n" + text);
    }

    @Override
    public void performed(Event<String> event) {
        String eventType = event.getType();
        if (eventType.equals(MeshEvent.EVENT_TYPE_DISCONNECTED)) {

        } else if (eventType.equals(ProvisioningEvent.EVENT_TYPE_PROVISION_SUCCESS)) {

        } else if (eventType.equals(ScanEvent.EVENT_TYPE_DEVICE_FOUND)) {
            onDeviceFound((ScanEvent) event);
        }
    }
}
