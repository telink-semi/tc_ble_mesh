package com.telink.ble.mesh.ui;

import android.content.Intent;
import android.os.Bundle;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.util.FileSystem;

import java.io.File;

/**
 * json preview
 * Created by kee on 2018/9/18.
 */
public class JsonPreviewActivity extends BaseActivity {
    public static final String FILE_PATH = "com.telink.ble.mesh.FILE_PATH";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_json_preview);
        setTitle("JsonPreview");
        enableBackNav(true);

        TextView tv_json = findViewById(R.id.tv_json);
        final Intent intent = getIntent();
        String info = FileSystem.readString(new File(intent.getStringExtra(FILE_PATH)));
        tv_json.setText(info);
    }
}
