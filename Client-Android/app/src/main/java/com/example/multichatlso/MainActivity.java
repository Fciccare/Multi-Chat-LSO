package com.example.multichatlso;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import com.example.multichatlso.Model.Server;
import com.example.multichatlso.View.LoginActivity;
import com.example.multichatlso.View.RoomActivity;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.activity_main);

        Server.getInstance();//Init server
        Log.d(TAG, "Server actived, switch to Login activity");
        startActivity(new Intent(this, LoginActivity.class));
    }

  /* @Override
    protected void onDestroy() {
        Log.d(TAG, "MainActivity destroying, send EXT");
        Server.getInstance().write("[EXT]"+ RoomActivity.getRoomId());
        super.onDestroy();
    }*/
}

