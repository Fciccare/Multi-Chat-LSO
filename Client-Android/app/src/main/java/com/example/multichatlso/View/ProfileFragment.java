package com.example.multichatlso.View;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.example.multichatlso.Model.Server;
import com.example.multichatlso.R;
import com.google.android.material.floatingactionbutton.ExtendedFloatingActionButton;
import com.google.android.material.floatingactionbutton.FloatingActionButton;

import java.util.Objects;

import es.dmoral.toasty.Toasty;


public class ProfileFragment extends Fragment {

    private static ProfileFragment profileFragment = null;
    private static final String TAG = "ProfileFragment";
    public static String username;

    private TextView txtUsername;

    private ExtendedFloatingActionButton logoutButton;

    private ProfileFragment() {/*Required empty public constructor*/}

    public static ProfileFragment getInstance(){
        if(profileFragment == null)
            profileFragment = new ProfileFragment();
        return profileFragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_profile, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        logoutButton = view.findViewById(R.id.buttonLogOut);
        txtUsername = view.findViewById(R.id.profileText);

        txtUsername.setText(username);

        logoutButton.setOnClickListener(view12 -> {
            Log.d(TAG, "Socket close");
            Server.getInstance().stopServer();
            requireActivity().finish();
        });
    }

}