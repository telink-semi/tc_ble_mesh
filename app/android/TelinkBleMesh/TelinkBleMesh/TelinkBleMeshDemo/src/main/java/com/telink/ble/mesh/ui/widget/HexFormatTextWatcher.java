/********************************************************************************************************
 * @file HexFormatTextWatcher.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2017
 *
 * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
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