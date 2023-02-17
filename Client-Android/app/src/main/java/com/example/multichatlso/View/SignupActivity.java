package com.example.multichatlso.View;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.EditText;
import android.widget.Toast;

import com.example.multichatlso.R;
import com.example.multichatlso.Model.Server;
import com.google.android.material.floatingactionbutton.ExtendedFloatingActionButton;

import es.dmoral.toasty.Toasty;

public class SignupActivity extends AppCompatActivity {

    private static final String TAG = "SignupActivity";

    private EditText password, confirmPassword, username;
    ExtendedFloatingActionButton signup;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_signup);

        username = findViewById(R.id.editTextTextUsername);
        password = findViewById(R.id.editTextTextPassword1);
        confirmPassword = findViewById(R.id.editTextTextPassword2);
        signup = findViewById(R.id.buttonRegister);

        signup.setOnClickListener(view -> {
            signUp();
        });

    }

    private void signUp() {
        String txtUsername = username.getText().toString();
        String txtPassword1 = password.getText().toString();
        String txtPassword2 = confirmPassword.getText().toString();

        if(username.getText().toString().isEmpty() || password.getText().toString().isEmpty() || confirmPassword.getText().toString().isEmpty()){
            Toasty.error(getApplicationContext(), "Riempi i campi", Toast.LENGTH_SHORT, true).show();
            return;
        }
        if(!txtPassword1.equals(txtPassword2)){
            Toasty.error(getApplicationContext(), "Password diverse", Toast.LENGTH_SHORT, true).show();
            return;
        }

        String message = "[RGT]" + txtUsername + "<>" + txtPassword1;
        Server.getInstance().write(message);
        String receivingString = Server.getInstance().read();
        Log.d(TAG, "Server: " + receivingString);
        if(receivingString.contains("Register successful")){
            Log.d(TAG, "Register successful");
            startActivity(new Intent(this, LoginActivity.class));
        }else {
            Log.e(TAG, "Register failed");
            Toasty.error(getApplicationContext(), "Registrazione fallito", Toasty.LENGTH_SHORT, true).show();
        }

    }
}