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