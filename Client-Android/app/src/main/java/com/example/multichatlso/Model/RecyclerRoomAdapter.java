package com.example.multichatlso.Model;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.example.multichatlso.R;

import java.util.ArrayList;

public class RecyclerRoomAdapter extends RecyclerView.Adapter<RecyclerRoomAdapter.ViewHolder> {

    private ArrayList<Room> rooms;

    public RecyclerRoomAdapter(ArrayList<Room> rooms){
        this.rooms = rooms;
    }

    @NonNull
    @Override
    public RecyclerRoomAdapter.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.card,parent,false);
        return new ViewHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerRoomAdapter.ViewHolder holder, int position) {
        Room room = rooms.get(position);
        holder.setDetail(room);
    }

    @Override
    public int getItemCount() {
        return rooms.size();
    }

    public class ViewHolder extends RecyclerView.ViewHolder{

        private TextView roomName;
        private TextView roomCounter;

        public ViewHolder(final View view) {
            super(view);
            roomName = view.findViewById(R.id.txtRoomCardName);
            roomCounter = view.findViewById(R.id.txtRoomCardCounter);
        }

        public void setDetail(Room room) {
            roomName.setText(room.getName());
            roomCounter.setText(String.format("%d/%d", room.getClients_counter(), room.getMax_client()));
        }
    }
}
