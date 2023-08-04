package com.example.multichatlso.View;

import android.content.Intent;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout;

import android.os.CountDownTimer;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.example.multichatlso.Model.RecyclerRoomAdapter;
import com.example.multichatlso.Model.Room;
import com.example.multichatlso.Model.Server;
import com.example.multichatlso.R;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import es.dmoral.toasty.Toasty;
import taimoor.sultani.sweetalert2.Sweetalert;


public class HomepageFragment extends Fragment {

    private static HomepageFragment homepageFragment;
    private SwipeRefreshLayout swipeContainer;
    private ArrayList<Room> rooms;
    private RecyclerView recyclerView;
    private RecyclerRoomAdapter adapter;
    private Sweetalert pDialog;

    private Thread t = null;

    private static final String TAG = "HomepageFragment";

    private HomepageFragment() {
        // Required empty public constructor
    }

    public static HomepageFragment getInstance(){
        if(homepageFragment == null){
            Log.d(TAG, "Home fragment non presente, creazione in corso");
            homepageFragment = new HomepageFragment();
        }

        Log.d(TAG, "Invio homefragment");
        return homepageFragment;
    }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_homepage, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        //Button buttonSKT = view.findViewById(R.id.buttonSocket);
        //Button buttonLST = view.findViewById(R.id.buttonList);
        swipeContainer = view.findViewById(R.id.swipeContainer);
        //EditText txtSocket = view.findViewById(R.id.editTextSocket);


        recyclerView = view.findViewById(R.id.recyclerView);
        rooms = new ArrayList<Room>();
        adapter = new RecyclerRoomAdapter(rooms);
        RecyclerView.LayoutManager layoutManager = new LinearLayoutManager(getContext());
        //RecyclerView.LayoutManager layoutManager1 = new GridLayoutManager(getContext(), 2);
        recyclerView.setLayoutManager(layoutManager);
        recyclerView.setAdapter(adapter);

        listRoom();
        swipeContainer.setOnRefreshListener(() -> {
            listRoom();
            swipeContainer.setRefreshing(false);
        });

        recyclerView.addOnItemTouchListener(
            new RecyclerItemClickListener(getContext(), recyclerView ,new RecyclerItemClickListener.OnItemClickListener() {
                @Override public void onItemClick(View view, int position) {
                    requestToEnterRoom(position);
                }

                @Override public void onLongItemClick(View view, int position) {Toasty.info(requireContext(), "Easter egg").show();}
            })
        );
    }

    private void requestToEnterRoom(int position){
        ExecutorService executor = Executors.newSingleThreadExecutor();
        //Handler h = new Handler(Looper.getMainLooper());
        //new Handler().postDelayed(() -> { executor.shutdown(); stopLoading(); Toasty.error(requireContext(), "Tempo scaduto, riprova").show();}, 15000 );
        executor.execute(() -> {
            t  = new Thread(() -> {
                Room room = rooms.get(position);
                String message = "[RQT]" + String.valueOf(room.getId());//[RQT]2
                Server.getInstance().write(message);
                startLoading();
                requireActivity().runOnUiThread(() -> {
                    new CountDownTimer(12000, 12000){

                        @Override
                        public void onTick(long l) {}

                        @Override
                        public void onFinish() {
                            Log.d(TAG, "Timeout");
                            stopLoading();
                            t.interrupt();
                            executor.shutdown();
                        }
                    }.start();
                });
                String value = Server.getInstance().read();
                if(value == null){
                    Log.e(TAG, "Socket read null");
                    stopLoading();
                    Toasty.error(requireContext(), "Errore, riprova").show();
                    return;
                }
                Log.d(TAG, value);
                getActivity().runOnUiThread(() -> {
                    if (value.toLowerCase().contains("accept")) {
                        Intent i = new Intent(getActivity(), RoomActivity.class);
                        i.putExtra("Room", room);
                        stopLoading();
                        startActivity(i);
                    } else {
                        stopLoading();
                        Toasty.error(requireContext(), "Non sei stato accettato nella stanza").show();
                    }
                });
            });
            t.start();
        });

        /*
        new Thread(() -> {
            Room room = rooms.get(position);
            String message = "[RQT]" + String.valueOf(room.getId());//[RQT]2
            Server.getInstance().write(message);
            String value = Server.getInstance().read();

            if(value.toLowerCase().contains("accept")){
                Intent i = new Intent(getActivity(), RoomActivity.class);
                i.putExtra("Room", room);
                startActivity(i);
            }else{
                Toasty.error(requireContext(), "Non sei stato accettato nella stanza").show();
            }
        }).start();*/

    }

    private void listRoom(){
        String message = "[LST]";
        Log.d(TAG, "Write to Server: " + message);
        Server.getInstance().write(message);
        ExecutorService executor = Executors.newSingleThreadExecutor();
        //executor.execute(() -> {
            new Thread(() -> {
                String result = "";
                do {
                    String temp = Server.getInstance().read() + "\n";//TODO: ERROR HANDLING
                    Log.d(TAG, "Il server ha risposto con: " + temp);
                    result += temp;
                }while (!result.contains("[/LST]"));

                Log.d(TAG, "Lista di stanza: " + result);
                List<Room> roomArrayList = castListToRoom(result);
                rooms.clear();
                rooms.addAll(roomArrayList);
                Log.d(TAG, "Lista Room: " + rooms);
                getActivity().runOnUiThread(() -> {
                    adapter.notifyDataSetChanged();
                });
            }).start();
        //});
    }

    private List<Room> castListToRoom(String list){
        String[] lines = list.split("\n");

        List<Room> rooms = new ArrayList<Room>();
        int max_client = 0;

        for(String s: lines){
            if(s.contains("[LST]")){
                s = s.replace("[LST]", "");
                max_client = Integer.parseInt(s.trim());
            }else if(s.contains("<>")){
                String[] splitted = s.split("<>");
                int id = Integer.parseInt(splitted[0].trim());
                int online_client = Integer.parseInt(splitted[2]);
                rooms.add(new Room(id, splitted[1], online_client, max_client));
            }
        }

        return rooms;

    }

    private void startLoading(){
        getActivity().runOnUiThread(() -> {
            if(pDialog ==null)
                pDialog = new Sweetalert(requireContext(), Sweetalert.PROGRESS_TYPE);;
            pDialog.setTitleText("Aspetta che il master ti accetti nella stanza");
            pDialog.setCancelable(false);
            pDialog.show();
        });
    }

    private void stopLoading(){
        //getActivity().runOnUiThread(() -> {
            if(pDialog!=null){
                pDialog.dismissWithAnimation();
                pDialog = null;
            }

        //});
    }


}