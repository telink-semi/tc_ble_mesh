/********************************************************************************************************
 * @file     HexFormatTextWatcher.java 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
package com.telink.ble.mesh.ui.widget;

import android.text.Editable;
import android.text.TextWatcher;
import android.widget.TextView;

import com.telink.ble.mesh.util.Arrays;

public class HexFormatTextWatcher implements TextWatcher {
    private TextView target;

    public HexFormatTextWatcher(TextView textView) {
        this.target = textView;
    }

    @Override
    public void beforeTextChanged(CharSequence s, int start, int count, int after) {
    }

    @Override
    public void onTextChanged(CharSequence s, int start, int before, int count) {
        dealInput(s.toString(), target);
    }

    @Override
    public void afterTextChanged(Editable s) {
    }

    private void dealInput(String input, TextView textView) {

        byte[] params = Arrays.hexToBytes(input);
        if (params != null) {
            textView.setText(Arrays.bytesToHexString(params, " "));
        } else {
            textView.setText("");
        }
    }
}