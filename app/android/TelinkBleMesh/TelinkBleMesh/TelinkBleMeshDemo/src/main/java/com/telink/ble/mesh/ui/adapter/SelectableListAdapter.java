package com.telink.ble.mesh.ui.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;

import com.telink.ble.mesh.demo.R;

import androidx.recyclerview.widget.RecyclerView;

/**
 * beans
 */
public class SelectableListAdapter extends BaseSelectableListAdapter<SelectableListAdapter.ViewHolder> {
    SelectableBean[] beans;
    Context mContext;

    public SelectableListAdapter(Context context, SelectableBean[] beans) {
        this.mContext = context;
        this.beans = beans;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_selectable, null, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.cb_month = itemView.findViewById(R.id.cb_month);
        return holder;
    }

    @Override
    public int getItemCount() {
        return beans == null ? 0 : beans.length;
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        SelectableBean month = beans[position];
        holder.cb_month.setText(month.name);
        holder.cb_month.setChecked(month.selected);
        holder.cb_month.setTag(position);
        holder.cb_month.setOnCheckedChangeListener(checkedChangeListener);
    }

    public boolean allSelected() {
        for (SelectableBean month : beans) {
            if (!month.selected) {
                return false;
            }
        }
        return true;
    }

    public void setAll(boolean selected) {
        for (SelectableBean month : beans) {
            month.selected = selected;
        }
        notifyDataSetChanged();
    }

    public int getBinaryResult() {
        int result = 0b0000;
        for (int i = 0; i < beans.length; i++) {
            if (beans[i].selected) {
                result |= (0b0001 << i);
            }
        }
        return result;
    }

    private CompoundButton.OnCheckedChangeListener checkedChangeListener = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            int position = (int) buttonView.getTag();
            beans[position].selected = isChecked;
            if (statusChangedListener != null) {
                statusChangedListener.onStatusChanged(SelectableListAdapter.this);
            }
        }
    };

    class ViewHolder extends RecyclerView.ViewHolder {


        public CheckBox cb_month;


        public ViewHolder(View itemView) {
            super(itemView);
        }
    }

    public static class SelectableBean {
        public String name;
        public boolean selected;

        public SelectableBean(String name) {
            this.name = name;
            this.selected = false;
        }
    }
    
}
