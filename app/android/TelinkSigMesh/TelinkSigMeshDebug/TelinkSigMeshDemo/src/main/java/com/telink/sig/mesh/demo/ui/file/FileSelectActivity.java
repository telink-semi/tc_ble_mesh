/********************************************************************************************************
 * @file FileSelectActivity.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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
package com.telink.sig.mesh.demo.ui.file;

import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.TextView;


import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.SharedPreferenceHelper;
import com.telink.sig.mesh.demo.ui.BaseActivity;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

/**
 * Created by ke on 2016/10/8.
 */
public class FileSelectActivity extends BaseActivity {
    private ListView lv_file;
    private TextView tv_parent;
    private TextView tv_cur_name; // 当前目录名称
    private FileListAdapter mAdapter;
    private List<File> mFiles = new ArrayList<>();
    private File mCurrentDir;
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            lv_file.setSelection(0);
        }
    };
    public static final String FILE_SUFFIX = "com.telink.sig.mesh.FILE_SUFFIX";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_select);
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.file_select);
        toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                if (item.getItemId() == R.id.item_root) {
                    mCurrentDir = Environment.getExternalStorageDirectory();
                    update();
                }
                return false;
            }
        });
        setTitle("Select File");
        enableBackNav(true);
        initView();
        update();
    }

    private void initView() {

        final Intent intent = getIntent();
        String targetSuffix = ".bin";
        if (intent.hasExtra(FILE_SUFFIX)) {
            targetSuffix = intent.getStringExtra(FILE_SUFFIX);
        }

        lv_file = findViewById(R.id.lv_file);
        tv_cur_name = findViewById(R.id.tv_cur_name);
        tv_parent = findViewById(R.id.tv_parent);
        tv_parent.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mCurrentDir = mCurrentDir.getParentFile();
                update();
            }
        });
        String savedDir = SharedPreferenceHelper.getDirPath(this);
        if (savedDir == null || !new File(savedDir).exists()) {
            mCurrentDir = Environment.getExternalStorageDirectory();
        } else {
            mCurrentDir = new File(savedDir);
        }
//        mCurrentDir = getFilesDir();
        mAdapter = new FileListAdapter(this, targetSuffix);
        lv_file.setAdapter(mAdapter);
        lv_file.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                if (mFiles.get(position).isDirectory()) {
                    mCurrentDir = mFiles.get(position);
                    update();
                } else {
                    SharedPreferenceHelper.saveDirPath(FileSelectActivity.this, mCurrentDir.getAbsolutePath());
                    Intent intent = new Intent();
                    intent.putExtra("path", mFiles.get(position).getAbsolutePath());
                    setResult(RESULT_OK, intent);
                    finish();
                }
            }
        });
    }


    private void update() {
        if (mCurrentDir.getParentFile() != null) {
            tv_parent.setVisibility(View.VISIBLE);
        } else {
            tv_parent.setVisibility(View.INVISIBLE);
        }
        tv_cur_name.setText(mCurrentDir.toString());
        File[] files = mCurrentDir.listFiles();
        if (files == null) {
            mFiles.clear();
        } else {
            mFiles = new ArrayList<>(Arrays.asList(files));

            // 排序
            Collections.sort(mFiles, new Comparator<File>() {
                @Override
                public int compare(File o1, File o2) {
                    if (o1.isDirectory() && o2.isFile())
                        return -1;
                    if (o1.isFile() && o2.isDirectory())
                        return 1;
                    return o1.getName().toUpperCase().compareTo(o2.getName().toUpperCase());
                }
            });
        }
        mAdapter.setData(mFiles);
        handler.sendEmptyMessage(0);
    }

}
