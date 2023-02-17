package com.example.multichatlso.View;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.FragmentManager;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.MenuItem;

import com.example.multichatlso.R;
import com.google.android.material.bottomnavigation.BottomNavigationView;
import com.google.android.material.dialog.MaterialAlertDialogBuilder;
import com.google.android.material.navigation.NavigationBarView;

public class BottomNavigationActivity extends AppCompatActivity {

    private BottomNavigationView bottomNavigationView;
    private FragmentManager fragmentManager = getSupportFragmentManager();

    private static final String TAG = "BottomNavigationActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bottom_navigation);

        bottomNavigationView = findViewById(R.id.bottom_navigation_view);

        //fragmentManager.beginTransaction().add(R.id.frameLayout, ProfileFragment.getInstance(), "2").hide(ProfileFragment.getInstance()).commit();
        fragmentManager.beginTransaction().add(R.id.frameLayout, HomepageFragment.getInstance(), "1").commit();

        bottomNavigationView.setOnItemSelectedListener(item -> {
            switch (item.getItemId()){
                case R.id.home_menu:
                    Log.d(TAG, "Click home");
                    //fragmentManager.beginTransaction().hide(HomepageFragment.getInstance()).show(HomepageFragment.getInstance()).commit();
                break;

                case R.id.home_profile:
                    Log.d(TAG, "Click profile");
                break;

            }

            return true;
        });

    }

    @Override
    public void onBackPressed() {
        Log.d(TAG, "Tasto Back premuto");
        new MaterialAlertDialogBuilder(this)
                .setTitle("Uscire dall'App?")
                .setMessage("Vuoi uscire dall'App?")
                .setPositiveButton("Sì", (dialogInterface, i) -> {
                    Intent homeIntent = new Intent(Intent.ACTION_MAIN);
                    homeIntent.addCategory( Intent.CATEGORY_HOME );
                    homeIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                    startActivity(homeIntent);
                })
                .setNegativeButton("No", (dialogInterface, i) -> {})
                .show();
    }
}