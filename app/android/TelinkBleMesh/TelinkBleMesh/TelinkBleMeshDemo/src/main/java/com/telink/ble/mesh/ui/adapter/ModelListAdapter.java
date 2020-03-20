package com.telink.ble.mesh.ui.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.core.message.MeshSigModel;

import java.util.List;

import androidx.recyclerview.widget.RecyclerView;

/**
 * models list adapter
 */
public class ModelListAdapter extends BaseRecyclerViewAdapter<ModelListAdapter.ViewHolder> {
    List<MeshSigModel> models;
    Context mContext;
    boolean selectMode = false;

    public ModelListAdapter(Context context, List<MeshSigModel> modelList) {
        mContext = context;
        models = modelList;
    }

    public void setSelectMode(boolean selectMode) {
        this.selectMode = selectMode;
        this.notifyDataSetChanged();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_setting_model, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_id = (TextView) itemView.findViewById(R.id.tv_model_id);
        holder.tv_desc = (TextView) itemView.findViewById(R.id.tv_model_desc);
        holder.iv_select = itemView.findViewById(R.id.iv_select);
        return holder;
    }

    @Override
    public int getItemCount() {
        return models == null ? 0 : models.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);

        MeshSigModel model = models.get(position);

        holder.tv_id.setText("id: 0x" + Integer.toHexString(model.modelId));
        holder.tv_desc.setText("modelName: " + model.modelName);
        if (selectMode && model.selected) {
            holder.iv_select.setVisibility(View.VISIBLE);
        } else {
            holder.iv_select.setVisibility(View.INVISIBLE);
        }

    }

    class ViewHolder extends RecyclerView.ViewHolder {


        public ImageView iv_select;
        public TextView tv_id, tv_desc;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
