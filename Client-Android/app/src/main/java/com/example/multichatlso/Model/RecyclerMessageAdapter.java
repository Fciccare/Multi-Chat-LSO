package com.example.multichatlso.Model;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RelativeLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.example.multichatlso.R;

import java.util.ArrayList;

public class RecyclerMessageAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {

    private static int INCOMING_MESSAGE = 1;
    private static int OUTCOMING_MESSAGE = 2;

    private ArrayList<Message> messages;

    public RecyclerMessageAdapter (ArrayList<Message> messages){
        this.messages = messages;
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        if (INCOMING_MESSAGE == viewType){
            View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.message_layout,parent,false);
            return new ViewHolderIncoming(view);
        }else if (OUTCOMING_MESSAGE == viewType){
            View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.message_layout_2,parent,false);
            return new ViewHolderOutcoming(view);
        }
        return null;
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
        Message message = messages.get(position);

        if (INCOMING_MESSAGE == holder.getItemViewType()){
            ViewHolderIncoming incoming = (ViewHolderIncoming) holder;
            incoming.textMessage.setText(message.getText());
        }else if (OUTCOMING_MESSAGE == holder.getItemViewType()){
            ViewHolderOutcoming outcoming = (ViewHolderOutcoming) holder;
            outcoming.textMessage.setText(message.getText());
        }

    }

    @Override
    public int getItemViewType(int position) {
        if(messages.get(position).getUser().equals("5"))
            return OUTCOMING_MESSAGE;
        else return INCOMING_MESSAGE;
    }

    @Override
    public int getItemCount() {
        return messages.size();
    }

    public class ViewHolderIncoming extends RecyclerView.ViewHolder{

        private TextView textMessage;
        //private TextView senderName;

        public ViewHolderIncoming(@NonNull View itemView) {
            super(itemView);
            textMessage = itemView.findViewById(R.id.textMessage);
        }

        public void setDetail(Message message) {
            textMessage.setText(message.getText());
        }
    }

    public class ViewHolderOutcoming extends RecyclerView.ViewHolder{

        private TextView textMessage;
        //private TextView senderName;

        public ViewHolderOutcoming(@NonNull View itemView) {
            super(itemView);
            textMessage = itemView.findViewById(R.id.textMessage2);
        }

        public void setDetail(Message message) {
            textMessage.setText(message.getText());
        }
    }
}
