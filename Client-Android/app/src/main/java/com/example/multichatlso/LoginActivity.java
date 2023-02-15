package com.example.multichatlso;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import com.google.android.material.floatingactionbutton.ExtendedFloatingActionButton;
import com.google.android.material.textfield.TextInputEditText;

import es.dmoral.toasty.Toasty;

public class LoginActivity extends AppCompatActivity {

    private static final String TAG = "LoginActivity";

    private EditText passwordText, username;
    ExtendedFloatingActionButton signIn, signUp;

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

    public void login(){
        if(username.getText().toString().isEmpty() || passwordText.getText().toString().isEmpty()){
            Toasty.error(getApplicationContext(), "Riempi i campi", Toast.LENGTH_SHORT, true).show();
            return;
        }

        String name = username.getText().toString();
        String password = passwordText.getText().toString();
        String message = "[LGN]" + name + "<>" + password;
        Log.d(TAG, message);
        Server.getInstance().write(message);
        String recevingString = Server.getInstance().read();
        if(recevingString != null) Log.d(TAG, recevingString + String.valueOf(recevingString.length()));
        if(recevingString.contains("Login successful"))
            Toasty.success(getApplicationContext(), "Loggato con successo", Toast.LENGTH_SHORT, true).show();
        else
            Toasty.error(getApplicationContext(), "Login errato", Toast.LENGTH_SHORT, true).show();

    }
}