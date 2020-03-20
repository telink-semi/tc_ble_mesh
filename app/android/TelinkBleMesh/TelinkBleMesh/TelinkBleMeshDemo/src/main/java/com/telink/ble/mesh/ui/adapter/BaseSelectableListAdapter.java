package com.telink.ble.mesh.ui.adapter;


import androidx.recyclerview.widget.RecyclerView;

/**
 * beans
 */
public abstract class BaseSelectableListAdapter<V extends RecyclerView.ViewHolder> extends BaseRecyclerViewAdapter<V> {
    SelectStatusChangedListener statusChangedListener;

    public abstract boolean allSelected();

    public abstract void setAll(boolean selected);


    public interface SelectStatusChangedListener {
        void onStatusChanged(BaseSelectableListAdapter adapter);
    }

    public void setStatusChangedListener(SelectStatusChangedListener statusChangedListener) {
        this.statusChangedListener = statusChangedListener;
    }
}
