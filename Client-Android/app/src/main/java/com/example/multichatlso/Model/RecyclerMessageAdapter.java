package com.example.multichatlso.Model;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.example.multichatlso.R;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Locale;

public class RecyclerMessageAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {

    public static int user_id;
    private static final int INCOMING_MESSAGE = 1;
    private static final int OUTCOMING_MESSAGE = 2;

    private static final int ADMIN_MESSAGE = 0;

    private ArrayList<Message> messages;

    public RecyclerMessageAdapter (ArrayList<Message> messages){
        this.messages = messages;
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        if (INCOMING_MESSAGE == viewType){
            View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.out,parent,false);
            return new ViewHolderIncoming(view);
        }else if (OUTCOMING_MESSAGE == viewType){
            View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.me,parent,false);
            return new ViewHolderOutcoming(view);
        }else if (ADMIN_MESSAGE == viewType){
            View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.mid,parent,false);
            return new ViewHolderMid(view);
        }
        return null;
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
        Message message = messages.get(position);

        if (INCOMING_MESSAGE == holder.getItemViewType()){
            ViewHolderIncoming incoming = (ViewHolderIncoming) holder;
            incoming.setDetail(message);
        }else if (OUTCOMING_MESSAGE == holder.getItemViewType()){
            ViewHolderOutcoming outcoming = (ViewHolderOutcoming) holder;
            outcoming.setDetail(message);
        }else if (ADMIN_MESSAGE == holder.getItemViewType()) {
            ViewHolderMid mid = (ViewHolderMid) holder;
            mid.setDetail(message);
        }
    }

    @Override
    public int getItemViewType(int position) {//TODO: Add logic
        if (messages.get(position).getUser_id() == user_id){
            return OUTCOMING_MESSAGE;
        }else if ((messages.get(position).getUser_id()==0)){
             return ADMIN_MESSAGE;
        }else return INCOMING_MESSAGE;
    }

    @Override
    public int getItemCount() {
        return messages.size();
    }

    public class ViewHolderIncoming extends RecyclerView.ViewHolder{

        private TextView textMessage;
        private TextView senderName;

        private TextView textDate;

        public ViewHolderIncoming(@NonNull View itemView) {
            super(itemView);
            //textMessage = itemView.findViewById(R.id.textMessage);
            textMessage = itemView.findViewById(R.id.text_gchat_message_other);
            senderName = itemView.findViewById(R.id.text_gchat_user_other);
            textDate = itemView.findViewById(R.id.text_gchat_timestamp_other);
        }

        public void setDetail(Message message) {
            System.out.println(message.toString());
            textMessage.setText(message.getText());
            senderName.setText(String.valueOf(message.getName()));
            Date currentTime = new Date();
            SimpleDateFormat sdf = new SimpleDateFormat("HH:mm", Locale.getDefault());
            String formattedTime = sdf.format(currentTime);
            textDate.setText(formattedTime);
        }
    }

    public class ViewHolderMid extends RecyclerView.ViewHolder{
        private TextView textMessage;

        public ViewHolderMid(@NonNull View itemView) {
            super(itemView);
            textMessage = itemView.findViewById(R.id.text_gchat_message_admin);
        }
        public void setDetail(Message message) {
            textMessage.setText(message.getText());
        }
    }

    public class ViewHolderOutcoming extends RecyclerView.ViewHolder{

        private TextView textMessage;
        private TextView textDate;

        public ViewHolderOutcoming(@NonNull View itemView) {
            super(itemView);
            //textMessage = itemView.findViewById(R.id.textMessage2);
            textMessage = itemView.findViewById(R.id.text_gchat_message_me);
            textDate = itemView.findViewById(R.id.text_gchat_timestamp_me);
        }

        public void setDetail(Message message) {
            textMessage.setText(message.getText());
            Date currentTime = new Date();
            SimpleDateFormat sdf = new SimpleDateFormat("HH:mm", Locale.getDefault());
            String formattedTime = sdf.format(currentTime);
            textDate.setText(formattedTime);
        }
    }
}
