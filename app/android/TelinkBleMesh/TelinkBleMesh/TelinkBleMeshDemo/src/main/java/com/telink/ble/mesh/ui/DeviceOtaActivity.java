package com.telink.ble.mesh.ui;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.GattOtaEvent;
import com.telink.ble.mesh.foundation.parameter.GattOtaParameters;
import com.telink.ble.mesh.util.TelinkLog;
import com.telink.file.selector.FileSelectActivity;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 */

public class DeviceOtaActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {

//    private Button btn_back;

    private TextView selectFile, info, progress, tv_version_info;
    private Button startOta;

    //    private String mPath;
    private byte[] mFirmware;
    private int meshAddress;

    private final static int REQUEST_CODE_GET_FILE = 1;
    private final static int MSG_PROGRESS = 11;
    private final static int MSG_INFO = 12;


    private Handler mInfoHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == MSG_PROGRESS) {
                progress.setText(msg.obj + "%");
            } else if (msg.what == MSG_INFO) {
                info.append("\n" + msg.obj);
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_device_ota);
        setTitle("OTA");
        enableBackNav(true);
        initViews();

        Intent intent = getIntent();
        if (intent.hasExtra("meshAddress")) {
            meshAddress = intent.getIntExtra("meshAddress", -1);
        } else {
            toastMsg("device error");
            finish();
            return;
        }
        // todo mesh interface
        TelinkMeshApplication.getInstance().addEventListener(GattOtaEvent.EVENT_TYPE_OTA_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(GattOtaEvent.EVENT_TYPE_OTA_PROGRESS, this);
        TelinkMeshApplication.getInstance().addEventListener(GattOtaEvent.EVENT_TYPE_OTA_FAIL, this);

        MeshService.getInstance().idle(false);
    }


    private void initViews() {
        selectFile = findViewById(R.id.selectFile);
        info = findViewById(R.id.log);
        progress = findViewById(R.id.progress);
        tv_version_info = findViewById(R.id.tv_version_info);
        tv_version_info.setText(getString(R.string.version, "null"));

        startOta = findViewById(R.id.startOta);

        selectFile.setOnClickListener(this);
        startOta.setOnClickListener(this);

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {

            case R.id.startOta:

                if (mFirmware == null) {
//                    toastMsg("firmware error");
                    toastMsg("select firmware!");
                    return;
                }

                info.setText("start OTA");

                progress.setText("");
                startOta.setEnabled(false);
                GattOtaParameters parameters = new GattOtaParameters(meshAddress, mFirmware);
                MeshService.getInstance().startGattOta(parameters);
//                mDevice.startGattOta(firmware);
                break;

            case R.id.selectFile:
                startActivityForResult(new Intent(this, FileSelectActivity.class).putExtra(FileSelectActivity.KEY_SUFFIX, ".bin"), REQUEST_CODE_GET_FILE);
                break;
        }
    }

    private void onOtaComplete() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                startOta.setEnabled(true);
            }
        });
    }

    @Override
    public void performed(final Event<String> event) {
        switch (event.getType()) {
            case GattOtaEvent.EVENT_TYPE_OTA_SUCCESS:
                MeshService.getInstance().idle(false);
                mInfoHandler.obtainMessage(MSG_INFO, "OTA_SUCCESS").sendToTarget();
                onOtaComplete();
                break;

            case GattOtaEvent.EVENT_TYPE_OTA_FAIL:
                MeshService.getInstance().idle(true);
                mInfoHandler.obtainMessage(MSG_INFO, "OTA_FAIL").sendToTarget();
                onOtaComplete();
                break;


            case GattOtaEvent.EVENT_TYPE_OTA_PROGRESS:
                int progress = ((GattOtaEvent) event).getProgress();
                mInfoHandler.obtainMessage(MSG_PROGRESS, progress).sendToTarget();
                break;
        }

    }

    private void readFirmware(String fileName) {
        try {
            InputStream stream = new FileInputStream(fileName);
            int length = stream.available();
            mFirmware = new byte[length];
            stream.read(mFirmware);
            stream.close();

            byte[] version = new byte[4];
            System.arraycopy(mFirmware, 2, version, 0, 4);
            String firmVersion = new String(version);
            tv_version_info.setText(getString(R.string.version, firmVersion));
            selectFile.setText(fileName);
        } catch (IOException e) {
            e.printStackTrace();
            mFirmware = null;
            tv_version_info.setText(getString(R.string.version, "null"));
            selectFile.setText("file error");
        }
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (resultCode != Activity.RESULT_OK || requestCode != REQUEST_CODE_GET_FILE)
            return;

        String mPath = data.getStringExtra(FileSelectActivity.KEY_RESULT);
        TelinkLog.d("select: " + mPath);
//        File f = new File(mPath);
        readFirmware(mPath);
    }
}
