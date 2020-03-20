package com.telink.ble.mesh.ui;

import com.telink.ble.mesh.demo.R;

/**
 * Created by kee on 2018/9/29.
 */

public class IconGenerator {

    /**
     * @param type  0: common device, 1: lpn
     * @param onOff -1: offline; 0: off, 1: on
     * @return res
     */
    public static int getIcon(int type, int onOff) {
        if (type == 1){
            return R.drawable.ic_low_power;
        }else {
            if (onOff == -1) {
                return R.drawable.ic_bulb_offline;
            } else if (onOff == 0) {
                return R.drawable.ic_bulb_off;
            } else {
            /*if (deviceInfo.lum == 100) {
                return R.drawable.ic_bulb_on;
            } else {
                return R.drawable.ic_bulb_on_half;
            }*/
                return R.drawable.ic_bulb_on;
            }
        }
    }

    public static int generateDeviceIconRes(int onOff) {
//        return R.drawable.ic_low_power;
        if (onOff == -1) {
            return R.drawable.ic_bulb_offline;
        } else if (onOff == 0) {
            return R.drawable.ic_bulb_off;
        } else {
            /*if (deviceInfo.lum == 100) {
                return R.drawable.ic_bulb_on;
            } else {
                return R.drawable.ic_bulb_on_half;
            }*/
            return R.drawable.ic_bulb_on;
        }
    }

}
