package com.example.multichatlso.View;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.AutoCompleteTextView;
import android.widget.Button;

import com.example.multichatlso.Model.Message;
import com.example.multichatlso.Model.RecyclerMessageAdapter;
import com.example.multichatlso.Model.RecyclerRoomAdapter;
import com.example.multichatlso.Model.Room;
import com.example.multichatlso.Model.Server;
import com.example.multichatlso.R;
import com.google.android.material.dialog.MaterialAlertDialogBuilder;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import es.dmoral.toasty.Toasty;

public class RoomActivity extends AppCompatActivity {

    private static final String TAG = "RoomActivity";

    private Room room;
    private ArrayList<Message> messages;

    private AutoCompleteTextView textView;
    private Button button;
    private RecyclerView recyclerView;
    private RecyclerMessageAdapter adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_room);

        Bundle bundle = getIntent().getExtras();
        if (bundle != null) {
            room = (Room) bundle.getSerializable("Room");
            Log.d(TAG, "Ricevuta Room: " + room.toString());
        }

        textView = findViewById(R.id.textMessage);
        button = findViewById(R.id.buttonSend);
        recyclerView = findViewById(R.id.recyclerView2);

        Toasty.success(getBaseContext(), room.toString()).show();

        messages = new ArrayList<Message>();
        adapter = new RecyclerMessageAdapter(messages);
        RecyclerView.LayoutManager layoutManager = new LinearLayoutManager(getApplicationContext());
        recyclerView.setLayoutManager(layoutManager);
        recyclerView.setAdapter(adapter);

        fetchMessage();

        button.setOnClickListener(view -> {
            sendMessage();
        });

    }

    private void sendMessage(){
        String message = textView.getText().toString();
        message = "[MSG]" + message + "<>" + room.getId();
        Server.getInstance().write(message);
        Log.d(TAG, "Send to server: " + message);
        textView.setText("");
    }

    private void fetchMessage(){
        ExecutorService executor = Executors.newSingleThreadExecutor();
        //Handler h = new Handler(Looper.getMainLooper());
        executor.execute(() -> {
            new Thread(() -> {
                while(true){
                    String result = Server.getInstance().read();
                    Log.d(TAG, "Server sended: " + result);
                    if (result.contains("[MSG]")){
                        String[] splitted = result.trim().split("<>");
                        Message m = new Message(splitted[0].replace("[MSG]", ""), splitted[1]);
                        messages.add(m);
                        runOnUiThread(() -> {
                            adapter.notifyDataSetChanged();
                        });
                    } else if(result.contains("[RQT]")){
                        //[RQT]client_socket_id<>username<>room_id
                        result = result.replace("[RQT]", "");
                        String[] splitted = result.trim().split("<>");//0:Client_Socket_id, 1:Username, 2:Room_id
                        runOnUiThread(() -> {
                            new MaterialAlertDialogBuilder(this)
                                    .setTitle("Richiesta di entrata")
                                    .setMessage("Vuoi fare entrare " + splitted[1]+"nella stanza?")
                                    .setPositiveButton("Sì", (dialogInterface, i) -> {
                                        Server.getInstance().write("[ACC]"+splitted[0]+"<>"+splitted[2]);
                                    })
                                    .setNegativeButton("No", (dialogInterface, i) -> {
                                        Server.getInstance().write("[NAC]"+splitted[0]);
                                    })
                                    .show();
                        });
                    }
                }
            }).start();
        });
    }
}