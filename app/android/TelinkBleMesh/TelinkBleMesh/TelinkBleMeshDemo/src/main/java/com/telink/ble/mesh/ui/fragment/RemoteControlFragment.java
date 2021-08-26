/********************************************************************************************************
 * @file DeviceControlFragment.java
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
package com.telink.ble.mesh.ui.fragment;

import android.os.Bundle;
import android.text.TextUtils;
import android.util.SparseBooleanArray;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.config.ModelPublicationSetMessage;
import com.telink.ble.mesh.core.message.generic.DeltaSetMessage;
import com.telink.ble.mesh.core.message.generic.OnOffGetMessage;
import com.telink.ble.mesh.core.message.lighting.CtlGetMessage;
import com.telink.ble.mesh.core.message.lighting.CtlTemperatureSetMessage;
import com.telink.ble.mesh.core.message.lighting.HslGetMessage;
import com.telink.ble.mesh.core.message.lighting.HslSetMessage;
import com.telink.ble.mesh.core.message.lighting.LightnessGetMessage;
import com.telink.ble.mesh.core.message.lighting.LightnessSetMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.entity.ModelPublication;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.model.UnitConvert;
import com.telink.ble.mesh.ui.adapter.RemotePublishListAdapter;
import com.telink.ble.mesh.ui.adapter.SwitchListAdapter;
import com.telink.ble.mesh.ui.widget.CompositionColorView;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.List;

/**
 * remote control fragment
 */

public class RemoteControlFragment extends BaseFragment {

    NodeInfo deviceInfo;
    RecyclerView rv_rmt_ele;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_remote_control, null);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        final int address = getArguments().getInt("address");
        deviceInfo = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(address);
        MeshLogger.d("device in remote: " + deviceInfo);
        initView(view);
    }

    private void initView(View view) {
        rv_rmt_ele = view.findViewById(R.id.rv_rmt_ele);
        rv_rmt_ele.setLayoutManager(new LinearLayoutManager(getActivity()));
        // no recycle
        rv_rmt_ele.getRecycledViewPool().setMaxRecycledViews(0, 0);

        RemotePublishListAdapter adapter = new RemotePublishListAdapter(this, deviceInfo);
        rv_rmt_ele.setAdapter(adapter);
    }

    public void sendPubMsg(int position) {
        MeshLogger.d("send pub message: " + position);
        RemotePublishListAdapter.ViewHolder holder = (RemotePublishListAdapter.ViewHolder) rv_rmt_ele.findViewHolderForLayoutPosition(position);
        if (holder == null) {
            MeshLogger.d("send pub message - ---- null: " + position);
            return;
        }
        int eleAdr = deviceInfo.meshAddress + position;
        int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();

        String pubAdrInput = holder.et_pub_adr.getText().toString().trim();
        if (TextUtils.isEmpty(pubAdrInput)) {
            Toast.makeText(getActivity(), "pub address empty", Toast.LENGTH_SHORT).show();
            return;
        }
        int pubAdr = Integer.valueOf(pubAdrInput, 16);

        String mdlIdInput = holder.et_mdl_id.getText().toString().trim();
        if (TextUtils.isEmpty(mdlIdInput)) {
            Toast.makeText(getActivity(), "model id empty", Toast.LENGTH_SHORT).show();
            return;
        }
        int modelId = Integer.valueOf(mdlIdInput, 16);
        if (!checkModelId(modelId, position)) {
            Toast.makeText(getActivity(), "model id not exists", Toast.LENGTH_SHORT).show();
            return;
        }

        ModelPublication modelPublication = ModelPublication.createDefault(eleAdr, pubAdr, appKeyIndex, 0, modelId, true);
        ModelPublicationSetMessage publicationSetMessage = new ModelPublicationSetMessage(deviceInfo.meshAddress, modelPublication);
        MeshService.getInstance().sendMeshMessage(publicationSetMessage);
    }

    private boolean checkModelId(int modelId, int position) {
        CompositionData.Element element = deviceInfo.compositionData.elements.get(position);
        for (int mid : element.sigModels) {
            if (mid == modelId) return true;
        }

        for (int mid : element.vendorModels) {
            if (mid == modelId) return true;
        }
        return false;
    }


    @Override
    public void onDestroy() {
        super.onDestroy();
    }


}
