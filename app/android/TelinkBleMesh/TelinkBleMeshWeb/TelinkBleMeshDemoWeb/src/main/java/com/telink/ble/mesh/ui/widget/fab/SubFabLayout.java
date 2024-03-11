package com.telink.ble.mesh.ui.widget.fab;

import android.content.Context;
import android.content.res.TypedArray;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.View;
import android.view.ViewGroup;

import com.telink.ble.mesh.demo.R;


public class SubFabLayout extends ViewGroup {
    private String mTagText;
    private SubTagView mTagView;

    private OnTagClickListener mOnTagClickListener;
    private OnFabClickListener mOnFabClickListener;

    public interface OnTagClickListener {
        void onTagClick();
    }

    public interface OnFabClickListener {
        void onFabClick();
    }

    public void setOnTagClickListener(OnTagClickListener onTagClickListener) {
        mOnTagClickListener = onTagClickListener;
    }

    public void setOnFabClickListener(OnFabClickListener onFabClickListener) {
        mOnFabClickListener = onFabClickListener;
    }

    public SubFabLayout(Context context) {
        this(context, null);
    }

    public SubFabLayout(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public SubFabLayout(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        getAttributes(context, attrs);
        settingTagView(context);
    }

    private void getAttributes(Context context, AttributeSet attributeSet) {
        TypedArray typedArray = context.obtainStyledAttributes(attributeSet
                , R.styleable.FabTagLayout);
        mTagText = typedArray.getString(R.styleable.FabTagLayout_tagText);
        typedArray.recycle();
    }

    private void settingTagView(Context context) {
        mTagView = new SubTagView(context);
        mTagView.setTagText(mTagText);
        addView(mTagView);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);

        int width = 0;
        int height = 0;

        int count = getChildCount();
        for (int i = 0; i < count; i++) {

            View view = getChildAt(i);

            measureChild(view, widthMeasureSpec, heightMeasureSpec);

            width += view.getMeasuredWidth();
            height = Math.max(height, view.getMeasuredHeight());
        }

        width += dp2px(8 + 8 + 8);
        height += dp2px(8 + 8);

        //直接将该ViewGroup设定为wrap_content的
        setMeasuredDimension(width, height);
    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        //为子View布局
        View tagView = getChildAt(0);
        View fabView = getChildAt(1);

        int tagWidth = tagView.getMeasuredWidth();
        int tagHeight = tagView.getMeasuredHeight();

        int fabWidth = fabView.getMeasuredWidth();
        int fabHeight = fabView.getMeasuredHeight();

        int tl = dp2px(8);
        int tt = (getMeasuredHeight() - tagHeight) / 2;
        int tr = tl + tagWidth;
        int tb = tt + tagHeight;

        int fl = tr + dp2px(8);
        int ft = (getMeasuredHeight() - fabHeight) / 2;
        int fr = fl + fabWidth;
        int fb = ft + fabHeight;

        fabView.layout(fl, ft, fr, fb);
        tagView.layout(tl, tt, tr, tb);
        bindEvents(tagView, fabView);
    }

    private void bindEvents(View tagView, View fabView) {
        tagView.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mOnTagClickListener != null) {
                    mOnTagClickListener.onTagClick();
                }
            }
        });

        fabView.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mOnFabClickListener != null) {
                    mOnFabClickListener.onFabClick();
                }
            }
        });
    }

    public void setBackgroundColor(int color) {
        mTagView.setBackgroundColor(color);
    }

    public void setTextColor(int color) {
        mTagView.setTextColor(color);
    }

    private int dp2px(int value) {
        return (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP
                , value, getResources().getDisplayMetrics());

    }
}