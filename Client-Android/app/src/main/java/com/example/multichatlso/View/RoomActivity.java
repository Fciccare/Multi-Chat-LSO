package com.example.multichatlso.View;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.os.Bundle;
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

import java.util.ArrayList;
import java.util.List;

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
        new Thread(() -> {
            while(true){
                String result = Server.getInstance().read();
                Log.d(TAG, "Server sended: " + result);
                if (result.contains("[MSG]")){
                    String[] splitted = result.trim().split("<>");
                    Message m = new Message(splitted[0].replace("[MSG]", ""), splitted[1]);
                    messages.add(m);
                    adapter.notifyDataSetChanged();
                }
            }
        }).start();
    }
}