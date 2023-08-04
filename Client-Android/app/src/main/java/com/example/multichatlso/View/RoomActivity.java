package com.example.multichatlso.View;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.content.DialogInterface;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.util.Log;
import android.widget.AutoCompleteTextView;
import android.widget.Button;

import com.example.multichatlso.Model.Message;
import com.example.multichatlso.Model.RecyclerMessageAdapter;
import com.example.multichatlso.Model.Room;
import com.example.multichatlso.Model.Server;
import com.example.multichatlso.R;
import com.google.android.material.dialog.MaterialAlertDialogBuilder;

import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import es.dmoral.toasty.Toasty;
import taimoor.sultani.sweetalert2.Sweetalert;

public class RoomActivity extends AppCompatActivity {

    private static final String TAG = "RoomActivity";

    private static Room room;
    private ArrayList<Message> messages;

    private AutoCompleteTextView textView;
    private Button button;
    private RecyclerView recyclerView;
    private RecyclerMessageAdapter adapter;

    private ExecutorService executor;
    private Thread t;

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
        if(message.trim().isEmpty()){
            Toasty.error(getBaseContext(), "Inserisci un messaggio").show();
            return;
        }
        message = "[MSG]" + message.trim() + "<>" + room.getId();
        Server.getInstance().write(message);
        Log.d(TAG, "Send to server: " + message);
        textView.setText("");
    }

    private void fetchMessage(){
        executor = Executors.newSingleThreadExecutor();
        //Handler h = new Handler(Looper.getMainLooper());
        executor.execute(() -> {
            t = new Thread(() -> {
                while(true){
                    String result = "";
                    result = Server.getInstance().read();
                    Log.d(TAG, "Server sended: " + result);
                    if (result.contains("[MSG]")){
                        String[] splitted = result.trim().split("<>");
                        Message m = new Message(splitted[0].replace("[MSG]", ""), Integer.parseInt(splitted[1]), splitted[2]);
                        messages.add(m);
                        runOnUiThread(() -> {
                            adapter.notifyDataSetChanged();
                            recyclerView.smoothScrollToPosition(adapter.getItemCount() - 1);
                        });
                    } else if(result.contains("[RQT]")){
                        //[RQT]client_socket_id<>username<>room_id
                        result = result.replace("[RQT]", "");
                        String[] splitted = result.trim().split("<>");//0:Client_to_enter_Socket_id, 1:Username, 2:Room_id
                        runOnUiThread(() -> {
                            Sweetalert dialog = new Sweetalert(this, Sweetalert.WARNING_TYPE)
                                    .setTitleText("Richiesta di entrata")
                                    .setContentText("Vuoi fare entrare " + splitted[1]+" nella stanza?")
                                    .setCancelButton("No", sweetAlertDialog -> {
                                        Log.d(TAG, "Server sending: " + "[NAC]"+splitted[0]);
                                        Server.getInstance().write("[NAC]"+splitted[0]);
                                        sweetAlertDialog.dismissWithAnimation();
                                    })
                                    .setNeutralButton("Sì", sweetAlertDialog -> {
                                        Log.d(TAG, "Server sending: " + "[ACC]"+splitted[0]);
                                        Server.getInstance().write("[ACC]"+splitted[0]+"<>"+splitted[2]);
                                        sweetAlertDialog.dismissWithAnimation();
                                    }).setNeutralButtonBackgroundColor("green");
                            dialog.setCancelable(false);
                            dialog.show();
                            new CountDownTimer(10000, 10000){

                                @Override
                                public void onTick(long l) {

                                }

                                @Override
                                public void onFinish() {
                                    Log.d(TAG, "Finish 10 sec");
                                    dialog.dismissWithAnimation();
                                }
                            }.start();
                        });

                    }
                }
            });
            t.start();
        });
    }



    @Override
    public void onBackPressed() {
        Log.d(TAG, "Tasto Back premuto");
        new MaterialAlertDialogBuilder(this)
                .setTitle("Uscire dalla stanza?")
                .setMessage("Vuoi uscire dalla stanza? \nPer rientrare dovrai fare di nuovo richiesta al master")
                .setPositiveButton("Sì", (dialogInterface, i) -> {
                    Server.getInstance().write("[EXT]"+ room.getId());
                    t.interrupt();
                    executor.shutdown();
                    super.onBackPressed();
                    finish();
                })
                .setNegativeButton("No", (dialogInterface, i) -> {})
                .show();
    }

    public static int getRoomId() {
        return room.getId();
    }
}
