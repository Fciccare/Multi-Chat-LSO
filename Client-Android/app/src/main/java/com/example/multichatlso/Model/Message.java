package com.example.multichatlso.Model;

import java.io.Serializable;

public class Message implements Serializable {

    private String text;
    private int user_id;

    public Message(String text, int user_id) {
        this.text = text;
        this.user_id = user_id;
    }

    public String getText() {
        return text;
    }

    public void setText(String text) {
        this.text = text;
    }

    public int getUser_id() {
        return user_id;
    }

    public void setUser_id(int user_id) {
        this.user_id = user_id;
    }
}
