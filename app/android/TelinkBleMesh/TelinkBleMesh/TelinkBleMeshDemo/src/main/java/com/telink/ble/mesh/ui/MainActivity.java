package com.telink.ble.mesh.ui;

import android.content.DialogInterface;
import android.os.Bundle;
import android.view.MenuItem;

import com.google.android.material.bottomnavigation.BottomNavigationView;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.generic.OnOffGetMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshConfiguration;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.AutoConnectEvent;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.parameter.AutoConnectParameters;
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.ui.fragment.DeviceFragment;
import com.telink.ble.mesh.ui.fragment.GroupFragment;
import com.telink.ble.mesh.ui.fragment.SettingFragment;
import com.telink.ble.mesh.util.MeshLogger;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;

/**
 * MainActivity include DeviceFragment & GroupFragment
 * Created by kee on 2017/8/18.
 */

public class MainActivity extends BaseActivity implements BottomNavigationView.OnNavigationItemSelectedListener, EventListener<String> {

    private FragmentManager fm;
    private DeviceFragment deviceFragment;
    private Fragment groupFragment;
    private SettingFragment settingFragment;
    private AlertDialog.Builder mDialogBuilder;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        startMeshService();
        setContentView(R.layout.activity_main);
        BottomNavigationView bottomNavigationView = findViewById(R.id.bottom_nav);
        bottomNavigationView.setOnNavigationItemSelectedListener(this);
        fm = getSupportFragmentManager();
        deviceFragment = new DeviceFragment();
        groupFragment = new GroupFragment();
        settingFragment = new SettingFragment();
        fm.beginTransaction()
                .add(R.id.fl_container, deviceFragment).add(R.id.fl_container, groupFragment).add(R.id.fl_container, settingFragment)
                .show(deviceFragment).hide(groupFragment).hide(settingFragment)
                .commit();

        MeshInfo mesh = TelinkMeshApplication.getInstance().getMeshInfo();
        if (mesh.nodes != null) {
            for (NodeInfo deviceInfo : mesh.nodes) {
                deviceInfo.setOnOff(-1);
                deviceInfo.lum = 0;
                deviceInfo.temp = 0;
            }
        }

        TelinkMeshApplication.getInstance().addEventListener(AutoConnectEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);

        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_MESH_EMPTY, this);
    }


    private void startMeshService() {
        MeshService.getInstance().init(this, TelinkMeshApplication.getInstance());
        MeshConfiguration meshConfiguration = TelinkMeshApplication.getInstance().getMeshInfo().convertToConfiguration();
        MeshService.getInstance().setupMeshNetwork(meshConfiguration);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
        MeshService.getInstance().clear();
    }


    @Override
    protected void onResume() {
        super.onResume();
        if (MeshService.getInstance().isBluetoothEnabled()) {
            this.autoConnect();
        } else {
            showBleOpenDialog();
        }
    }


    private void autoConnect() {
        MeshLogger.log("main auto connect");
//        MeshService.getInstance().autoConnect(new AutoConnectParameters(AutoConnectFilterType.NODE_IDENTITY));
        MeshService.getInstance().autoConnect(new AutoConnectParameters());
    }


    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(MeshEvent.EVENT_TYPE_MESH_EMPTY)) {
            MeshLogger.log(TAG + "#EVENT_TYPE_MESH_EMPTY");
        } else if (event.getType().equals(AutoConnectEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN)) {
            // get all device on off status when auto connect success
            AppSettings.ONLINE_STATUS_ENABLE = MeshService.getInstance().getOnlineStatus();
            if (!AppSettings.ONLINE_STATUS_ENABLE) {
                MeshService.getInstance().getOnlineStatus();
                int rspMax = TelinkMeshApplication.getInstance().getMeshInfo().getOnlineCountInAll();
                int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
                OnOffGetMessage message = OnOffGetMessage.getSimple(0xFFFF, appKeyIndex, rspMax);
                MeshService.getInstance().sendMeshMessage(message);
            }
//            } else {
//                MeshLogger.log("online status enabled");
//            }
        }
    }

    @Override
    public boolean onNavigationItemSelected(@NonNull MenuItem item) {
        switch (item.getItemId()) {
            case R.id.item_device:
                fm.beginTransaction().hide(groupFragment).hide(settingFragment).show(deviceFragment).commit();
                break;
            case R.id.item_group:
                fm.beginTransaction().hide(deviceFragment).hide(settingFragment).show(groupFragment).commit();
                break;
            case R.id.item_setting:
                fm.beginTransaction().hide(deviceFragment).hide(groupFragment).show(settingFragment).commit();
                break;

        }
        return true;
    }


    private void showBleOpenDialog() {
        if (mDialogBuilder == null) {
            mDialogBuilder = new AlertDialog.Builder(this);
            mDialogBuilder.setCancelable(false);
            mDialogBuilder.setMessage("Enable Bluetooth!");
            mDialogBuilder.setNegativeButton("cancel", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    finish();
                }
            });
            mDialogBuilder.setPositiveButton("enable", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    MeshService.getInstance().enableBluetooth();
                }
            });
        }

        mDialogBuilder.show();
    }

}
