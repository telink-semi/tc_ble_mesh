/********************************************************************************************************
 * @file MeshSceneListAdapter.java
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
package com.telink.ble.mesh.ui.adapter;

import android.content.Context;
import android.content.Intent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.scene.SceneRecallMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.model.web.MeshScene;
import com.telink.ble.mesh.ui.activity.SceneListActivity;
import com.telink.ble.mesh.ui.activity.SceneSettingActivity;

import java.util.ArrayList;
import java.util.List;

/**
 * Scene List - web
 */
public class MeshSceneListAdapter extends BaseRecyclerViewAdapter<MeshSceneListAdapter.ViewHolder> {
    private List<MeshScene> sceneList = null;
    private Context mContext;

    public MeshSceneListAdapter(Context context) {
        mContext = context;
    }

    public void resetData(List<MeshScene> sceneList) {
        this.sceneList = sceneList;
        this.notifyDataSetChanged();
    }

    public void add(MeshScene scene) {
        if (this.sceneList == null) {
            this.sceneList = new ArrayList<>();
        }
        this.sceneList.add(scene);
        this.notifyDataSetChanged();
    }

    public List<MeshScene> getSceneList() {
        return sceneList;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_mesh_scene, parent, false);
        ViewHolder holder = new ViewHolder(itemView);

        holder.tv_name = itemView.findViewById(R.id.tv_name);
        holder.tv_info = itemView.findViewById(R.id.tv_info);
        holder.tv_scene_id = itemView.findViewById(R.id.tv_scene_id);
        holder.tv_time = itemView.findViewById(R.id.tv_time);

        holder.iv_recall = itemView.findViewById(R.id.iv_recall);
        holder.iv_edit = itemView.findViewById(R.id.iv_edit);
        return holder;
    }

    private View.OnClickListener imageClick = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            int position = (int) v.getTag();
            if (v.getId() == R.id.iv_edit) {
                ((SceneListActivity) mContext).startActivityForResult(
                        new Intent(mContext, SceneSettingActivity.class)
                                .putExtra("scene", sceneList.get(position)), SceneListActivity.REQUEST_CODE_SCENE_SETTING);
            } else if (v.getId() == R.id.iv_recall) {
                int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
                SceneRecallMessage recallMessage = SceneRecallMessage.getSimple(0xFFFF,
                        appKeyIndex, sceneList.get(position).getSceneId(), false, 0);
                MeshService.getInstance().sendMeshMessage(recallMessage);
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

        MeshScene scene = sceneList.get(position);
        holder.tv_name.setText(scene.getName());
        holder.tv_info.setText(scene.getInfo());
        holder.tv_scene_id.setText(String.format("0x%04X", scene.getSceneId()));
        holder.tv_time.setText(MeshUtils.getTimeFormat(scene.getCreateTime()));

        holder.iv_recall.setOnClickListener(this.imageClick);
        holder.iv_recall.setTag(position);

        holder.iv_edit.setOnClickListener(this.imageClick);
        holder.iv_edit.setTag(position);
    }

    class ViewHolder extends RecyclerView.ViewHolder {

        TextView tv_name, tv_info, tv_scene_id, tv_time;
        ImageView iv_recall, iv_edit;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }

}
