package com.telink.ble.mesh.ui.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;

import androidx.recyclerview.widget.RecyclerView;

/**
 * models list adapter
 */
public class SimpleTextAdapter extends BaseRecyclerViewAdapter<SimpleTextAdapter.ViewHolder> {
    String[] texts;
    Context mContext;
    boolean selectMode = false;

    public SimpleTextAdapter(Context context, String[] texts) {
        mContext = context;
        this.texts = texts;
    }

    public void setSelectMode(boolean selectMode) {
        this.selectMode = selectMode;
        this.notifyDataSetChanged();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        TextView textView = new TextView(mContext);
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_simple_text, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        return holder;
    }

    @Override
    public int getItemCount() {
        return texts == null ? 0 : texts.length;
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        ((TextView) holder.itemView).setText(texts[position]);
    }

    class ViewHolder extends RecyclerView.ViewHolder {
        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
