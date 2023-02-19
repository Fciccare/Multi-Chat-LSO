package com.example.multichatlso.View;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;

import com.example.multichatlso.Model.Room;
import com.example.multichatlso.R;

import es.dmoral.toasty.Toasty;

public class RoomActivity extends AppCompatActivity {

    private static final String TAG = "RoomActivity";

    private Room room;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_room);

        Bundle bundle = getIntent().getExtras();
        if (bundle != null) {
            room = (Room) bundle.getSerializable("Room");
            Log.d(TAG, "Ricevuta Room: " + room.toString());
        }

        Toasty.success(getBaseContext(), room.toString()).show();


    }
}