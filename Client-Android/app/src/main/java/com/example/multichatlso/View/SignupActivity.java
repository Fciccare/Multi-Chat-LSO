package com.example.multichatlso.View;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.lifecycle.Observer;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.MenuItem;
import android.widget.EditText;
import android.widget.Toast;

import com.example.multichatlso.R;
import com.example.multichatlso.Model.Server;
import com.google.android.material.appbar.MaterialToolbar;
import com.google.android.material.floatingactionbutton.ExtendedFloatingActionButton;

import java.util.zip.CRC32;

import es.dmoral.toasty.Toasty;

public class SignupActivity extends AppCompatActivity {

    private static final String TAG = "SignupActivity";

    private EditText password, confirmPassword, username;
    private MaterialToolbar toolbar;
    ExtendedFloatingActionButton signup;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_signup);

        username = findViewById(R.id.editTextTextUsername);
        password = findViewById(R.id.editTextTextPassword1);
        confirmPassword = findViewById(R.id.editTextTextPassword2);
        signup = findViewById(R.id.buttonRegister);
        toolbar = findViewById(R.id.MaterialToolbar2);

        setSupportActionBar(toolbar);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setDisplayShowHomeEnabled(true);
        getSupportActionBar().setTitle("Registrazione");

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

        CRC32 crc32 = new CRC32();
        crc32.update(txtPassword1.getBytes());

        String message = "[RGT]" + txtUsername + "<>" + Long.toHexString(crc32.getValue()).toUpperCase();
        Log.d(TAG, "Sending: " + message);
        Server.getInstance().write(message);
        Server.getInstance().read();

        Server.getInstance().getListen().singleObserve(this, receivingString -> {
            Log.d(TAG, "Observer started");
            if(receivingString == null){
                Log.e(TAG, "Socket read null");
                Toasty.error(getApplicationContext(), "Errore, riprova").show();
                return;
            }
            Log.d(TAG, "Server: " + receivingString);
            if(receivingString.contains("Register successful")){
                startActivity(new Intent(this, LoginActivity.class));
                finish();
            }else {
                Toasty.error(getApplicationContext(), "Registrazione fallito", Toasty.LENGTH_SHORT, true).show();
            }
            Server.getInstance().getListen().removeObservers(this);
        });
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            onBackPressed(); // Chiama il metodo di default per tornare indietro
            return true;
        }
        //}else if(item.getItemId() == R.id.listButton){
        //    Toasty.normal(this, "Lista persone").show();
        //    return true;
        //}
        return super.onOptionsItemSelected(item);
    }
}