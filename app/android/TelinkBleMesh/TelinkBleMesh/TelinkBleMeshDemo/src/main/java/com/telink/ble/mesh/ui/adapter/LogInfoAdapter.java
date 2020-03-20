package com.telink.ble.mesh.ui.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.LogInfo;

import java.text.SimpleDateFormat;
import java.util.List;
import java.util.Locale;

import androidx.recyclerview.widget.RecyclerView;

/**
 * 分组列表 in GroupFragment
 * Created by kee on 2017/8/21.
 */

public class LogInfoAdapter extends BaseRecyclerViewAdapter<LogInfoAdapter.ViewHolder> {

    private Context mContext;
    private List<LogInfo> mLogs;
    private SimpleDateFormat mDateFormat = new SimpleDateFormat("MM-dd HH:mm:ss.SSS", Locale.getDefault());

    public LogInfoAdapter(Context context, List<LogInfo> logs) {
        this.mContext = context;
        this.mLogs = logs;
    }

    public void refreshLogs(List<LogInfo> logs) {
        this.mLogs = logs;
        this.notifyDataSetChanged();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_log_info, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_name = itemView.findViewById(R.id.tv_log);
        return holder;
    }

    @Override
    public int getItemCount() {
        return mLogs == null ? 0 : mLogs.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        LogInfo logInfo = mLogs.get(position);
        String info = mDateFormat.format(logInfo.datetime) + "/" + logInfo.tag + " : " + logInfo.log;
        holder.tv_name.setText(info);
    }


    class ViewHolder extends RecyclerView.ViewHolder {
        TextView tv_name;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
