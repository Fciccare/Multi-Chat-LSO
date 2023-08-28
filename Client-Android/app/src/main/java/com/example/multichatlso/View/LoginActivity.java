package com.example.multichatlso.View;

import androidx.appcompat.app.AppCompatActivity;
import androidx.lifecycle.Observer;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.EditText;
import android.widget.Toast;

import com.example.multichatlso.Model.RecyclerMessageAdapter;
import com.example.multichatlso.R;
import com.example.multichatlso.Model.Server;
import com.google.android.material.dialog.MaterialAlertDialogBuilder;
import com.google.android.material.floatingactionbutton.ExtendedFloatingActionButton;

import java.util.zip.CRC32;

import es.dmoral.toasty.Toasty;

public class LoginActivity extends AppCompatActivity {

    private static final String TAG = "LoginActivity";

    private EditText passwordText, username;
    private ExtendedFloatingActionButton signIn, signUp;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        signIn = findViewById(R.id.buttonLogin);
        signUp = findViewById(R.id.buttonSignUp);
        username = findViewById(R.id.editTextTextPersonName);
        passwordText = findViewById(R.id.editTextTextPassword);
        signIn.setOnClickListener(view -> {
            login();
        });

        signUp.setOnClickListener(view -> {
            Log.d(TAG, "Switch to SignUpActivity");
            startActivity(new Intent(this, SignupActivity.class));
        });

    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.d(TAG, "Resume socket");
        Server.getInstance();
    }

    /*public void login(){
        if(username.getText().toString().isEmpty() || passwordText.getText().toString().isEmpty()){
            Toasty.error(getApplicationContext(), "Riempi i campi", Toast.LENGTH_SHORT, true).show();
            return;
        }

        String name = username.getText().toString();
        String password = passwordText.getText().toString();
        String message = "[LGN]" + name + "<>" + password;
        Log.d(TAG, message);
        Server.getInstance().write(message);
        String receivingString = Server.getInstance().blockingRead();
        if(receivingString == null || receivingString.isEmpty()){
            Log.e(TAG, "Socket read null");
            Toasty.error(getApplicationContext(), "Errore, riprova").show();
            return;
        }
        Log.d(TAG, receivingString);
        String user_id = receivingString.trim().split("<>")[1];
        RecyclerMessageAdapter.user_id=Integer.parseInt(user_id);
        if(receivingString != null && receivingString.contains("Login successful"))
            startActivity(new Intent(this, BottomNavigationActivity.class));
        else{
            Log.d(TAG, "Server responde with: " + receivingString);
            Toasty.error(getApplicationContext(), "Login errato", Toast.LENGTH_SHORT, true).show();
        }
    }*/

    public void login(){
        if(username.getText().toString().isEmpty() || passwordText.getText().toString().isEmpty()){
            Toasty.error(getApplicationContext(), "Riempi i campi", Toast.LENGTH_SHORT, true).show();
            return;
        }
        String name = username.getText().toString();
        String password = passwordText.getText().toString();

        CRC32 crc32 = new CRC32();
        crc32.update(password.getBytes());

        String message = "[LGN]" + name + "<>" + Long.toHexString(crc32.getValue()).toUpperCase();
        Log.d(TAG, message);
        Server.getInstance().write(message);
        Server.getInstance().read();

        Server.getInstance().getListen().singleObserve(this, receivingString -> {//TODO: SPLIT FUNCTION
            Log.d(TAG, "Observer started");
            if(receivingString == null || receivingString.isEmpty()){
                Log.e(TAG, "Socket read null");
                Toasty.error(getApplicationContext(), "Errore, riprova").show();
                return;
            }
            Log.d(TAG, "Observed:" + receivingString);
            if(receivingString.contains("Login successful")){
                String user_id = receivingString.trim().split("<>")[1];
                RecyclerMessageAdapter.user_id=Integer.parseInt(user_id);
                startActivity(new Intent(this, BottomNavigationActivity.class));
                //finish();
            } else{
                Toasty.error(getApplicationContext(), "Login errato", Toast.LENGTH_SHORT, true).show();
            }
            Server.getInstance().getListen().removeObservers(this);
        });
    }

    @Override
    public void onBackPressed() {
        Log.d(TAG, "Tasto Back premuto");
        new MaterialAlertDialogBuilder(this)
                .setTitle("Uscire dall'App?")
                .setMessage("Vuoi uscire dall'App?")
                .setPositiveButton("SÌ", (dialogInterface, i) -> {
                    Intent homeIntent = new Intent(Intent.ACTION_MAIN);
                    homeIntent.addCategory( Intent.CATEGORY_HOME );
                    homeIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                    startActivity(homeIntent);
                })
                .setNegativeButton("NO", (dialogInterface, i) -> {})
                .show();
    }

}