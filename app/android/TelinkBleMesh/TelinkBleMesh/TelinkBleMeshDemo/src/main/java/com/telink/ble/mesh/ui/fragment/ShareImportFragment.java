package com.telink.ble.mesh.ui.fragment;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.json.MeshStorageService;
import com.telink.ble.mesh.ui.JsonPreviewActivity;
import com.telink.ble.mesh.ui.file.FileSelectActivity;
import com.telink.ble.mesh.util.FileSystem;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.File;

/**
 * share import fragment
 */

public class ShareImportFragment extends BaseFragment implements View.OnClickListener {
    private TextView tv_file_select;
    private TextView tv_log;
    private Button btn_open;
    private static final int REQUEST_CODE_GET_FILE = 1;
    private String mPath;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_share_import, null);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        tv_file_select = view.findViewById(R.id.tv_file_select);
        tv_file_select.setOnClickListener(this);
        btn_open = view.findViewById(R.id.btn_open);
        btn_open.setOnClickListener(this);
        tv_log = view.findViewById(R.id.tv_log);
        view.findViewById(R.id.btn_import).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tv_file_select:
                startActivityForResult(new Intent(getActivity(), FileSelectActivity.class).putExtra(FileSelectActivity.KEY_SUFFIX, ".json"), REQUEST_CODE_GET_FILE);
                break;

            case R.id.btn_open:
                if (mPath == null) {
                    return;
                }
                startActivity(new Intent(this.getActivity(), JsonPreviewActivity.class).putExtra(JsonPreviewActivity.FILE_PATH, mPath));
                break;

            case R.id.btn_import:
                if (mPath == null) {
                    toastMsg("Pls select target file");
                    return;
                }
                File file = new File(mPath);
                if (!file.exists()) {
                    toastMsg("file not exist");
                    return;
                }
                String jsonData = FileSystem.readString(file);
                MeshInfo newMesh = MeshStorageService.getInstance().importExternal(jsonData, TelinkMeshApplication.getInstance().getMeshInfo());
                newMesh.saveOrUpdate(getActivity());
                MeshService.getInstance().idle(true);
                MeshService.getInstance().setupMeshNetwork(newMesh.convertToConfiguration());
                TelinkMeshApplication.getInstance().setupMesh(newMesh);
                tv_log.append("Mesh storage import success, back to home page to reconnect\n");
                toastMsg("import success");
                break;
        }
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (resultCode != Activity.RESULT_OK || requestCode != REQUEST_CODE_GET_FILE)
            return;

        mPath = data.getStringExtra(FileSelectActivity.KEY_RESULT);
        btn_open.setVisibility(View.VISIBLE);
        tv_file_select.setText(mPath);

        tv_log.append("File selected: " + mPath + "\n");
        MeshLogger.log("select: " + mPath);
    }

}
