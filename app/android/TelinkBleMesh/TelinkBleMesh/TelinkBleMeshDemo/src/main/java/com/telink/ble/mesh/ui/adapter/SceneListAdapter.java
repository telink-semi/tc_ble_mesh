/********************************************************************************************************
 * @file     SceneListAdapter.java 
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
package com.telink.ble.mesh.ui.adapter;

import android.content.Context;
import android.content.Intent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.model.Scene;
import com.telink.ble.mesh.ui.IconGenerator;
import com.telink.ble.mesh.ui.SceneSettingActivity;
import com.telink.ble.mesh.core.message.scene.SceneRecallMessage;
import com.telink.ble.mesh.foundation.MeshService;

import java.util.List;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * Scene List
 */
public class SceneListAdapter extends BaseRecyclerViewAdapter<SceneListAdapter.ViewHolder> {
    List<Scene> sceneList;
    Context mContext;

    public SceneListAdapter(Context context, List<Scene> scenes) {
        mContext = context;
        sceneList = scenes;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_scene, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_scene_info = itemView.findViewById(R.id.tv_scene_info);
        holder.iv_recall = itemView.findViewById(R.id.iv_recall);

        holder.iv_edit = itemView.findViewById(R.id.iv_edit);
        holder.rv_inner_device = itemView.findViewById(R.id.rv_inner_device);

        return holder;
    }

    private View.OnClickListener imageClick = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            int position = (int) v.getTag();
            if (v.getId() == R.id.iv_edit) {
                mContext.startActivity(new Intent(mContext, SceneSettingActivity.class).putExtra("sceneId", sceneList.get(position).id));
            } else if (v.getId() == R.id.iv_recall) {
//                MeshService.getInstance().cmdSceneRecall(0xFFFF, 0, sceneList.get(position).id, 0, null);

                int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
                SceneRecallMessage recallMessage = SceneRecallMessage.getSimple(0xFFFF,
                        appKeyIndex, sceneList.get(position).id, false, 0);
                MeshService.getInstance().sendMeshMessage(recallMessage);
                // mesh interface
//                MeshService.getInstance().recallScene(0xFFFF, false, 0, sceneList.get(position).id, 0, (byte) 0, null);
            }
        }
    };


    @Override
    public int getItemCount() {
        return sceneList == null ? 0 : sceneList.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);

        Scene scene = sceneList.get(position);
        holder.tv_scene_info.setText("Scene id: 0x" + Long.toHexString(scene.id));

        holder.iv_recall.setOnClickListener(this.imageClick);
        holder.iv_recall.setTag(position);

        holder.iv_edit.setOnClickListener(this.imageClick);
        holder.iv_edit.setTag(position);

        holder.rv_inner_device.setLayoutManager(new LinearLayoutManager(mContext));
        holder.rv_inner_device.setAdapter(new SimpleDeviceAdapter(scene.states));
    }

    class ViewHolder extends RecyclerView.ViewHolder {


        TextView tv_scene_info;
        ImageView iv_recall, iv_edit;
        RecyclerView rv_inner_device;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }


    class SimpleDeviceViewHolder extends RecyclerView.ViewHolder {

        ImageView iv_device;
        TextView tv_device_info;

        public SimpleDeviceViewHolder(View itemView) {
            super(itemView);
        }
    }

    class SimpleDeviceAdapter extends BaseRecyclerViewAdapter<SimpleDeviceViewHolder> {

        List<Scene.SceneState> innerDevices;

        SimpleDeviceAdapter(List<Scene.SceneState> devices) {
            this.innerDevices = devices;
        }

        @Override
        public SimpleDeviceViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
            View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_simple_device, parent, false);
            SimpleDeviceViewHolder holder = new SimpleDeviceViewHolder(itemView);
            holder.iv_device = itemView.findViewById(R.id.iv_device);
            holder.tv_device_info = itemView.findViewById(R.id.tv_device_info);
            return holder;
        }

        @Override
        public void onBindViewHolder(SimpleDeviceViewHolder holder, int position) {
            super.onBindViewHolder(holder, position);

            Scene.SceneState state = innerDevices.get(position);
            holder.iv_device.setImageResource(IconGenerator.generateDeviceIconRes(state.onOff));
            holder.tv_device_info.setText(mContext.getString(R.string.scene_state_desc,
                    state.address,
                    getOnOffDesc(state.onOff)));
        }

        @Override
        public int getItemCount() {
            return this.innerDevices == null ? 0 : this.innerDevices.size();
        }
    }

    public String getOnOffDesc(int onOff) {
        if (onOff == 1) {
            return "ON";
        } else if (onOff == 0) {
            return "OFF";
        } else if (onOff == -1) {
            return "OFFLINE";
        }
        return "UNKNOWN";
    }
}
