package com.example.multichatlso.View;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;

import com.example.multichatlso.Model.RecyclerRoomAdapter;
import com.example.multichatlso.Model.Room;
import com.example.multichatlso.R;
import com.example.multichatlso.Model.Server;
import com.google.android.material.dialog.MaterialAlertDialogBuilder;

import java.util.ArrayList;
import java.util.List;

import es.dmoral.toasty.Toasty;

public class HomepageActivity extends AppCompatActivity {

    private static final String TAG = "HomepageActivity";
    private ArrayList<Room> rooms;
    private RecyclerView recyclerView;
    private RecyclerRoomAdapter adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_homepage);

        Button buttonSKT = findViewById(R.id.buttonSocket);
        Button buttonLST = findViewById(R.id.buttonList);
        EditText txtSocket = findViewById(R.id.editTextSocket);


        recyclerView = findViewById(R.id.recyclerView);
        rooms = new ArrayList<Room>();
        adapter = new RecyclerRoomAdapter(rooms);
        RecyclerView.LayoutManager layoutManager = new LinearLayoutManager(getApplicationContext());
        recyclerView.setLayoutManager(layoutManager);
        recyclerView.setAdapter(adapter);


        buttonSKT.setOnClickListener(view -> {
            String message = txtSocket.getText().toString();

            Server.getInstance().write(message);
            String result = Server.getInstance().read();

            Log.d(TAG, "Risposta della socket: " + result);
            Toasty.success(this, result, Toasty.LENGTH_LONG, true).show();
            txtSocket.setText("");
        });

        buttonLST.setOnClickListener(view -> {
            listRoom();
        });
    }

    private void listRoom(){
        String message = "[LST]";
        Server.getInstance().write(message);

        String result = "";

        do {
            result += Server.getInstance().read() + "\n";
        }while (!result.contains("[/LST]"));

        Log.d(TAG, "Lista di stanza: " + result);
        List<Room> roomArrayList = castListToRoom(result);
        rooms.clear();
        rooms.addAll(roomArrayList);
        Log.d(TAG, "Lista Room: " + rooms);
        adapter.notifyDataSetChanged();

    }

    private List<Room> castListToRoom(String list){
        String[] lines = list.split("\n");

        List<Room> rooms = new ArrayList<Room>();
        int max_client = 0;

        for(String s: lines){
            if(s.contains("[LST]")){
                s = s.replace("[LST]", "");
                max_client = Integer.parseInt(s.trim());
            }else if(s.contains("<>")){
                String[] splitted = s.split("<>");
                int id = Integer.parseInt(splitted[0].trim());
                int online_client = Integer.parseInt(splitted[2]);
                rooms.add(new Room(id, splitted[1], online_client, max_client));
            }
        }

        return rooms;

    }

    @Override
    public void onBackPressed() {
        Log.d(TAG, "Tasto Back premuto");
        new MaterialAlertDialogBuilder(this)
                .setTitle("Uscire dall'App?")
                .setMessage("Vuoi uscire dall'App?")
                .setPositiveButton("Sì", (dialogInterface, i) -> {
                    Intent homeIntent = new Intent(Intent.ACTION_MAIN);
                    homeIntent.addCategory( Intent.CATEGORY_HOME );
                    homeIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                    startActivity(homeIntent);
                })
                .setNegativeButton("No", (dialogInterface, i) -> {})
                .show();
    }
}