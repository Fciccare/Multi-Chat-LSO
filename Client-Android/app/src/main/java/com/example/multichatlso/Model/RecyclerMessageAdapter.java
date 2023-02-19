package com.example.multichatlso.Model;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.example.multichatlso.R;

import java.util.ArrayList;

public class RecyclerMessageAdapter extends RecyclerView.Adapter<RecyclerMessageAdapter.ViewHolder> {

    private ArrayList<Message> messages;

    public RecyclerMessageAdapter (ArrayList<Message> messages){
        this.messages = messages;
    }

    @NonNull
    @Override
    public RecyclerMessageAdapter.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.card,parent,false);
        return new RecyclerMessageAdapter.ViewHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerMessageAdapter.ViewHolder holder, int position) {
        Message message = messages.get(position);
        holder.setDetail(message);
    }

    @Override
    public int getItemCount() {
        return messages.size();
    }

    public class ViewHolder extends RecyclerView.ViewHolder{

        private TextView textMessage;
        private TextView senderName;

        public ViewHolder(final View view) {
            super(view);
            textMessage = view.findViewById(R.id.txtRoomCardName);
            senderName = view.findViewById(R.id.txtRoomCardCounter);
        }

        public void setDetail(Message message) {
            textMessage.setText(message.getText());
            String messageResult = "Sended: " + message.getUser();
            senderName.setText(messageResult);
            senderName.setCompoundDrawables(null, null, null, null);
        }
    }
}
