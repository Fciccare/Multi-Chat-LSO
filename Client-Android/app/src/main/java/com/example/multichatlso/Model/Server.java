package com.example.multichatlso.Model;

import android.os.StrictMode;
import android.util.Log;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.net.Socket;

public class Server {

    private static final String TAG = "Server";

    private static Server instance = null;

    private Socket socket = null;
    private PrintWriter out = null;
    private BufferedReader input = null;

    public static Server getInstance(){
        if(instance == null)
            instance = new Server();
        return instance;
    }

    private Server() {
        StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
        StrictMode.setThreadPolicy(policy);

        new Thread(() -> {
            socket = new Socket();
            try {
                socket.connect(new InetSocketAddress("natour.ddns.net", 9192), 15000); //Timeout 15 sec for to avoid stuck
                Log.d(TAG, "Socket connected");
            } catch (IOException e) {
                Log.e(TAG, e.toString());
            }
        }).start();
    }

    private boolean initRead(){
        if(socket.isConnected() && input == null){
            try {
                input = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                Log.d(TAG, "Input created");
                return true;
            } catch (IOException e) {
                Log.e(TAG, e.toString());
            }
        }else if(input != null)
            return true;
        else Log.e(TAG, "Can't create in/out beacause socket not connected or is alread created");
        return false;
    }

    private boolean initWrite(){
        if(socket.isConnected() && out == null){
            try {
                out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(socket.getOutputStream())), true);
                Log.d(TAG, "Out created");
                return true;
            } catch (IOException e) {
                Log.e(TAG, e.toString());
            }
        }else if(out != null)
            return true;
        else Log.e(TAG, "Can't create in/out beacause socket not connected or is alread created");
        return false;
    }

    public void write(String message){
        new Thread(() -> {
            if(out == null)
                initWrite();
            out.println(message);
        }).start();
    }

    public String read(){
        String message = "";
        if(input == null)
            initRead();
            //if(!initRead())
                //return null;
        try {
            message = input.readLine();
            if(message == null)
                Log.e(TAG, "Server disconnected");
        } catch (IOException e) {
            Log.e(TAG, e.toString());
        }
        return message;
    }
}

