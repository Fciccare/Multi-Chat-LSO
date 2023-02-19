package com.example.multichatlso.Model;

import java.io.Serializable;

public class Room implements Serializable {

    private int id;
    private String name;
    private int clients_counter;
    private int max_client;
    //Client* master_client;


    public Room(int id, String name, int clients_counter, int max_client) {
        this.id = id;
        this.name = name;
        this.clients_counter = clients_counter;
        this.max_client = max_client;
    }

    @Override
    public String toString() {
        return "Room{" +
                "id=" + id +
                ", name='" + name + '\'' +
                ", clients_counter=" + clients_counter +
                ", max_client=" + max_client +
                '}';
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getClients_counter() {
        return clients_counter;
    }

    public void setClients_counter(int clients_counter) {
        this.clients_counter = clients_counter;
    }

    public int getMax_client() {
        return max_client;
    }

    public void setMax_client(int max_client) {
        this.max_client = max_client;
    }
}
