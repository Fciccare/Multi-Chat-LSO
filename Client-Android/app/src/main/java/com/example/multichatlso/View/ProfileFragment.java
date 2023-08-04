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

import com.example.multichatlso.Model.Server;
import com.example.multichatlso.R;

import es.dmoral.toasty.Toasty;


public class ProfileFragment extends Fragment {

    private static ProfileFragment profileFragment = null;
    private static final String TAG = "ProfileFragment";

    private EditText txtSocket;
    private Button button;

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

        txtSocket = view.findViewById(R.id.editTextSocket);
        button = view.findViewById(R.id.buttonSocket);

        button.setOnClickListener(view1 -> {
            Server.getInstance().write(txtSocket.getText().toString());
            Log.d(TAG, "Scrivo allo socket: " + txtSocket.getText().toString());

            String result = "";
            result = Server.getInstance().read();

            if(result.contains("Error"))
                Toasty.error(requireContext(), result).show();
            else
                Toasty.info(requireContext(), result).show();
            txtSocket.setText("");
        });
    }
}