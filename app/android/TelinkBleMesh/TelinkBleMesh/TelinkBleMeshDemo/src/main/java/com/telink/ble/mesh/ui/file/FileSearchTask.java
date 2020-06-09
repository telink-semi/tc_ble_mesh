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
                if (file.exists() && file.isFile()){
                    publishProgress(file);
                }
            }
            cursor.close();
        }
    }
}
