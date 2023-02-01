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

public class MainActivity extends AppCompatActivity {

    private Socket socket = null;
    private PrintWriter out = null;
    private BufferedReader input = null;

    private static final String TAG = "MainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
        StrictMode.setThreadPolicy(policy);

        try{
            Thread thread = new Thread(() -> {
                socket = new Socket();
                try {
                    socket.connect(new InetSocketAddress("127.0.0.1", 9192), 15000); //Timeout 5 sec for to avoid stuck
                } catch (IOException e) {
                    e.printStackTrace();
                }
                System.out.println("Connected");
            });

            thread.run();
            thread.join();

            input = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            Log.d(TAG, "Input created");
            out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(socket.getOutputStream())), true);
            Log.d(TAG, "Out created");
        } catch (Exception e){
            Log.e(TAG, "Socket error " + e.toString());
        }

        Button button = findViewById(R.id.button);
        EditText username = findViewById(R.id.editTextTextPersonName);
        EditText passwordText = findViewById(R.id.editTextTextPassword);
        button.setOnClickListener(view -> {
            String name = username.getText().toString();
            String password = passwordText.getText().toString();
            String message = "[LGN]" + name + "<>" + password;
            Log.d(TAG, message);
            out.println(message);
            String recevingString = "";
            try {
                recevingString = input.readLine();
                Log.d(TAG, recevingString);
                Toast.makeText(getApplicationContext(), recevingString, Toast.LENGTH_LONG);
            } catch (IOException e) {
                e.printStackTrace();
            }
        });
    }




}