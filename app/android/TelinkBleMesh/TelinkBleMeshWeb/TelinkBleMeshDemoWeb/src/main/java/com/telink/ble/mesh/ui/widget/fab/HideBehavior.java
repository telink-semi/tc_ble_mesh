package com.telink.ble.mesh.ui.widget.fab;



import android.content.Context;
import android.os.Build;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.view.animation.Interpolator;

import androidx.coordinatorlayout.widget.CoordinatorLayout;
import androidx.core.view.ViewCompat;
import androidx.core.view.ViewPropertyAnimatorListenerAdapter;
import androidx.interpolator.view.animation.FastOutSlowInInterpolator;


/**
 * Created by Lenovo.
 */

public class HideBehavior extends CoordinatorLayout.Behavior{
    private static final Interpolator INTERPOLATOR = new FastOutSlowInInterpolator();
    private boolean mIsAnimatingOut = false;


    public HideBehavior() {
    }

    public HideBehavior(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    public boolean onStartNestedScroll(CoordinatorLayout coordinatorLayout
            , View child
            , View directTargetChild
            , View target
            , int nestedScrollAxes) {

       return nestedScrollAxes== ViewGroup.SCROLL_AXIS_VERTICAL||
               super.onStartNestedScroll(coordinatorLayout, child, directTargetChild, target,
                       nestedScrollAxes);

    }


    /*@Override
    public void onNestedPreScroll(CoordinatorLayout coordinatorLayout
            , View child
            , View target
            , int dx
            , int dy
            , int[] consumed) {
        Log.e("onNestedPreScroll", "进入");

        if(dx > 0 && ! mIsAnimatingOut *//*&& child.getVisibility() == View.VISIBLE*//*){
            animateOut(child);
        }else if(dy < 0 *//*&& child.getVisibility() != View.VISIBLE*//*){
            animateIn(child);
        }


    }*/

    @Override
    public void onNestedScroll(CoordinatorLayout coordinatorLayout, View child, View target, int dxConsumed, int dyConsumed, int dxUnconsumed, int dyUnconsumed) {
        super.onNestedScroll(coordinatorLayout, child, target, dxConsumed, dyConsumed, dxUnconsumed,
                dyUnconsumed);
        Log.e("onNestedScroll", "进入");
        if(dyConsumed > 0 /*&& ! mIsAnimatingOut && child.getVisibility() == View.VISIBLE*/){
            Log.e("animateOut", "进入");
            //fab划出去
            animateOut(child);
        }else if(dyConsumed < 0 /*&& child.getVisibility() != View.VISIBLE*/){
            Log.e("animateIn", "进入");
            //fab划进来
            animateIn(child);
        }
    }

    private void animateOut(View child) {
        Log.e("animateOut", "进入");
        //属性动画
        //设置监听判断状态
        ViewCompat.animate(child)
                .scaleX(0.0f)
                .scaleY(0.0f)
                .alpha(0.0f)
                .setInterpolator(INTERPOLATOR)
                .setListener(new ViewPropertyAnimatorListenerAdapter(){
            @Override
            public void onAnimationStart(View view) {
                mIsAnimatingOut = true;
                super.onAnimationStart(view);
            }

            @Override
            public void onAnimationCancel(View view) {
                mIsAnimatingOut = false;
                super.onAnimationCancel(view);
            }

            @Override
            public void onAnimationEnd(View view) {
                //view.setVisibility(View.GONE);
                mIsAnimatingOut = false;
                super.onAnimationEnd(view);
            }
        }).start();
    }


    /*private void animateOut(final View button) {
        if (Build.VERSION.SDK_INT >= 14) {
            ViewCompat.animate(button)
                    .scaleX(0.0F)
                    .scaleY(0.0F)
                    .alpha(0.0F)
                    .setInterpolator(INTERPOLATOR)
                    .withLayer()
                    .setListener(new ViewPropertyAnimatorListener() {
                        public void onAnimationStart(View view) {
                            mIsAnimatingOut = true;
                        }

                        public void onAnimationCancel(View view) {
                            mIsAnimatingOut = false;
                        }

                        public void onAnimationEnd(View view) {
                            mIsAnimatingOut = false;
                            view.setVisibility(View.GONE);
                        }
                    }).start();
        } else {
            Animation anim = AnimationUtils.loadAnimation(button.getContext()
                    , R.anim.design_fab_out);
            anim.setInterpolator(INTERPOLATOR);
            anim.setDuration(200L);
            anim.setAnimationListener(new Animation.AnimationListener() {
                public void onAnimationStart(Animation animation) {
                    mIsAnimatingOut = true;
                }

                public void onAnimationEnd(Animation animation) {
                    mIsAnimatingOut = false;
                    button.setVisibility(View.GONE);
                }

                @Override
                public void onAnimationRepeat(final Animation animation) {
                }
            });
            button.startAnimation(anim);
        }
    }*/

    //滑进来
    private void animateIn(View child) {
        Log.e("animateIn", "进入");
        //child.setVisibility(View.VISIBLE);
        //属性动画
        ViewCompat.animate(child)
                .scaleX(1.0f)
                .scaleY(1.0f)
                .alpha(1.0f)
                .setInterpolator(INTERPOLATOR)
                .setListener(null)
                .start();
    }

    /*private void animateIn(View button) {
        button.setVisibility(View.VISIBLE);
        if (Build.VERSION.SDK_INT >= 14) {
            ViewCompat.animate(button).scaleX(1.0F).scaleY(1.0F).alpha(1.0F)
                    .setInterpolator(INTERPOLATOR).withLayer().setListener(null)
                    .start();
        } else {
            Animation anim = AnimationUtils.loadAnimation(button.getContext(), R.anim.design_fab_in);
            anim.setDuration(200L);
            anim.setInterpolator(INTERPOLATOR);
            button.startAnimation(anim);
        }
    }*/


    @Override
    public void onStopNestedScroll(CoordinatorLayout coordinatorLayout, View child, View target) {
        super.onStopNestedScroll(coordinatorLayout, child, target);
    }
}
