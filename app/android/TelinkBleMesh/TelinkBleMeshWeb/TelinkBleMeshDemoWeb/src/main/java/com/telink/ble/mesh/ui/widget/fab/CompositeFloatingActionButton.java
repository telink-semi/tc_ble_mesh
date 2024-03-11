package com.telink.ble.mesh.ui.widget.fab;

import android.animation.Animator;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.content.Context;
import android.content.res.ColorStateList;
import android.content.res.TypedArray;
import android.graphics.Color;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.util.Log;
import android.util.TypedValue;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.BounceInterpolator;
import android.view.animation.LinearInterpolator;

import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.telink.ble.mesh.demo.R;


public class CompositeFloatingActionButton extends ViewGroup {

    public final static int POS_LEFT_BOTTOM = 0;
    public final static int POS_RIGHT_BOTTOM = 1;

    public static final int ANIM_FADE = 0;
    public static final int ANIM_SCALE = 1;
    public static final int ANIM_BOUNCE = 2;

    //该View包含的默认子View
    private View mBackgroundView;
    private FloatingActionButton mFloatingActionButton;

    //该View的属性
    private int mBackgroundColor;
    private Drawable mFabIcon;
    private ColorStateList mFabColor;
    private int mAnimationDuration;
    private int mAnimationMode;
    private int mPosition;

    //主Fab是否被点开
    private boolean isMenuOpen;

    private OnFabItemClickListener mOnFabItemClickListener;

    public interface OnFabItemClickListener{
        void onFabItemClick(SubFabLayout view, int pos);
    }

    public void setOnFabItemClickListener(OnFabItemClickListener onFabItemClickListener){
        mOnFabItemClickListener = onFabItemClickListener;
    }



    public CompositeFloatingActionButton(Context context) {
        this(context, null);
    }

    public CompositeFloatingActionButton(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public CompositeFloatingActionButton(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        //获取属性值
        getAttributes(context, attrs);
        //添加一个背景View和一个FloatingActionButton
        setBaseViews(context);
    }

    private void getAttributes(Context context, AttributeSet attrs){
        TypedArray typedArray = context.obtainStyledAttributes(attrs, R.styleable.CompositeFloatingActionButton);
        mBackgroundColor = typedArray.getColor(
                R.styleable.CompositeFloatingActionButton_backgroundColor, Color.TRANSPARENT);
        mFabIcon = typedArray.getDrawable(R.styleable.CompositeFloatingActionButton_switchFabIcon);
        mFabColor = typedArray.getColorStateList(R.styleable.CompositeFloatingActionButton_switchFabColor);
        mAnimationDuration = typedArray.getInt(R.styleable.CompositeFloatingActionButton_animationDuration, 150);
        mAnimationMode = typedArray.getInt(R.styleable.CompositeFloatingActionButton_animationMode, ANIM_SCALE);
        mPosition = typedArray.getInt(R.styleable.CompositeFloatingActionButton_position, POS_RIGHT_BOTTOM);
        typedArray.recycle();
    }

    private void setBaseViews(Context context){
        mBackgroundView = new View(context);
        mBackgroundView.setBackgroundColor(mBackgroundColor);
        mBackgroundView.setAlpha(0);
        addView(mBackgroundView);

        mFloatingActionButton = new FloatingActionButton(context);
        mFloatingActionButton.setBackgroundTintList(mFabColor);
        mFloatingActionButton.setImageDrawable(mFabIcon);
        addView(mFloatingActionButton);
    }


    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        int count = getChildCount();
        for(int i=0; i<count; i++){
            measureChild(getChildAt(i), widthMeasureSpec, heightMeasureSpec);
        }
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        if(changed){
            //布局背景和主Fab
            layoutFloatingActionButton();
            layoutBackgroundView();
            layoutItems();
        }
    }

    @Override
    public boolean onInterceptTouchEvent(MotionEvent ev) {
        boolean intercepted = false;
        int x = (int)ev.getX();
        int y = (int)ev.getY();
        if(isMenuOpen){
            switch (ev.getAction()){
                case MotionEvent.ACTION_DOWN:
                    if(judgeIfTouchBackground(x, y)){
                        intercepted = true;
                    }
                    intercepted = false;
                    break;
                case MotionEvent.ACTION_MOVE:
                    intercepted = false;
                    break;
                case MotionEvent.ACTION_UP:
                    intercepted = false;
                    break;
            }
        }
        return intercepted;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if(isMenuOpen){
            closeMenu();
            changeBackground();
            rotateFloatingButton();
            changeStatus();
            return true;
        }
        return super.onTouchEvent(event);
    }

    private boolean judgeIfTouchBackground(int x, int y){
        Rect a = new Rect();
        Rect b = new Rect();
        a.set(0, 0, getWidth(), getHeight() - getChildAt(getChildCount() - 1).getTop());
        b.set(0, getChildAt(getChildCount() - 1).getTop(), getChildAt(getChildCount() - 1).getLeft(), getHeight());
        if(a.contains(x, y) || b.contains(x, y)){
            return true;
        }
        return false;
    }


    private void layoutFloatingActionButton(){
        int width = mFloatingActionButton.getMeasuredWidth();
        int height = mFloatingActionButton.getMeasuredHeight();
        int fl = 0;
        int ft = 0;
        int fr = 0;
        int fb = 0;

        switch (mPosition){
            case POS_LEFT_BOTTOM:
            case POS_RIGHT_BOTTOM:
                fl = getMeasuredWidth() - width - dp2px(8);
                ft = getMeasuredHeight() - height - dp2px(8);
                fr = fl + width;
                fb = ft + height;
                break;

        }
        mFloatingActionButton.layout(fl, ft, fr, fb);
        bindFloatingEvent();
    }

    private void layoutBackgroundView(){
        mBackgroundView.layout(0, 0
                , getMeasuredWidth(), getMeasuredHeight());
        //bindBackgroundEvent();
    }

    private void layoutItems(){
        int count = getChildCount();
        for(int i=2; i<count; i++) {
            SubFabLayout child = (SubFabLayout) getChildAt(i);
            child.setVisibility(INVISIBLE);

            int width = child.getMeasuredWidth();
            int height = child.getMeasuredHeight();
            Log.e("子View的宽高是", width + "*" + height);

            int fabHeight = mFloatingActionButton.getMeasuredHeight();

            int cl = 0;
            int ct = 0;

            switch (mPosition) {
                case POS_LEFT_BOTTOM:
                case POS_RIGHT_BOTTOM:
                    cl = getMeasuredWidth() - width - dp2px(8);
                    ct = getMeasuredHeight() - fabHeight - (i - 1) * height - dp2px(8);
            }
            Log.e("子View的坐标是", cl + "+" + ct);
            child.layout(cl, ct, cl + width, ct + height);
            bindMenuEvents(child, i);
            prepareAnim(child);
        }
    }

    private void bindFloatingEvent(){
        mFloatingActionButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                rotateFloatingButton();
                changeBackground();
                changeStatus();
                if (isMenuOpen) {
                    openMenu();
                } else {
                    closeMenu();
                }
            }
        });
    }

    private void bindMenuEvents(final SubFabLayout child, final int pos){
        child.setOnTagClickListener(new SubFabLayout.OnTagClickListener() {
            @Override
            public void onTagClick() {
                rotateFloatingButton();
                changeBackground();
                changeStatus();
                closeMenu();
                if(mOnFabItemClickListener != null){
                    mOnFabItemClickListener.onFabItemClick(child, pos);
                }
            }
        });

        child.setOnFabClickListener(new SubFabLayout.OnFabClickListener() {
            @Override
            public void onFabClick() {
                rotateFloatingButton();
                changeBackground();
                changeStatus();
                closeMenu();
                if (mOnFabItemClickListener != null) {
                    mOnFabItemClickListener.onFabItemClick(child, pos);
                }
            }
        });
    }

    private void prepareAnim(SubFabLayout child){
        switch (mAnimationMode){
            case ANIM_BOUNCE:
                child.setTranslationY(50);
                break;
            case ANIM_SCALE:
                child.setScaleX(0f);
                child.setScaleY(0f);
                break;
        }
    }


    private void rotateFloatingButton(){
        ObjectAnimator animator = isMenuOpen ? ObjectAnimator.ofFloat(mFloatingActionButton
                , "rotation", 45F, 0f) : ObjectAnimator.ofFloat(mFloatingActionButton, "rotation", 0f, 45f);
        animator.setDuration(150);
        animator.setInterpolator(new LinearInterpolator());
        animator.start();
    }

    private void changeBackground(){
        ObjectAnimator animator = isMenuOpen ? ObjectAnimator.ofFloat(mBackgroundView, "alpha", 0.9f, 0f) :
                ObjectAnimator.ofFloat(mBackgroundView, "alpha", 0f, 0.9f);
        animator.setDuration(150);
        animator.setInterpolator(new LinearInterpolator());
        animator.start();
    }

    private void changeStatus(){
        if(isMenuOpen){
            isMenuOpen = false;
        }else{
            isMenuOpen = true;
        }
    }

    private void openMenu(){
        switch (mAnimationMode){
            case ANIM_BOUNCE:
                bounceToShow();
                break;
            case ANIM_SCALE:
                scaleToShow();
        }
    }

    private void bounceToShow(){
        for(int i=2; i<getChildCount(); i++){
            View view = getChildAt(i);
            view.setVisibility(VISIBLE);

            ObjectAnimator trans = ObjectAnimator.ofFloat(view, "translationY", 50f, 0);
            ObjectAnimator show = ObjectAnimator.ofFloat(view, "alpha", 0f, 1f);
            AnimatorSet set = new AnimatorSet();
            set.play(trans).with(show);
            set.setDuration(mAnimationDuration);
            set.setInterpolator(new BounceInterpolator());
            set.start();

        }
    }

    private void scaleToShow(){
        for(int i = 2; i<getChildCount(); i++){
            View view = getChildAt(i);
            view.setVisibility(VISIBLE);
            view.setAlpha(0);
            ObjectAnimator scaleX = ObjectAnimator.ofFloat(view, "scaleX", 0f, 1f);
            ObjectAnimator scaleY = ObjectAnimator.ofFloat(view, "scaleY", 0f, 1f);
            ObjectAnimator alpha = ObjectAnimator.ofFloat(view, "alpha", 0f, 1f);
            AnimatorSet set = new AnimatorSet();
            set.playTogether(scaleX, scaleY, alpha);
            set.setDuration(mAnimationDuration);
            set.start();
        }
    }


    private void closeMenu(){
        for(int i=2; i<getChildCount(); i++){
            final View view = getChildAt(i);
            ObjectAnimator alpha = ObjectAnimator.ofFloat(view, "alpha", 1f, 0f);
            alpha.setDuration(mAnimationDuration);
            alpha.addListener(new Animator.AnimatorListener() {
                @Override
                public void onAnimationStart(Animator animation) {

                }

                @Override
                public void onAnimationEnd(Animator animation) {
                    view.setVisibility(INVISIBLE);
                }

                @Override
                public void onAnimationCancel(Animator animation) {

                }

                @Override
                public void onAnimationRepeat(Animator animation) {

                }
            });
            alpha.start();
        }
    }

    private int dp2px(int value) {
        return (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP
                , value, getResources().getDisplayMetrics());

    }

    public void setAnimationDuration(int duration){
        mAnimationDuration = duration;
    }

    public void setSwitchFabColor(ColorStateList color){
        mFloatingActionButton.setBackgroundTintList(color);
    }

    public void setTagBackgroundColor(int color){
        for(int i=2; i<getChildCount(); i++){
            SubFabLayout subFabLayout = (SubFabLayout) getChildAt(i);
            subFabLayout.setBackgroundColor(color);
        }
    }

    public void setTextColor(int color){
        for(int i=2; i<getChildCount(); i++){
            SubFabLayout subFabLayout = (SubFabLayout) getChildAt(i);
            subFabLayout.setTextColor(color);
        }
    }

    public void setBackgroundColor(int color){
        mBackgroundColor = color;
        mBackgroundView.setBackgroundColor(color);
    }

    public void setFabIcon(Drawable icon){
        mFloatingActionButton.setImageDrawable(icon);
    }

    public boolean getButtonState(){
        return isMenuOpen;
    }
}