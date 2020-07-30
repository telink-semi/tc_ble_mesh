/********************************************************************************************************
 * @file     FileSearchActivity.java 
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
package com.telink.ble.mesh.ui.file;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;


/**
 * Created by ke on 2019/12/30.
 */
public class FileSearchActivity extends AppCompatActivity {

    private ListView lv_search_results;
    List<File> searchResults = new ArrayList<>();
    private FileListAdapter mAdapter;
    private EditText et_search_input;

    private ProgressBar pb_search;

    @SuppressLint("HandlerLeak")
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case FileSearchTask.MSG_SEARCH_START:
                    pb_search.setVisibility(View.VISIBLE);
                    pb_search.setIndeterminate(true);
                    searchResults.clear();
                    mAdapter.setData(searchResults);
                    break;

                case FileSearchTask.MSG_SEARCH_COMPLETE:
                    pb_search.setIndeterminate(false);
                    pb_search.setVisibility(View.INVISIBLE);
                    break;
                case FileSearchTask.MSG_SEARCH_ITEM:
                    searchResults.add((File) msg.obj);
                    mAdapter.setData(searchResults);
                    break;
            }


        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_search);
        initTitle();
        initView();
    }


    private void initTitle() {
        Toolbar toolbar = findViewById(R.id.title_bar);
        TextView tv_title = toolbar.findViewById(R.id.tv_title);
        tv_title.setText("Search");
        toolbar.setNavigationOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });
    }

    private void initView() {
        et_search_input = findViewById(R.id.et_search_input);
        pb_search = findViewById(R.id.pb_search);
        lv_search_results = findViewById(R.id.lv_search_results);
        mAdapter = new FileListAdapter(this, "bin");
        mAdapter.setSearchMode(true);
        lv_search_results.setAdapter(mAdapter);

        lv_search_results.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                Intent intent = new Intent();
                intent.putExtra(FileSelectActivity.KEY_RESULT, searchResults.get(position).getAbsolutePath());
                setResult(RESULT_OK, intent);
                finish();
            }
        });

        findViewById(R.id.iv_search).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                search();
            }
        });

        et_search_input.setOnEditorActionListener(new TextView.OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
                if (actionId == EditorInfo.IME_ACTION_SEARCH) {
                    search();
                    return true;
                }
                return false;
            }
        });
    }


    private void search() {
        String input = et_search_input.getText().toString().trim();
        if (TextUtils.isEmpty(input)) {
            return;
        }
        InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
        if (imm != null) {
            imm.hideSoftInputFromWindow(et_search_input.getWindowToken(), 0);
        }
        Log.d("search", input);
        FileSearchTask task = new FileSearchTask(this, handler);
        task.execute(input);
    }

}
