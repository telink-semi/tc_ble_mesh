package com.telink.sig.mesh.demo;

/**
 * Created by kee on 2018/5/15.
 */

public class ActivityManager {
    private static final ActivityManager ourInstance = new ActivityManager();

    public static ActivityManager getInstance() {
        return ourInstance;
    }

    private static final String TAG = "ActivityManager";

    private int frontCnt;


    private ActivityManager() {
        frontCnt = 0;
    }

    public void onActivityStart() {
        frontCnt++;
    }

    public void onActivityStop() {
        frontCnt--;
    }

    public boolean isApplicationForeground() {
        return frontCnt > 0;
    }

}
