/********************************************************************************************************
 * @file FileSelectActivity.java
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
package com.telink.ble.mesh.ui.file;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import com.telink.ble.mesh.demo.R;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;


/**
 * Created by ke on 2019/12/30.
 */
public class FileSelectActivity extends AppCompatActivity {

    public static final String KEY_SUFFIX = "com.telink.file.selector.suffix";

    public static final String KEY_TITLE = "com.telink.file.selector.title";

    public static final String KEY_RESULT = "Result";

    private static final int REQUEST_CODE_SEARCH = 0x01;

    private ListView lv_file;
    private ImageView tv_parent;
    private TextView tv_cur_name; // current dir
    private FileListAdapter mAdapter;
    private List<File> mFiles = new ArrayList<>();
    private File mCurrentDir;

    private static final String DEFAULT_FILE_SUFFIX = ".bin";

    private String fileSuffix = DEFAULT_FILE_SUFFIX;

    private static final String DEFAULT_TITLE = "File Selector";

    private static final int SORT_BY_NAME = 0x00;

    private static final int SORT_BY_TIME = 0x01;

    private static final int SORT_BY_TYPE = 0x02;

    private static final int SORT_BY_SIZE = 0x03;

    private int sortType = SORT_BY_NAME;

    /*
            "Sort By Type",
            "Sort By Size"
     */
    private static final String[] SORTS = {"By Name",
            "By Time",};

    @SuppressLint("HandlerLeak")
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            lv_file.setSelection(0);
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_select);

        Intent intent = getIntent();
        if (intent.hasExtra(KEY_SUFFIX)) {
            fileSuffix = intent.getStringExtra(KEY_SUFFIX);
        }
        initTitle(intent.getStringExtra(KEY_TITLE));
        initView();
        getCacheDirPath();
        update();
    }

    private void getCacheDirPath() {
        String savedDir = FileSelectorCache.getDirPath(this);
        if (savedDir == null || !new File(savedDir).exists()) {
            mCurrentDir = Environment.getExternalStorageDirectory();
        } else {
            mCurrentDir = new File(savedDir);
        }
    }

    private void changeToRootDir() {
        mCurrentDir = Environment.getExternalStorageDirectory();
    }

    private void showSortDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Sort Type");
        builder.setItems(SORTS, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                if (which != sortType) {
                    sortType = which;
                    update();
                    dialog.dismiss();
                }
            }
        });
        builder.show();
    }


    private void initTitle(String title) {
        Toolbar toolbar = findViewById(R.id.title_bar);
        TextView tv_title = toolbar.findViewById(R.id.tv_title);
        tv_title.setText(title == null ? DEFAULT_TITLE : title);
        toolbar.setNavigationOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });
        toolbar.inflateMenu(R.menu.file_select);

        toolbar.setOnMenuItemClickListener(item -> {
            if (item.getItemId() == R.id.item_root) {
                changeToRootDir();
                update();
            } else if (item.getItemId() == R.id.item_sort) {
                showSortDialog();
            } else if (item.getItemId() == R.id.item_search) {
                onSearchClick();
            } /*else if (item.getItemId() == R.id.item_setting) {
                onSettingClick();
            }*/
            return false;
        });
    }

    private void initView() {
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
        mAdapter = new FileListAdapter(this, fileSuffix);
        lv_file.setAdapter(mAdapter);
        lv_file.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                if (mFiles.get(position).isDirectory()) {
                    mCurrentDir = mFiles.get(position);
                    update();
                } else {
                    FileSelectorCache.saveDirPath(FileSelectActivity.this, mCurrentDir.getAbsolutePath());
                    String filePath = mFiles.get(position).getAbsolutePath();
                    Intent intent = new Intent();
                    intent.putExtra(KEY_RESULT, filePath);
                    setResult(RESULT_OK, intent);
                    finish();
                }
            }
        });

        // show copy and time
        /*lv_file.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
            @Override
            public boolean onItemLongClick(AdapterView<?> parent, View view, int position, long id) {
                PopupMenu popup = new PopupMenu(FileSelectActivity.this, view);
                MenuInflater inflater = popup.getMenuInflater();
                popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
                    @Override
                    public boolean onMenuItemClick(MenuItem item) {

                        return false;
                    }
                });
                inflater.inflate(R.menu.file_popup_info, popup.getMenu());

                popup.getMenu().findItem(R.id.item_copy).setTitle("");
                popup.show();
                return true;
            }
        });*/
    }

    private void onSearchClick() {
        startActivityForResult(new Intent(this, FileSearchActivity.class), REQUEST_CODE_SEARCH);
    }

    private void onSettingClick() {
//        startActivity(new Intent(this, FileSearchActivity.class));
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
            /*if (focus) {
                Iterator<File> fileIterator = mFiles.iterator();
                File file;
                while (fileIterator.hasNext()) {
                    file = fileIterator.next();
                    if (file.isFile() && !file.getName().toLowerCase().endsWith(FILE_SUFFIX)) {
                        fileIterator.remove();
                    }
                }
            }*/
            // sort
            Collections.sort(mFiles, new Comparator<File>() {
                @Override
                public int compare(File o1, File o2) {
                    if (o1.isDirectory() && o2.isFile())
                        return -1;
                    if (o1.isFile() && o2.isDirectory())
                        return 1;
                    if (sortType == SORT_BY_NAME) {
                        return o1.getName().toUpperCase().compareTo(o2.getName().toUpperCase());
                    } else {
                        return Long.compare(o2.lastModified(), o1.lastModified());
                    }
                        /*if (sortType == SORT_BY_TIME) {
                        return Long.compare(o2.lastModified(), o1.lastModified());
                    } else if (sortType == SORT_BY_TYPE) {

                    } else if (sortType == SORT_BY_SIZE) {

                    }*/
                }
            });

        }
        mAdapter.setData(mFiles);
        handler.sendEmptyMessage(0);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == REQUEST_CODE_SEARCH && resultCode == RESULT_OK) {
            setResult(RESULT_OK, data);
            finish();
        }
    }
}
