package com.telink.ble.mesh.ui;

import android.os.Bundle;
import android.os.Handler;
import android.view.MenuItem;
import android.view.View;
import android.widget.CheckBox;
import android.widget.Toast;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.Scene;
import com.telink.ble.mesh.ui.adapter.BaseSelectableListAdapter;
import com.telink.ble.mesh.ui.adapter.DeviceSelectAdapter;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.config.ConfigMessage;
import com.telink.ble.mesh.core.message.scene.SceneDeleteMessage;
import com.telink.ble.mesh.core.message.scene.SceneRegisterStatusMessage;
import com.telink.ble.mesh.core.message.scene.SceneStoreMessage;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.util.TelinkLog;

import java.util.ArrayList;
import java.util.List;

import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * Scene Setting
 * Created by kee on 2018/9/18.
 */
public class SceneSettingActivity extends BaseActivity implements View.OnClickListener, BaseSelectableListAdapter.SelectStatusChangedListener, EventListener<String> {

    //    private GroupSelectAdapter mGroupAdapter;
    private DeviceSelectAdapter mDeviceAdapter;
    //    private int sceneId;
    private Scene scene;
    private MeshInfo mesh;
    private List<NodeInfo> devices;
    //    private List<Group> groups;
    private CheckBox cb_device, cb_group;

    private Handler delayHandler = new Handler();
    /**
     * add or remove from list
     */
    private List<SettingModel> selectedAdrList;
    private int settingIndex;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_scene_setting);
        mesh = TelinkMeshApplication.getInstance().getMeshInfo();

        int sceneId = getIntent().getIntExtra("sceneId", -1);
        if (sceneId == -1) {
            sceneId = mesh.allocSceneId();
            if (sceneId == -1) {
                finish();
                Toast.makeText(getApplicationContext(), "no available scene id", Toast.LENGTH_SHORT).show();
                return;
            }
            scene = new Scene();
            scene.id = sceneId;
        } else {
            scene = mesh.getSceneById(sceneId);
        }


        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.check);
        toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                if (item.getItemId() == R.id.item_check) {
                    saveStart();
                }
                return false;
            }
        });
        setTitle("Scene Setting");
        enableBackNav(true);
        cb_device = findViewById(R.id.cb_device);
        cb_group = findViewById(R.id.cb_group);
        RecyclerView rv_device = findViewById(R.id.rv_device);

        rv_device.setLayoutManager(new LinearLayoutManager(this));
        if (mesh.nodes != null) {
            for (NodeInfo deviceInfo : mesh.nodes) {
                deviceInfo.selected = scene.contains(deviceInfo);
            }
        }
        devices = mesh.nodes;
        mDeviceAdapter = new DeviceSelectAdapter(this, devices);
        mDeviceAdapter.setStatusChangedListener(this);
        rv_device.setAdapter(mDeviceAdapter);
        cb_device.setChecked(mDeviceAdapter.allSelected());
        findViewById(R.id.btn_save_scene).setOnClickListener(this);
        findViewById(R.id.cb_device).setOnClickListener(this);
        findViewById(R.id.cb_group).setOnClickListener(this);

        // mesh interface
        TelinkMeshApplication.getInstance().addEventListener(SceneRegisterStatusMessage.class.getName(), this);
//        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_SCENE_REGISTER_STATUS, this);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        delayHandler.removeCallbacksAndMessages(null);
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_save_scene:
                saveStart();
                break;

            case R.id.cb_device:
                mDeviceAdapter.setAll(!mDeviceAdapter.allSelected());
                break;

            case R.id.cb_group:
//                mGroupAdapter.setAll(!mGroupAdapter.allSelected());
                break;
        }
    }

    private void setNextAddress() {

        if (settingIndex > selectedAdrList.size() - 1) {
            dismissWaitingDialog();
            toastMsg("store scene complete");
//            finish();
            return;
        }
        SettingModel model = selectedAdrList.get(settingIndex);

        TelinkLog.d("set next address: " + model.address);

        // F0:0D:02:00:01:00:82:45:00:01:00:01:00:37:00
        delayHandler.removeCallbacks(cmdTimeoutCheckTask);
        delayHandler.postDelayed(cmdTimeoutCheckTask, 2000);

        int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
        MeshMessage meshMessage;
        if (model.add) {
            meshMessage = SceneStoreMessage.getSimple(model.address,
                    appKeyIndex,
                    scene.id,
                    true, 1);
        } else {
            meshMessage = SceneDeleteMessage.getSimple(model.address,
                    appKeyIndex,
                    scene.id,
                    true, 1);
        }
        MeshService.getInstance().sendMeshMessage(meshMessage);
    }

    private Runnable cmdTimeoutCheckTask = new Runnable() {
        @Override
        public void run() {
            settingIndex++;
            setNextAddress();
        }
    };

    private void saveStart() {
        selectedAdrList = new ArrayList<>();
        if (devices != null) {
            int adr;
            for (NodeInfo deviceInfo : devices) {
                if (deviceInfo.getOnOff() == -1) {
                    continue;
                }

                adr = deviceInfo.getTargetEleAdr(MeshSigModel.SIG_MD_SCENE_S.modelId);
                if (adr == -1) {
                    TelinkLog.d("scene save: device check fail");
                    continue;
                }

                if (!deviceInfo.selected) {
                    if (scene.contains(deviceInfo))
                        selectedAdrList.add(new SettingModel(adr, false));
                } else {
                    selectedAdrList.add(new SettingModel(adr, true));
                }
            }
        }
        if (selectedAdrList.size() == 0) {
            toastMsg("select at least one item !");
        } else {
            showWaitingDialog("setting...");
            settingIndex = 0;
            setNextAddress();
        }
    }

    @Override
    public void onStatusChanged(BaseSelectableListAdapter adapter) {
        if (adapter == mDeviceAdapter) {
            cb_device.setChecked(mDeviceAdapter.allSelected());
        }
    }

    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(SceneRegisterStatusMessage.class.getName())) {
            StatusNotificationEvent statusNotificationEvent = (StatusNotificationEvent) event;
            NotificationMessage notificationMessage = statusNotificationEvent.getNotificationMessage();
            SceneRegisterStatusMessage sceneRegisterStatusMessage = (SceneRegisterStatusMessage) notificationMessage.getStatusMessage();
            if (sceneRegisterStatusMessage.getStatusCode() == ConfigMessage.STATUS_SUCCESS
                    && notificationMessage.getSrc() == selectedAdrList.get(settingIndex).address) {
                delayHandler.removeCallbacks(cmdTimeoutCheckTask);
                NodeInfo deviceInfo = mesh.getDeviceByMeshAddress(notificationMessage.getSrc());
                if (selectedAdrList.get(settingIndex).add) {
                    scene.saveFromDeviceInfo(deviceInfo);
                } else {
                    scene.removeByAddress(deviceInfo.meshAddress);
                }
                mesh.saveScene(scene);
                mesh.saveOrUpdate(this);
                settingIndex++;
                setNextAddress();
            }
        }
    }

    class SettingModel {
        int address;
        boolean add;

        SettingModel(int address, boolean add) {
            this.address = address;
            this.add = add;
        }
    }
}
