package com.example.multichatlso.View;

import android.content.Intent;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.Observer;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;

import com.example.multichatlso.Model.Room;
import com.example.multichatlso.Model.Server;
import com.example.multichatlso.R;
import com.google.android.material.floatingactionbutton.ExtendedFloatingActionButton;

import es.dmoral.toasty.Toasty;


public class InsertRoomFragment extends Fragment {

    private static InsertRoomFragment insertRoomFragment = null;
    private static final String TAG = "InsertRoomFragment";

    private EditText txtNameRoom;
    private ExtendedFloatingActionButton button;

    public InsertRoomFragment() {
        // Required empty public constructor
    }

    public static InsertRoomFragment getInstance(){
        if(insertRoomFragment == null)
            insertRoomFragment = new InsertRoomFragment();
        return insertRoomFragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //insertRoomFragment.().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_RESIZE);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_insert_room, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        txtNameRoom = view.findViewById(R.id.editTextRoomName);
        button = view.findViewById(R.id.buttonRoomCreate);

        button.setOnClickListener(view1 -> {
            createRoom();
        });

    }

    private void createRoom() {
        String roomName = txtNameRoom.getText().toString();
        if(roomName.length() > 32 || roomName.length() == 0){
            Toasty.error(getContext(), "Nome della stanza non accettato").show();
            return;
        }

        String message = "[CRT]"+roomName;
        Server.getInstance().write(message);
        Log.d(TAG, "Write to server: " + message);
        Server.getInstance().read();

        Server.getInstance().getListen().singleObserve(this, response -> {
            Log.d(TAG, "Observer started");
            if(response == null){
                Log.e(TAG, "Socket read null");
                Toasty.error(requireContext(), "Errore creazione stanza, riprova").show();
                return;
            }
            Log.d(TAG, "Server responde: " + response);
            if(!response.toLowerCase().contains("successful")){
                Toasty.error(requireContext(), "Errore creazione stanza").show();
            }

            String[] splitted = response.trim().split("<>");
            int id = Integer.parseInt(splitted[1]);

            Room room = new Room(id, roomName, 1, 32); //TODO:Fix hardcoded MAX_CLIENT

            Log.d(TAG, "Room creata, passaggio schermata");

            Intent i = new Intent(getActivity(), RoomActivity.class);
            i.putExtra("Room", room);
            startActivity(i);
        });

    }
}