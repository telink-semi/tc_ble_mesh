package com.telink.ble.mesh.ui;

import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.ui.widget.ColorPanel;
import com.telink.ble.mesh.core.message.lighting.HslSetMessage;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.util.TelinkLog;

import androidx.core.graphics.ColorUtils;

/**
 * Created by kee on 2018/8/28.
 */

public class ColorPanelActivity extends BaseActivity {
    ColorPanel color_panel;
    View color_presenter;
    int address;
    TextView tv_hsl;
    SeekBar sb_lightness;
    SeekBar sb_red, sb_green, sb_blue;
    TextView tv_red, tv_green, tv_blue;
    private long preTime;
    private static final int DELAY_TIME = 320;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_color_panel);
        enableBackNav(true);
        setTitle("HSL");
        color_panel = findViewById(R.id.color_panel);
        color_panel.setColorChangeListener(colorChangeListener);
        color_panel.setColor(Color.WHITE);

        color_presenter = findViewById(R.id.color_presenter);
        address = getIntent().getIntExtra("address", 0);

        tv_hsl = findViewById(R.id.tv_hsl);
        sb_lightness = findViewById(R.id.sb_lightness);

        tv_red = findViewById(R.id.tv_red);
        tv_green = findViewById(R.id.tv_green);
        tv_blue = findViewById(R.id.tv_blue);


        sb_red = findViewById(R.id.sb_red);
        sb_green = findViewById(R.id.sb_green);
        sb_blue = findViewById(R.id.sb_blue);

        sb_red.setOnSeekBarChangeListener(onSeekBarChangeListener);
        sb_green.setOnSeekBarChangeListener(onSeekBarChangeListener);
        sb_blue.setOnSeekBarChangeListener(onSeekBarChangeListener);


        sb_lightness.setOnSeekBarChangeListener(onSeekBarChangeListener);
//        refreshDesc(color_panel.getColor());
    }

    private ColorPanel.ColorChangeListener colorChangeListener = new ColorPanel.ColorChangeListener() {

        private long preTime;
        private static final int DELAY_TIME = 320;


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
                TelinkLog.w("CMD reject : color set");
            }
        }
    };

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

            if (seekBar == sb_lightness) {
                float visibility = ((float) progress) / 100;
                if (color_panel != null) {
                    color_panel.setVisibility(visibility, true);
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
                sb_lightness.setProgress((int) (hsv[2] * 100));

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


    private void sendHslSetMessage(float[] hslValue) {
        int hue = (int) (hslValue[0] * 65535 / 360);
        int sat = (int) (hslValue[1] * 65535);
        int lightness = (int) (hslValue[2] * 65535);

        MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        HslSetMessage hslSetMessage = HslSetMessage.getSimple(address, meshInfo.getDefaultAppKeyIndex(),
                lightness,
                hue,
                sat,
                false, 0);
        MeshService.getInstance().sendMeshMessage(hslSetMessage);
    }

    private void refreshDesc(float[] hslValue, int color) {

        color_presenter.setBackgroundColor(color);

        int red = (color >> 16) & 0xFF;
        int green = (color >> 8) & 0xFF;
        int blue = color & 0xFF;
        String rgb = "RGB: \n\tR -- " + red +
                "\n\tG -- " + green +
                "\n\tB -- " + blue;

        sb_red.setProgress(red);
        sb_green.setProgress(green);
        sb_blue.setProgress(blue);

        tv_red.setText(String.format("%03d", red));
        tv_green.setText(String.format("%03d", green));
        tv_blue.setText(String.format("%03d", blue));

        // Hue Saturation Hue
        // float[] hslValue = new float[3];
        String hsl = "HSL: \n\tH -- " + (hslValue[0]) + "(" + (byte) (hslValue[0] * 100 / 360) + ")" +
                "\n\tS -- " + (hslValue[1]) + "(" + (byte) (hslValue[1] * 100) + ")" +
                "\n\tL -- " + (hslValue[2] + "(" + (byte) (hslValue[2] * 100) + ")"
        );
        tv_hsl.setText(hsl);
    }
}
