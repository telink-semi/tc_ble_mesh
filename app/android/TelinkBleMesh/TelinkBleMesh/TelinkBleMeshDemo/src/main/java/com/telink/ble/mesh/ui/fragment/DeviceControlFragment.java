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
import android.util.SparseBooleanArray;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.SeekBar;
import android.widget.TextView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.generic.DeltaSetMessage;
import com.telink.ble.mesh.core.message.generic.OnOffGetMessage;
import com.telink.ble.mesh.core.message.lighting.CtlGetMessage;
import com.telink.ble.mesh.core.message.lighting.CtlTemperatureSetMessage;
import com.telink.ble.mesh.core.message.lighting.HslGetMessage;
import com.telink.ble.mesh.core.message.lighting.HslSetMessage;
import com.telink.ble.mesh.core.message.lighting.LightnessGetMessage;
import com.telink.ble.mesh.core.message.lighting.LightnessSetMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.model.UnitConvert;
import com.telink.ble.mesh.ui.adapter.SwitchListAdapter;
import com.telink.ble.mesh.ui.widget.CompositionColorView;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.List;

import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * device control fragment
 * Created by kee on 2017/8/18.
 */

public class DeviceControlFragment extends BaseFragment implements EventListener<String>, View.OnClickListener {
    private CompositionColorView cps_color;
    private View ll_lum, ll_lum_level, ll_temp, ll_temp_level;
    NodeInfo deviceInfo;
    TextView tv_lum, tv_temp, tv_lum_level, tv_temp_level;
    SeekBar sb_lum, sb_temp;
    private long preTime;
    private static final int DELAY_TIME = 320;

    private SparseBooleanArray lumEleInfo;
    private SparseBooleanArray tempEleInfo;
    private int hslEleAdr;
    private List<Integer> onOffEleAdrList;
    SwitchListAdapter switchListAdapter;
    int delta = 0;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_device_control, null);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        double max = 0xFFFF;
        double stepCnt = 10;

        delta = (int) Math.ceil(max / stepCnt);
        final int address = getArguments().getInt("address");
        deviceInfo = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(address);
        lumEleInfo = deviceInfo.getLumEleInfo();
        tempEleInfo = deviceInfo.getTempEleInfo();
        hslEleAdr = deviceInfo.getTargetEleAdr(MeshSigModel.SIG_MD_LIGHT_HSL_S.modelId);
        onOffEleAdrList = deviceInfo.getOnOffEleAdrList();

        initView(view);
        setVisibility();

        TelinkMeshApplication.getInstance().addEventListener(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);

        getNodeStatus();
    }

    private void initView(View view) {
        RecyclerView rv_switch = view.findViewById(R.id.rv_switch);
        rv_switch.setLayoutManager(new GridLayoutManager(getActivity(), 4));
        switchListAdapter = new SwitchListAdapter(getActivity(), onOffEleAdrList);
        rv_switch.setAdapter(switchListAdapter);
        cps_color = view.findViewById(R.id.cps_color);
        cps_color.setMessageDelegate(new CompositionColorView.ColorMessageDelegate() {
            @Override
            public void onHSLMessage(float[] hsl) {
                sendHslSetMessage(hsl);
            }
        });
        ll_lum = view.findViewById(R.id.ll_lum);
        ll_lum_level = view.findViewById(R.id.ll_lum_level);
        ll_temp = view.findViewById(R.id.ll_temp);
        ll_temp_level = view.findViewById(R.id.ll_temp_level);

        tv_lum = view.findViewById(R.id.tv_lum);
        tv_temp = view.findViewById(R.id.tv_temp);
        tv_lum_level = view.findViewById(R.id.tv_lum_level);
        tv_temp_level = view.findViewById(R.id.tv_temp_level);


        sb_lum = view.findViewById(R.id.sb_brightness);
        sb_temp = view.findViewById(R.id.sb_temp);

        sb_lum.setOnSeekBarChangeListener(onSeekBarChangeListener);
        sb_temp.setOnSeekBarChangeListener(onSeekBarChangeListener);

        view.findViewById(R.id.iv_lum_add).setOnClickListener(this);
        view.findViewById(R.id.iv_lum_minus).setOnClickListener(this);
        view.findViewById(R.id.iv_temp_add).setOnClickListener(this);
        view.findViewById(R.id.iv_temp_minus).setOnClickListener(this);
    }

    private void getNodeStatus() {
        if (deviceInfo.compositionData.lowPowerSupport()) {
            //skip lpn
            return;
        }
        int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
        int modelId = MeshSigModel.SIG_MD_LIGHT_CTL_S.modelId;
        int modelEleAdr = deviceInfo.getTargetEleAdr(modelId);
        String desc = null;

        if (modelEleAdr != -1) {
            MeshService.getInstance().sendMeshMessage(CtlGetMessage.getSimple(modelEleAdr, appKeyIndex, 0));
            return;
        }

        modelId = MeshSigModel.SIG_MD_LIGHT_HSL_S.modelId;
        modelEleAdr = deviceInfo.getTargetEleAdr(modelId);
        if (modelEleAdr != -1) {
            MeshService.getInstance().sendMeshMessage(HslGetMessage.getSimple(modelEleAdr, appKeyIndex, 0));
            return;
        }

        modelId = MeshSigModel.SIG_MD_LIGHTNESS_S.modelId;
        modelEleAdr = deviceInfo.getTargetEleAdr(modelId);
        if (modelEleAdr != -1) {
            MeshService.getInstance().sendMeshMessage(LightnessGetMessage.getSimple(modelEleAdr, appKeyIndex, 0));
            return;
        }

        modelId = MeshSigModel.SIG_MD_G_ONOFF_S.modelId;
        modelEleAdr = deviceInfo.getTargetEleAdr(modelId);
        if (modelEleAdr != -1) {
            MeshService.getInstance().sendMeshMessage(OnOffGetMessage.getSimple(modelEleAdr, appKeyIndex, 0));
        }
    }


    private void setVisibility() {
        if (hslEleAdr == -1) {
            cps_color.setVisibility(View.GONE);
        } else {
//            ll_lum.setVisibility(View.GONE);
//            ll_lum_level.setVisibility(View.GONE);
//            ll_temp.setVisibility(View.GONE);
//            ll_temp_level.setVisibility(View.GONE);
        }

        if (lumEleInfo == null) {
            ll_lum.setVisibility(View.GONE);
        } else {
            tv_lum.setText(getString(R.string.lum_progress, deviceInfo.lum, Integer.toHexString(lumEleInfo.keyAt(0))));
            sb_lum.setProgress(deviceInfo.lum);
            if (!lumEleInfo.get(lumEleInfo.keyAt(0))) {
                ll_lum_level.setVisibility(View.GONE);
            } else {
                tv_lum_level.setText(getString(R.string.lum_level, Integer.toHexString(lumEleInfo.keyAt(0))));
            }
        }

        if (tempEleInfo == null) {
            ll_temp.setVisibility(View.GONE);
        } else {
            tv_temp.setText(getString(R.string.temp_progress, deviceInfo.temp, Integer.toHexString(tempEleInfo.keyAt(0))));
            sb_temp.setProgress(deviceInfo.temp);
            if (!tempEleInfo.get(tempEleInfo.keyAt(0))) {
                ll_temp_level.setVisibility(View.GONE);
            } else {
                tv_temp_level.setText(getString(R.string.temp_level, Integer.toHexString(tempEleInfo.keyAt(0))));
            }
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    private void sendHslSetMessage(float[] hslValue) {
        int hue = Math.round(hslValue[0] * 65535 / 360);
        int sat = Math.round(hslValue[1] * 65535);
        int lightness = Math.round(hslValue[2] * 65535);
//        lightness = 0x54d5;
        MeshLogger.d("set hsl: hue -> " + hue + " sat -> " + sat + " lightness -> " + lightness);
        MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        HslSetMessage hslSetMessage = HslSetMessage.getSimple(hslEleAdr, meshInfo.getDefaultAppKeyIndex(),
                lightness,
                hue,
                sat,
                false, 0);
        MeshService.getInstance().sendMeshMessage(hslSetMessage);
    }


    private SeekBar.OnSeekBarChangeListener onSeekBarChangeListener = new SeekBar.OnSeekBarChangeListener() {

        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            if (seekBar == sb_lum) {
                if (cps_color.getVisibility() == View.VISIBLE) {
                    cps_color.updateLightness(progress);
                }
            }
            if (fromUser)
                onProgressUpdate(seekBar, progress, false);
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {

        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            onProgressUpdate(seekBar, seekBar.getProgress(), true);
        }

        void onProgressUpdate(SeekBar seekBar, int progress, boolean immediate) {

            if (seekBar == sb_lum || seekBar == sb_temp) {

                long currentTime = System.currentTimeMillis();
                if (seekBar == sb_lum) {
                    deviceInfo.lum = progress;
                    tv_lum.setText(getString(R.string.lum_progress, progress, Integer.toHexString(lumEleInfo.keyAt(0))));
                    progress = Math.max(1, progress);
                    if ((currentTime - preTime) >= DELAY_TIME || immediate) {
                        preTime = currentTime;
                        MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
                        LightnessSetMessage message = LightnessSetMessage.getSimple(lumEleInfo.keyAt(0),
                                meshInfo.getDefaultAppKeyIndex(),
                                UnitConvert.lum2lightness(progress),
                                false, 0);
                        MeshService.getInstance().sendMeshMessage(message);
                    }
                } else if (seekBar == sb_temp) {
                    deviceInfo.temp = progress;
                    tv_temp.setText(getString(R.string.temp_progress, progress, Integer.toHexString(tempEleInfo.keyAt(0))));
                    if ((currentTime - preTime) >= DELAY_TIME || immediate) {
                        preTime = currentTime;
                        MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
                        CtlTemperatureSetMessage temperatureSetMessage =
                                CtlTemperatureSetMessage.getSimple(tempEleInfo.keyAt(0),
                                        meshInfo.getDefaultAppKeyIndex(), UnitConvert.temp100ToTemp(progress),
                                        0, false, 0);
                        MeshService.getInstance().sendMeshMessage(temperatureSetMessage);
                    }
                }
            }
        }
    };


    @Override
    public void performed(Event<String> event) {


        if (event.getType().equals(MeshEvent.EVENT_TYPE_DISCONNECTED)
                || event.getType().equals(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED)) {
            getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    setVisibility();
                }
            });
        }
    }

    @Override
    public void onClick(View v) {
        MeshLogger.log("delta: " + delta);
        MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        int appKeyIndex = meshInfo.getDefaultAppKeyIndex();
        switch (v.getId()) {
            case R.id.iv_lum_add:
                DeltaSetMessage deltaSetMessage = DeltaSetMessage.getSimple(lumEleInfo.keyAt(0),
                        appKeyIndex, delta, true, 1);
                MeshService.getInstance().sendMeshMessage(deltaSetMessage);
                break;

            case R.id.iv_lum_minus:
                deltaSetMessage = DeltaSetMessage.getSimple(lumEleInfo.keyAt(0),
                        appKeyIndex, -delta, true, 1);
                MeshService.getInstance().sendMeshMessage(deltaSetMessage);
                break;

            case R.id.iv_temp_add:

                deltaSetMessage = DeltaSetMessage.getSimple(tempEleInfo.keyAt(0),
                        appKeyIndex, delta, true, 1);
                MeshService.getInstance().sendMeshMessage(deltaSetMessage);
                break;

            case R.id.iv_temp_minus:


                deltaSetMessage = DeltaSetMessage.getSimple(tempEleInfo.keyAt(0),
                        appKeyIndex, -delta, true, 1);
                MeshService.getInstance().sendMeshMessage(deltaSetMessage);
                break;

        }
    }

}
