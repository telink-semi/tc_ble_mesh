/********************************************************************************************************
 * @file SceneListActivity.java
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
package com.telink.ble.mesh.ui.activity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONException;
import com.google.android.material.textfield.TextInputEditText;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.scene.SceneRegisterStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.model.web.MeshScene;
import com.telink.ble.mesh.ui.adapter.MeshSceneListAdapter;
import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.ResponseInfo;
import com.telink.ble.mesh.web.TelinkHttpClient;
import com.telink.ble.mesh.web.WebUtils;

import java.io.IOException;
import java.util.List;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.Response;

/**
 * Scene List
 * Created by kee on 2018/9/18.
 */
public class SceneListActivity extends BaseActivity implements EventListener<String>, View.OnClickListener {

    public final static int REQUEST_CODE_SCENE_SETTING = 1;
    // get scene list
    public final static String URL_GET_SCENE_LIST = TelinkHttpClient.URL_BASE + "mesh-scene/getSceneList";

    public final static String URL_CREATE_SCENE = TelinkHttpClient.URL_BASE + "mesh-scene/createScene";

    private MeshSceneListAdapter adapter;
    private Handler handler = new Handler();
    private TextInputEditText et_input;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_scene_list);
        findViewById(R.id.fab_scene).setOnClickListener(this);
        setTitle("Scene List");

        enableBackNav(true);
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.menu_single);

        toolbar.setOnMenuItemClickListener(item -> {
            if (item.getItemId() == R.id.item_single) {
                getSceneList();
            }
            return false;
        });

        adapter = new MeshSceneListAdapter(this);
        RecyclerView rv_scheduler = findViewById(R.id.rv_scene);
        rv_scheduler.setLayoutManager(new LinearLayoutManager(this));
        rv_scheduler.setAdapter(adapter);
        ll_empty = findViewById(R.id.ll_empty);
        getSceneList();
    }

    @Override
    protected void onStart() {
        super.onStart();
        TelinkMeshApplication.getInstance().addEventListener(SceneRegisterStatusMessage.class.getName(), this);
    }

    @Override
    protected void onStop() {
        super.onStop();
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    private void onAddClick() {
        showCreateDialog();
    }

    private void showCreateDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("input scene name ")
                .setView(R.layout.dialog_single_input)
//                .setMessage("Input Network Name")
                .setPositiveButton("Create", (dialog, which) -> createScene(et_input.getText().toString()))
                .setNegativeButton("Cancel", (dialog, which) -> dialog.cancel());
        AlertDialog dialog = builder.show();
        et_input = dialog.findViewById(R.id.et_input);
    }


    private void createScene(String name) {
        if (TextUtils.isEmpty(name)) {
            toastMsg("pls input scene name");
            return;
        }
        int networkId = TelinkMeshApplication.getInstance().getMeshInfo().id;
        FormBody formBody = new FormBody.Builder()
                .add("name", name)
                .add("networkId", networkId + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_CREATE_SCENE, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.d("create scene call onFailure");
                dismissIndeterminateLoading();
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, SceneListActivity.this);
                if (responseInfo == null) {
                    return;
                }
                MeshLogger.d("create scene call response");
                try {

                    MeshScene network = JSON.parseObject(responseInfo.data, MeshScene.class);
                    if (network == null) {
                        showError("scene null in response");
                        return;
                    }
                    runOnUiThread(() -> {
                        adapter.add(network);
                        updateEmptyVisibility(false);
                    });
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    public void getSceneList() {
        showIndeterminateLoading();
        int networkId = TelinkMeshApplication.getInstance().getMeshInfo().id;
        FormBody formBody = new FormBody.Builder()
                .add("networkId", networkId + "").build();
        TelinkHttpClient.getInstance().post(URL_GET_SCENE_LIST, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                dismissIndeterminateLoading();
                MeshLogger.e("call fail");
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, SceneListActivity.this);
                if (responseInfo == null) return;
                if (!responseInfo.isSuccess()) {
                    showError(responseInfo.message);
                    return;
                }

                try {
                    List<MeshScene> list = JSON.parseArray(responseInfo.data, MeshScene.class);
                    runOnUiThread(() -> {
                        adapter.resetData(list);
                        updateEmptyVisibility(adapter.getItemCount() == 0);
                    });
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        });
    }


    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.fab_scene) {
            onAddClick();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        MeshLogger.d("SceneListActivity - onActivityResult : " + requestCode + " -- " + resultCode);
        if (requestCode == REQUEST_CODE_SCENE_SETTING && resultCode == RESULT_OK) {
            getSceneList();
        }
    }
}
