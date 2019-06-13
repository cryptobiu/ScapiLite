package crypto.cs.biu.scapilite.adapter;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import java.util.Date;
import java.util.List;
import java.util.Random;

import crypto.cs.biu.scapilite.R;
import crypto.cs.biu.scapilite.model.Poll;
import crypto.cs.biu.scapilite.util.DateTimeParser;

/**
 * Created by Inellipse on 23.4.2018.
 */

public class PollAdapter extends RecyclerView.Adapter<PollAdapter.ViewHolder> {
    private List<Poll> data;
    private Context context;
    private OnPollClick mOnItemClickListener;


    public PollAdapter(Context context, List<Poll> data, OnPollClick mOnItemClickListener) {
        this.context = context;
        this.data = data;
        this.mOnItemClickListener = mOnItemClickListener;
    }

    // Replace the contents of a view (invoked by the layout manager)
    @Override
    public void onBindViewHolder(ViewHolder holder, final int position) {
        final Poll item = data.get(position);


//        if (item.getProduct() != null) {
//            holder.item_defaultName.setText(item.getProduct().getTitle());
//            if (item.getProduct().getImageUrl() != null && !item.getProduct().getImageUrl().isEmpty()) {
//                Picasso.with(context).load(AppConfig.BASE_URL_IMAGES + item.getProduct().getImageUrl()).into(holder.item_image);
//            } else {
//                holder.item_image.setImageResource(R.drawable.ipko_logo);
//            }
//
//        } else {
//            holder.item_image.setImageResource(R.drawable.ipko_logo);
//            holder.item_defaultName.setText(item.getDefaultName());
//        }
//
//        if (item.getDetails() != null) {
//
//            String details = "";
//            for (String s : item.getDetails()) {
//                details += s + "\n";
//            }
//            holder.item_description.setText(details);
//        } else {
//            holder.item_description.setText("");
//        }
        holder.item_poll_title.setText(item.getTitle());
        holder.item_poll_description.setText(item.getDescription());
        holder.item_poll_time.setText(DateTimeParser.parseStartTimeHHmmCET(item.getExecutionTime()) + " CET");
        if (item.getExecutionTime() < new Date().getTime()) {
            holder.item_poll_status.setText(context.getString(R.string.completed) + " (" + new Random().nextInt(1000) + ")");
            holder.item_poll_status.setBackgroundResource(R.drawable.xml_back_completed);
        } else {
            holder.item_poll_status.setText(context.getString(R.string.active) + " (" + new Random().nextInt(1000) + ")");
            holder.item_poll_status.setBackgroundResource(R.drawable.xml_back_active);
        }

    }

    public class ViewHolder extends RecyclerView.ViewHolder implements View.OnClickListener {
        public final TextView item_poll_title;
        public final TextView item_poll_description;
        public final TextView item_poll_time;
        public final TextView item_poll_status;

        public ViewHolder(View itemView) {
            super(itemView);
            itemView.setClickable(true);
            item_poll_title = (TextView) itemView.findViewById(R.id.item_poll_title);
            item_poll_description = (TextView) itemView.findViewById(R.id.item_poll_description);
            item_poll_time = (TextView) itemView.findViewById(R.id.item_poll_time);
            item_poll_status = (TextView) itemView.findViewById(R.id.item_poll_status);

            itemView.setOnClickListener(this);

        }

        @Override
        public void onClick(View v) {
            itemClick(this, getLayoutPosition());
        }
    }


    private void itemClick(final ViewHolder holder, int position) {

        mOnItemClickListener.onPollClick(data.get(position));
    }

    // Create new views (invoked by the layout manager)
    @Override
    public PollAdapter.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        // create a new view
        View v = LayoutInflater.from(parent.getContext()).inflate(R.layout.item_poll, parent, false);
        ViewHolder vh = new ViewHolder(v);
        return vh;
    }


    // Return the size of your dataset (invoked by the layout manager)
    @Override
    public int getItemCount() {
        return data.size();
    }


    public interface OnPollClick {
        public void onPollClick(Poll item);
    }
}
