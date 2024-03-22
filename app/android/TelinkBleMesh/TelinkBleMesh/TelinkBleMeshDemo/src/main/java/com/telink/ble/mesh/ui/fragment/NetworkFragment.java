/********************************************************************************************************
 * @file NetworkFragment.java
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

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.appcompat.widget.Toolbar;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.Encipher;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.ble.BleAdvertiser;
import com.telink.ble.mesh.core.networking.SolicitationPDU;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.ui.DeviceBatchSettingActivity;
import com.telink.ble.mesh.ui.DirectForwardingListActivity;
import com.telink.ble.mesh.ui.FUActivity;
import com.telink.ble.mesh.ui.MeshInfoActivity;
import com.telink.ble.mesh.ui.PrivateBeaconSettingActivity;
import com.telink.ble.mesh.ui.SceneListActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.nio.ByteBuffer;

/**
 * setting
 */
public class NetworkFragment extends BaseFragment implements View.OnClickListener, EventListener<String> {
    private TextView tv_mesh_name;
    private Handler delayHandler = new Handler();
    private BleAdvertiser advertiser;
    private ProgressBar pb_sol;
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_network, null);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        setTitle(view, "Network");
        Toolbar toolbar = view.findViewById(R.id.title_bar);
        toolbar.setNavigationIcon(null);
        tv_mesh_name = view.findViewById(R.id.tv_mesh_name);
        view.findViewById(R.id.view_scene_setting).setOnClickListener(this);
        view.findViewById(R.id.view_mesh_info).setOnClickListener(this);
        view.findViewById(R.id.view_mesh_ota).setOnClickListener(this);
        view.findViewById(R.id.view_df).setOnClickListener(this);
        view.findViewById(R.id.view_sol).setOnClickListener(this);
        view.findViewById(R.id.view_node_batch).setOnClickListener(this);
        pb_sol = view.findViewById(R.id.pb_sol);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_MESH_RESET, this);
    }

    @Override
    public void onResume() {
        super.onResume();
        MeshLogger.d("network fragment - onResume");
        showMeshName();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (isSoliciting && advertiser != null) {
            advertiser.stopAdvertising();
        }
    }

    private void showMeshName() {
        tv_mesh_name.setText(TelinkMeshApplication.getInstance().getMeshInfo().meshName);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {

            case R.id.view_scene_setting:
                startActivity(new Intent(getActivity(), SceneListActivity.class));
                break;

            case R.id.view_mesh_ota:
                startActivity(new Intent(getActivity(), FUActivity.class));
                break;

            case R.id.view_mesh_info:
                startActivity(new Intent(getActivity(), MeshInfoActivity.class));
                break;

            case R.id.view_df:
                startActivity(new Intent(getActivity(), DirectForwardingListActivity.class));
                break;
            case R.id.view_sol:
                if (isSoliciting) {
                    toastMsg("already soliciting ...");
                    return;
                }
                toastMsg("start solicitation");
                startSolicitation();
                break;

            case R.id.view_node_batch:
                startActivity(new Intent(getActivity(), DeviceBatchSettingActivity.class));
                break;
        }
    }

    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(MeshEvent.EVENT_TYPE_MESH_RESET)) {
            showMeshName();
        }
    }


    private void startSolicitation() {
        pb_sol.setVisibility(View.VISIBLE);
        if (advertiser == null) {
            advertiser = new BleAdvertiser();
        }
        isSoliciting = true;
        delayHandler.removeCallbacks(solSettingTimeoutTask);
        long timeout = 2 * 60 * 1000; //  2 min
//        long timeout = 10 * 1000; //  2 min
        delayHandler.postDelayed(solSettingTimeoutTask, timeout);
        advertiser.startAdvertise(MeshUtils.SOL_UUID, buildSolData(), timeout);
    }

    private byte[] buildSolData() {
        byte IdentificationType = 0x00;
        byte[] networkKey = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultNetKey().key;
        byte[][] k2Output = Encipher.calculateNetKeyK2(networkKey);
        byte[] encryptionKey = k2Output[1];
        byte[] privacyKey = k2Output[2];
        byte nid = (byte) (k2Output[0][15] & 0x7F);
        SolicitationPDU pdu = createSolicitationPDU(encryptionKey, privacyKey, nid);
        byte[] encData = pdu.generateEncryptedPayload();
        MeshLogger.d("enc data - " + Arrays.bytesToHexString(encData));
        return ByteBuffer.allocate(1 + encData.length).put(IdentificationType).put(encData).array();
    }

    private SolicitationPDU createSolicitationPDU(
            byte[] encryptionKey, byte[] privacyKey, byte nid) {
        SolicitationPDU pdu = new SolicitationPDU(new SolicitationPDU.SolicitationEncryptionSuite(encryptionKey, privacyKey, nid));
        MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        int src = meshInfo.localAddress;
        int dst = MeshUtils.ADDRESS_ALL_PROXY;
        pdu.setNid(nid);
        pdu.setSeq(meshInfo.getSolSeq());
        pdu.setSrc(src);
        pdu.setDst(dst);
        return pdu;
    }

    private boolean isSoliciting = false;

    private Runnable solSettingTimeoutTask = () -> {
        isSoliciting = false;
        pb_sol.setVisibility(View.GONE);
    };

}
