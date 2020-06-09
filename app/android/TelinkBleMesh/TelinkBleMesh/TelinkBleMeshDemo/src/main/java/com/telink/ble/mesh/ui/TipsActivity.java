package com.telink.ble.mesh.ui;

import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;

/**
 * tips
 */
public class TipsActivity extends BaseActivity {
    public static final String INTENT_KEY_TIP_RES_ID = "TipsActivity.TIP_RES_ID";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_share_tip);
        setTitle("Tip");
        int tipResId = getIntent().getIntExtra(INTENT_KEY_TIP_RES_ID, R.string.share_tip);
        TextView tv_tip = findViewById(R.id.tv_tip);
        tv_tip.setText(tipResId);
        enableBackNav(true);
    }

    public void onKnownClick(View view) {
        finish();
    }

}
