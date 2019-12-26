package com.telink.sig.mesh.demo.ui.widget;

import android.content.Context;
import android.os.Parcelable;
import android.support.design.widget.BottomNavigationView;
import android.util.AttributeSet;

public class CustomBottomNavigationView extends BottomNavigationView {

    public CustomBottomNavigationView(Context context) {
        super(context);
    }

    public CustomBottomNavigationView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public CustomBottomNavigationView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    @Override
    protected Parcelable onSaveInstanceState() {
        super.onSaveInstanceState();
        return null;
    }

    @Override
    protected void onRestoreInstanceState(Parcelable state) {
        super.onRestoreInstanceState(state);
    }
}
