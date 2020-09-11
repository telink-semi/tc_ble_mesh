/********************************************************************************************************
 * @file     DeviceControlFragment.java 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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

import android.graphics.Color;
import android.os.Bundle;
import android.util.SparseBooleanArray;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ScrollView;
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
import com.telink.ble.mesh.ui.widget.ColorPanel;
import com.telink.ble.mesh.util.MeshLogger;


import java.util.List;

import androidx.core.graphics.ColorUtils;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * device control fragment
 * Created by kee on 2017/8/18.
 */

public class DeviceControlFragment extends BaseFragment implements EventListener<String>, View.OnClickListener {
    ColorPanel color_panel;
    View color_presenter;
    ScrollView sv_container;
    private View ll_hsl, ll_lum, ll_lum_level, ll_temp, ll_temp_level;
    NodeInfo deviceInfo;
    TextView tv_hsl, tv_lum, tv_temp, tv_lum_level, tv_temp_level;
    SeekBar sb_color, sb_lum, sb_temp;
    SeekBar sb_red, sb_green, sb_blue;
    TextView tv_red, tv_green, tv_blue;
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
        sv_container = view.findViewById(R.id.sv_container);
        color_panel = view.findViewById(R.id.color_panel);
        color_panel.setOnTouchListener(this.colorPanelTouchListener);
        color_panel.setColorChangeListener(colorChangeListener);
        color_panel.setColor(Color.WHITE);

        color_presenter = view.findViewById(R.id.color_presenter);

        ll_hsl = view.findViewById(R.id.ll_hsl);
        ll_lum = view.findViewById(R.id.ll_lum);
        ll_lum_level = view.findViewById(R.id.ll_lum_level);
        ll_temp = view.findViewById(R.id.ll_temp);
        ll_temp_level = view.findViewById(R.id.ll_temp_level);

        tv_hsl = view.findViewById(R.id.tv_hsl);
        tv_lum = view.findViewById(R.id.tv_lum);
        tv_temp = view.findViewById(R.id.tv_temp);
        tv_lum_level = view.findViewById(R.id.tv_lum_level);
        tv_temp_level = view.findViewById(R.id.tv_temp_level);

        tv_red = view.findViewById(R.id.tv_red);
        tv_green = view.findViewById(R.id.tv_green);
        tv_blue = view.findViewById(R.id.tv_blue);

        sb_color = view.findViewById(R.id.sb_color);
        sb_lum = view.findViewById(R.id.sb_brightness);
        sb_temp = view.findViewById(R.id.sb_temp);

        sb_red = view.findViewById(R.id.sb_red);
        sb_green = view.findViewById(R.id.sb_green);
        sb_blue = view.findViewById(R.id.sb_blue);

        sb_lum.setOnSeekBarChangeListener(onSeekBarChangeListener);
        sb_temp.setOnSeekBarChangeListener(onSeekBarChangeListener);
        sb_color.setOnSeekBarChangeListener(onSeekBarChangeListener);

        sb_red.setOnSeekBarChangeListener(onSeekBarChangeListener);
        sb_green.setOnSeekBarChangeListener(onSeekBarChangeListener);
        sb_blue.setOnSeekBarChangeListener(onSeekBarChangeListener);

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
            ll_hsl.setVisibility(View.GONE);
        } else {
//            ll_lum.setVisibility(View.GONE);
//            ll_lum_level.setVisibility(View.GONE);
//            ll_temp.setVisibility(View.GONE);
//            ll_temp_level.setVisibility(View.GONE);
        }

        if (lumEleInfo == null) {
            ll_lum.setVisibility(View.GONE);
        } else {
            tv_lum.setText(getString(R.string.lum_progress, Math.max(1, deviceInfo.lum), Integer.toHexString(lumEleInfo.keyAt(0))));
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

    private ColorPanel.ColorChangeListener colorChangeListener = new ColorPanel.ColorChangeListener() {


        @Override
        public void onColorChanged(float[] hsv, boolean touchStopped) {
            int color = Color.HSVToColor(hsv);
            float[] hslValue = new float[3];
            ColorUtils.colorToHSL(color, hslValue);

            refreshDesc(hslValue, color);
            long currentTime = System.currentTimeMillis();
            if ((currentTime - preTime) >= DELAY_TIME || touchStopped) {
                preTime = currentTime;
                sendHslSetMessage(hslValue);
            } else {
                MeshLogger.w("CMD reject : color set");
            }
        }
    };

    private void sendHslSetMessage(float[] hslValue) {
        int hue = Math.round(hslValue[0] * 65535 / 360);
        int sat = Math.round(hslValue[1] * 65535);
        int lightness = Math.round(hslValue[2] * 65535);
//        lightness = 0x54d5;
        MeshLogger.d("set hsl: hue -> " + hue + " sat -> "+ sat + " lightness -> "+ lightness);
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

            if (seekBar == sb_color) {

                float visibility = ((float) progress) / 100;
                if (color_panel != null) {
                    color_panel.setVisibility(visibility, immediate);
                }
            } else if (seekBar == sb_lum || seekBar == sb_temp) {

                long currentTime = System.currentTimeMillis();
                if (seekBar == sb_lum) {
                    deviceInfo.lum = progress;
                    progress = Math.max(1, progress);
                    tv_lum.setText(getString(R.string.lum_progress, progress, Integer.toHexString(lumEleInfo.keyAt(0))));
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
            } else if (seekBar == sb_red || seekBar == sb_green || seekBar == sb_blue) {
                long currentTime = System.currentTimeMillis();

                int red = sb_red.getProgress();
                int green = sb_green.getProgress();
                int blue = sb_blue.getProgress();
                int color = 0xFF000000 | ((red & 0xFF) << 16) | ((green & 0xFF) << 8) | (blue & 0xFF);

                color_panel.setColor(color);
                float[] hsv = new float[3];
                Color.colorToHSV(color, hsv);
                sb_color.setProgress((int) (hsv[2] * 100));

                float[] hslValue = new float[3];
                ColorUtils.colorToHSL(color, hslValue);
                refreshDesc(hslValue, color);

                if ((currentTime - preTime) >= DELAY_TIME || immediate) {
                    preTime = currentTime;
                    sendHslSetMessage(hslValue);
                }
            }
        }
    };

    private View.OnTouchListener colorPanelTouchListener = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            if (event.getAction() == MotionEvent.ACTION_UP) {
                sv_container.requestDisallowInterceptTouchEvent(false);
            } else {
                sv_container.requestDisallowInterceptTouchEvent(true);
            }
            return false;
        }
    };

    private void refreshDesc(float[] hslValue, int color) {

        color_presenter.setBackgroundColor(color);

        int red = (color >> 16) & 0xFF;
        int green = (color >> 8) & 0xFF;
        int blue = color & 0xFF;

        sb_red.setProgress(red);
        sb_green.setProgress(green);
        sb_blue.setProgress(blue);

        tv_red.setText(String.format("%03d", red));
        tv_green.setText(String.format("%03d", green));
        tv_blue.setText(String.format("%03d", blue));

        // Hue Saturation Hue
        String hsl = "HSL: \n\tH -- " + (hslValue[0]) + "(" + (byte) (hslValue[0] * 100 / 360) + ")" +
                "\n\tS -- " + (hslValue[1]) + "(" + (byte) (hslValue[1] * 100) + ")" +
                "\n\tL -- " + (hslValue[2] + "(" + (byte) (hslValue[2] * 100) + ")"
        );
        tv_hsl.setText(hsl);
    }


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
