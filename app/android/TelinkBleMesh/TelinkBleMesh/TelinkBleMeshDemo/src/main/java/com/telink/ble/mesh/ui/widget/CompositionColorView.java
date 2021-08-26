package com.telink.ble.mesh.ui.widget;

import android.content.Context;
import android.graphics.Color;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.SeekBar;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.util.MeshLogger;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.graphics.ColorUtils;


public class CompositionColorView extends FrameLayout {

    private ColorPanel color_panel;

    private View color_presenter;
    private TextView tv_hsl;

    // hsV
    private SeekBar sb_value;
    private TextView tv_value;

    // RGB
    private SeekBar sb_red, sb_green, sb_blue;

    private TextView tv_red, tv_green, tv_blue;

    private long preTime;
    private static final int DELAY_TIME = 320;

    private SeekBar sb_hue, sb_sat, sb_lit;
    private TextView tv_hue, tv_sat, tv_lit;
    private ColorMessageDelegate messageDelegate;

    public CompositionColorView(@NonNull Context context) {
        super(context);
        initView(context);
    }

    public CompositionColorView(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        initView(context);
    }

    public void setMessageDelegate(ColorMessageDelegate messageDelegate) {
        this.messageDelegate = messageDelegate;
    }

    private void initView(Context context) {
        View container = LayoutInflater.from(context).inflate(R.layout.layout_color_cps, this, true);

        color_panel = findViewById(R.id.color_panel);
        color_panel.setOnTouchListener(this.colorPanelTouchListener);
        color_panel.setColorChangeListener(colorChangeListener);
        color_panel.setColor(Color.WHITE);

        color_presenter = findViewById(R.id.color_presenter);

        tv_hsl = findViewById(R.id.tv_hsl);
        sb_value = findViewById(R.id.sb_value);

        tv_red = findViewById(R.id.tv_red);
        tv_green = findViewById(R.id.tv_green);
        tv_blue = findViewById(R.id.tv_blue);


        sb_red = findViewById(R.id.sb_red);
        sb_green = findViewById(R.id.sb_green);
        sb_blue = findViewById(R.id.sb_blue);

        sb_red.setOnSeekBarChangeListener(onSeekBarChangeListener);
        sb_green.setOnSeekBarChangeListener(onSeekBarChangeListener);
        sb_blue.setOnSeekBarChangeListener(onSeekBarChangeListener);


        sb_value.setOnSeekBarChangeListener(onSeekBarChangeListener);
        tv_value = findViewById(R.id.tv_value);

        sb_hue = findViewById(R.id.sb_hue);
        sb_sat = findViewById(R.id.sb_sat);
        sb_lit = findViewById(R.id.sb_lit);
        tv_hue = findViewById(R.id.tv_hue);
        tv_sat = findViewById(R.id.tv_sat);
        tv_lit = findViewById(R.id.tv_lit);

        sb_hue.setOnSeekBarChangeListener(onSeekBarChangeListener);
        sb_sat.setOnSeekBarChangeListener(onSeekBarChangeListener);
        sb_lit.setOnSeekBarChangeListener(onSeekBarChangeListener);
    }


    private ColorPanel.ColorChangeListener colorChangeListener = new ColorPanel.ColorChangeListener() {

        @Override
        public void onColorChanged(float[] hsv, boolean touchStopped) {
            int color = Color.HSVToColor(hsv);
            float[] hslValue = new float[3];
            ColorUtils.colorToHSL(color, hslValue);
            refreshDesc(hslValue, color, (int) (hsv[2] * 100));
            long currentTime = System.currentTimeMillis();
            if ((currentTime - preTime) >= DELAY_TIME || touchStopped) {
                preTime = currentTime;
                sendHslSetMessage(hslValue);
            } else {
                MeshLogger.log("CMD reject : color set", MeshLogger.LEVEL_INFO);
            }
        }
    };

    private View.OnTouchListener colorPanelTouchListener = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            if (event.getAction() == MotionEvent.ACTION_UP) {
                v.getParent().requestDisallowInterceptTouchEvent(false);
            } else {
                v.getParent().requestDisallowInterceptTouchEvent(true);
            }
            return false;
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

            if (seekBar == sb_value) {
                float visibility = ((float) progress) / 100;
                if (color_panel != null) {
                    color_panel.setVisibility(visibility, immediate);
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

                float[] hslValue = new float[3];
                ColorUtils.colorToHSL(color, hslValue);
                refreshDesc(hslValue, color, (int) (hsv[2] * 100));

                if ((currentTime - preTime) >= DELAY_TIME || immediate) {
                    preTime = currentTime;
                    sendHslSetMessage(hslValue);
                }
            } else if (seekBar == sb_hue || seekBar == sb_sat || seekBar == sb_lit) {
                long currentTime = System.currentTimeMillis();

                float hue = sb_hue.getProgress();
                float sat100 = sb_sat.getProgress();
                float lit100 = sb_lit.getProgress();
                float[] hslVal = new float[]{hue, sat100 / 100, lit100 / 100};
                int color = ColorUtils.HSLToColor(hslVal);

                float[] hsv = new float[3];
                Color.colorToHSV(color, hsv);

                refreshDesc(hslVal, color, (int) (hsv[2] * 100));

                if ((currentTime - preTime) >= DELAY_TIME || immediate) {
                    preTime = currentTime;
                    sendHslSetMessage(hslVal);
                }

            }
        }


    };

    public void updateLightness(int lightnessProgress) {
        if (sb_lit != null) {
            sb_lit.setProgress(lightnessProgress);

            float hue = sb_hue.getProgress();
            float sat100 = sb_sat.getProgress();
            float lit100 = sb_lit.getProgress();
            float[] hslVal = new float[]{hue, sat100 / 100, lit100 / 100};
            int color = ColorUtils.HSLToColor(hslVal);
            float[] hsv = new float[3];
            Color.colorToHSV(color, hsv);
            refreshDesc(hslVal, color, (int) (hsv[2] * 100));
        }
    }

    private void sendHslSetMessage(float[] hslValue) {
        if (messageDelegate != null) {
            messageDelegate.onHSLMessage(hslValue);
        }
    }

    private void refreshDesc(float[] hslValue, int color, int value100) {
        color_presenter.setBackgroundColor(color);

        sb_value.setProgress(value100);
        tv_value.setText("V: " + value100);

        int red = (color >> 16) & 0xFF;
        int green = (color >> 8) & 0xFF;
        int blue = color & 0xFF;

        sb_red.setProgress(red);
        sb_green.setProgress(green);
        sb_blue.setProgress(blue);

        tv_red.setText("R: " + String.format("%03d", red));
        tv_green.setText("G: " + String.format("%03d", green));
        tv_blue.setText("B: " + String.format("%03d", blue));

        // Hue Saturation Hue
        // float[] hslValue = new float[3];
        String hsl = "HSL: \n\tH -- " + (hslValue[0]) + "(" + (byte) (hslValue[0] * 100 / 360) + ")" +
                "\n\tS -- " + (hslValue[1]) + "(" + (byte) (hslValue[1] * 100) + ")" +
                "\n\tL -- " + (hslValue[2] + "(" + (byte) (hslValue[2] * 100) + ")"
        );
        tv_hsl.setText(hsl);


        int hue = (int) hslValue[0];
        int sat = (int) (hslValue[1] * 100);
        int lit = (int) (hslValue[2] * 100);
        sb_hue.setProgress(hue);
        sb_sat.setProgress(sat);
        sb_lit.setProgress(lit);

        tv_hue.setText("H: " + hue);
        tv_sat.setText("S: " + sat);
        tv_lit.setText("L: " + lit);
    }

    public interface ColorMessageDelegate {
        void onHSLMessage(float[] hsl);
    }

}
