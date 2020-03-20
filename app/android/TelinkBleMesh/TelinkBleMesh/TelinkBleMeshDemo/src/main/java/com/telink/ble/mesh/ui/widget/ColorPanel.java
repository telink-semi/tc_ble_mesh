package com.telink.ble.mesh.ui.widget;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ComposeShader;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.graphics.RadialGradient;
import android.graphics.RectF;
import android.graphics.Shader;
import android.graphics.SweepGradient;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

/**
 * Created by kee on 2018/7/10.
 */

public class ColorPanel extends View {

    /**
     * Customizable display parameters (in percents)
     */

    private Paint colorWheelPaint;

    private Paint colorPointerPaint;
    private RectF colorPointerCoords;


    private Bitmap colorWheelBitmap;


    private int colorWheelRadius;


    /**
     * Currently selected color
     */
    private float[] colorHSV = new float[]{0f, 0f, 1f};

    private ColorChangeListener colorChangeListener;

    public ColorPanel(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        init();
    }

    public ColorPanel(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public ColorPanel(Context context) {
        super(context);
        init();
    }

    private void init() {

        colorPointerPaint = new Paint();
        colorPointerPaint.setStyle(Paint.Style.STROKE);
        colorPointerPaint.setStrokeWidth(2f);
        colorPointerPaint.setARGB(0xFF, 0xFF, 0xFF, 0xFF);


        colorWheelPaint = new Paint();
        colorWheelPaint.setAntiAlias(true);
        colorWheelPaint.setDither(true);

        colorPointerCoords = new RectF();

    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        int widthSize = MeasureSpec.getSize(widthMeasureSpec);
        int heightSize = MeasureSpec.getSize(heightMeasureSpec);
        int size = Math.min(widthSize, heightSize);
        setMeasuredDimension(size, size);
    }

    @SuppressLint("DrawAllocation")
    @Override
    protected void onDraw(Canvas canvas) {

        int centerX = getWidth() / 2;
        int centerY = getHeight() / 2;

        // drawing color wheel

        canvas.drawBitmap(colorWheelBitmap, centerX - colorWheelRadius, centerY - colorWheelRadius, null);


        float hueAngle = (float) Math.toRadians(colorHSV[0]);
        int colorPointX = (int) (-Math.cos(hueAngle) * colorHSV[1] * colorWheelRadius) + centerX;
        int colorPointY = (int) (-Math.sin(hueAngle) * colorHSV[1] * colorWheelRadius) + centerY;

        /*float pointerRadius = 0.075f * colorWheelRadius;
        int pointerX = (int) (colorPointX - pointerRadius / 2);
        int pointerY = (int) (colorPointY - pointerRadius / 2);

        colorPointerCoords.set(pointerX, pointerY, pointerX + pointerRadius, pointerY + pointerRadius);
        canvas.drawOval(colorPointerCoords, colorPointerPaint);*/

    }

    @Override
    protected void onSizeChanged(int width, int height, int oldw, int oldh) {

        colorWheelRadius = width / 2;
        colorWheelBitmap = createColorWheelBitmap(colorWheelRadius * 2, colorWheelRadius * 2);
    }

    private Bitmap createColorWheelBitmap(int width, int height) {

        Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);

        int colorCount = 12;
        int colorAngleStep = 360 / 12;
        int colors[] = new int[colorCount + 1];
        float hsv[] = new float[]{0f, 1f, 1f};
        for (int i = 0; i < colors.length; i++) {
            hsv[0] = (i * colorAngleStep + 180) % 360;
            colors[i] = Color.HSVToColor(hsv);
        }
        colors[colorCount] = colors[0];

        SweepGradient sweepGradient = new SweepGradient(width / 2, height / 2, colors, null);
        RadialGradient radialGradient = new RadialGradient(width / 2, height / 2, colorWheelRadius, 0xFFFFFFFF, 0x00FFFFFF, Shader.TileMode.CLAMP);
        ComposeShader composeShader = new ComposeShader(sweepGradient, radialGradient, PorterDuff.Mode.SRC_OVER);

        colorWheelPaint.setShader(composeShader);

        Canvas canvas = new Canvas(bitmap);
        canvas.drawCircle(width / 2, height / 2, colorWheelRadius, colorWheelPaint);

        return bitmap;

    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int action = event.getAction();
        switch (action) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_MOVE:
            case MotionEvent.ACTION_UP:

                int x = (int) event.getX();
                int y = (int) event.getY();
                int cx = x - getWidth() / 2;
                int cy = y - getHeight() / 2;
                double d = Math.sqrt(cx * cx + cy * cy);

                if (d <= colorWheelRadius) {

                    colorHSV[0] = (float) (Math.toDegrees(Math.atan2(cy, cx)) + 180f);
                    colorHSV[1] = Math.max(0f, Math.min(1f, (float) (d / colorWheelRadius)));

                    invalidate();

                }
                onColorChanged(MotionEvent.ACTION_UP == action);
//                return false;
        }
        return true;
    }



    public void setColorChangeListener(ColorChangeListener listener) {
        this.colorChangeListener = listener;
    }

    private void onColorChanged(boolean touchStopped) {
        if (colorChangeListener != null) {
            colorChangeListener.onColorChanged(colorHSV, touchStopped);
        }
    }

    /**
     * @param visibility 0f - 1.0f
     */
    public void setVisibility(float visibility, boolean touchStopped) {
//        colorHSV[2] = (float) Math.min(1, 1 - ((float) progress) / 100);
        colorHSV[2] = visibility;
        invalidate();
        onColorChanged(touchStopped);
    }


    public void setColor(int color) {
        Color.colorToHSV(color, colorHSV);
    }

    public float[] getColor() {
//        return Color.HSVToColor(new float[]{colorHSV[0], colorHSV[1], 1f});
        return colorHSV;
    }

    public interface ColorChangeListener {
        void onColorChanged(float[] hsv, boolean touchStopped);
    }
}

