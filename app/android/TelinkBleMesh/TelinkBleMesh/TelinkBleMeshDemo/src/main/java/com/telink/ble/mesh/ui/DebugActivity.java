
package com.telink.ble.mesh.ui;

import android.os.Bundle;
import android.view.View;

import com.telink.ble.mesh.demo.R;

/**
 * special debug func
 * Created by kee on 2018/9/18.
 */
public class DebugActivity extends BaseActivity implements View.OnClickListener {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_debug);
        setTitle("Mesh Debug");

        enableBackNav(true);
        findViewById(R.id.btn_iv).setOnClickListener(this);
        findViewById(R.id.btn_sno).setOnClickListener(this);

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_iv:
                // todo mesh interface
//                MeshService.getInstance().ivTest();
                break;

            case R.id.btn_sno:
                // todo mesh interface
//                MeshService.getInstance().snoTest();
                break;

        }
    }
}