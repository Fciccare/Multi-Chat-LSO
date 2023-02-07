package com.example.multichatlso;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.StrictMode;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.net.Socket;

import es.dmoral.toasty.Toasty;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Server.getInstance();//Init server

        Button button = findViewById(R.id.button);
        EditText username = findViewById(R.id.editTextTextPersonName);
        EditText passwordText = findViewById(R.id.editTextTextPassword);
        button.setOnClickListener(view -> {
            if(username.getText().toString().isEmpty() || passwordText.getText().toString().isEmpty())
                Toasty.error(getApplicationContext(), "Riempi i campi", Toasty.LENGTH_LONG, true).show();
            else{
                String name = username.getText().toString();
                String password = passwordText.getText().toString();
                String message = "[LGN]" + name + "<>" + password;
                Log.d(TAG, message);
                Server.getInstance().write(message);
                String recevingString = Server.getInstance().read();
                Log.d(TAG, recevingString);
                if(recevingString.contains("Login successful"))
                    Toasty.success(getApplicationContext(), "Loggato con successo", Toast.LENGTH_LONG, true).show();
                else
                    Toasty.error(getApplicationContext(), "Login errato", Toast.LENGTH_LONG, true).show();

            }

        });
    }




}