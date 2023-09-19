/********************************************************************************************************
 * @file FileSearchTask.java
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

import android.content.ContentResolver;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Handler;
import android.provider.MediaStore;

import java.io.File;
import java.lang.ref.WeakReference;

public class FileSearchTask extends AsyncTask<String, File, Void> {

    private WeakReference<Context> mContextReference;

    public static final int MSG_SEARCH_START = 0x01;

    public static final int MSG_SEARCH_COMPLETE = 0x02;

    public static final int MSG_SEARCH_ITEM = 0x03;

    private Handler mHandler;

    FileSearchTask(Context context, Handler handler) {
        mContextReference = new WeakReference<Context>(context);
        mHandler = handler;
    }

    @Override
    protected void onPostExecute(Void aVoid) {
        super.onPostExecute(aVoid);
        onStateChanged(MSG_SEARCH_COMPLETE, null);
    }

    @Override
    protected void onProgressUpdate(File... values) {
        super.onProgressUpdate(values);
        if (values != null && values.length == 1) {
            onStateChanged(MSG_SEARCH_ITEM, values[0]);
        }
    }

    @Override
    protected void onPreExecute() {
        super.onPreExecute();
        onStateChanged(MSG_SEARCH_START, null);
    }

    @Override
    protected Void doInBackground(String... strings) {
        if (strings == null || strings.length != 1) {
            return null;
        }
        searchInput(mContextReference.get(), strings[0]);
        return null;
    }

    @Override
    protected void onCancelled() {
        super.onCancelled();
        onStateChanged(MSG_SEARCH_COMPLETE, null);
    }

    private void onStateChanged(int state, Object object) {
        if (mHandler != null) {
            mHandler.obtainMessage(state, object).sendToTarget();
        }
    }

    private void searchInput(Context context, String input) {
        if (context == null) return;
        ContentResolver resolver = context.getContentResolver();
        Uri uri = MediaStore.Files.getContentUri("external");
        Cursor cursor = resolver.query(uri,
                new String[]{MediaStore.Files.FileColumns.DATA, MediaStore.Files.FileColumns.SIZE},
                MediaStore.Files.FileColumns.TITLE + " LIKE '%" + input + "%'",
                null, null);
        if (cursor != null) {
            while (cursor.moveToNext()) {
//                FileSearchActivity.FileSearchResult bean = new FileSearchActivity.FileSearchResult();
                String path = cursor.getString(cursor.getColumnIndexOrThrow(MediaStore.Files.FileColumns.DATA));
                File file = new File(path);
                if (file.exists() && file.isFile()) {
                    publishProgress(file);
                }
            }
            cursor.close();
        }
    }
}
