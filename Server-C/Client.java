import java.net.*;
import java.util.ArrayList;
import java.util.Scanner;
import java.io.*;



public class Client{

    private ArrayList<Room> rooms;
    private int MAX_CLIENT = 0;

    private Scanner scanner = null;
    private Socket socket = null;
    private PrintWriter out = null;
    private BufferedReader input = null;

    public Client(String address, int port){
       try{
            socket = new Socket();
            socket.connect(new InetSocketAddress(address, port), 5000); //Timeout 5 sec for to avoid stuck
            System.out.println("Connected");

            scanner = new Scanner(System.in);

            input = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(socket.getOutputStream())), true);
        } catch (Exception e){
            System.out.println(e);
            System.exit(1);
        }
    }

    public boolean register(String username, String password){
        String message = "[RGT]" + username + "<>" + password;
        String recevString = null;
        System.out.println("\nSending: " + message);
        out.println(message);
        System.out.flush();
    
        try {
            recevString = input.readLine();
        } catch (IOException e) {
            e.printStackTrace();
        }
        
        if(recevString.isEmpty()){
            return false;
        }

        if(recevString.contains("Register successful")){
            return true;
        }

        return false;
    }

    private boolean login(String username, String password) {
        String message = "[LGN]" + username + "<>" + password;
        String recevString = null;
        System.out.println("\nSending: " + message);
        out.println(message);
        System.out.flush();

        try {
            recevString = input.readLine();
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (recevString.isEmpty()) {
            return false;
        }

        if (recevString.contains("Login successful")) {
            return true;
        }

        return false;
    }

    public boolean accessMenu(){
        System.out.print("1)Register\n2)Login\n3)Exit\n>");
        Integer choise = scanner.nextInt();
        scanner.nextLine();

        switch (choise) {
            case 1:{ //Register
                String username = getDataFromStdin("username");
                String password = getDataFromStdin("password");
                if (register(username, password)) {
                    System.out.println("Registrato con successo");
                    return accessMenu();
                } else {
                    System.out.println("Errore nella registrazione");
                    return false;
                }
            } 
            case 2:{ //Login
                String username = getDataFromStdin("username");
                String password = getDataFromStdin("password");
                if (login(username, password)) {
                    System.out.println("Login con successo");
                    return true;
                } else {
                    System.out.println("Errore nel login");
                    return false;
                }
            }   
            default:
                System.exit(0);
                
        }
        return false;
    }

  
    public String getDataFromStdin(String info){
        System.out.print("\nInserisci " + info + ": ");
        String data = null;
        while(scanner.hasNext()){
            if(scanner.hasNextLine()){
                data = scanner.nextLine();
                break;
            }
        }
        return data;
    }
   
    
    private void chatRoom() {
        new Thread(() -> {
            recvMessage();
        }).start();
        
        while(true){
            String data = null;
            while (scanner.hasNext()) {
                if (scanner.hasNextLine()) {
                    data = scanner.nextLine();
                    break;
                }
            }
            out.println("[MSG]"+data);
            System.out.flush();
        }
    }

    private void recvMessage(){
        while(true){
            try {
                String recevString = input.readLine();
                if(recevString == null){
                    System.out.println("Server disconnected");
                    System.exit(0);
                }
                System.out.println(">>>" + recevString);
                System.out.flush();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        
    }

    private boolean roomMenu() {
        System.out.print("1)Create Room\n2)Join Room\n3)Exit\n>");
        Integer choise = scanner.nextInt();
        scanner.nextLine();

        switch (choise) {
            case 1: { // Create Room
                String chatRoomName = getDataFromStdin("nome della stanza");
                out.println("[CRT]"+chatRoomName);
                System.out.flush();
                String recevString = null;
                try {
                    recevString = input.readLine();
                } catch (IOException e) {
                    e.printStackTrace();
                }

                if (recevString.isEmpty()) {
                    System.out.println("Room create failed");
                    return false;
                }

                if (recevString.contains("Room create successful")) {
                    System.out.println("Room create successful\n");
                    return true;
                }
                System.out.println("Room create failed");
                return false;
            }
            case 2: { 
                out.println("[LST]");
                String recevString = "";
                while(!recevString.equals("[/LST]")){
                    try {
                        recevString = input.readLine();
                        if(recevString.contains("[LST]")){
                            recevString = recevString.replace("[LST]", "");
                            int max_client = Integer.parseInt(recevString.trim());
                            rooms = new ArrayList<Room>();
                            MAX_CLIENT = max_client;
                        }else if(recevString.contains("<>")){
                            String[] splitted = recevString.split("<>");
                            int id = Integer.parseInt(splitted[0].trim());
                            int online_client = Integer.parseInt(splitted[2]);
                            rooms.add(new Room(splitted[1], MAX_CLIENT, online_client, id));
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
                System.out.println(rooms.toString());
                return false;
            }
            default:
                System.exit(0);

        }
        return true;
    }

    public static void main(String argv[]){
        Client c = new Client("127.0.0.1", 9192);
        boolean status = false;
        while(status == false){
            status = c.accessMenu();
        }
        
        boolean status1 = false;
        while (status1 == false) {
            status1 = c.roomMenu();
        }
        
        c.chatRoom();

        try{
            c.scanner.close();
            c.input.close();
            c.out.close();
            c.socket.close();
        }catch (Exception e){
            e.printStackTrace();
        } 
    }

}

class Room {
    String name = "";
    int max_client = 0;
    int online_client = 0;
    int id = 0;


    public Room(String name, int max_client, int online_client, int id) {
        this.name = name;
        this.max_client = max_client;
        this.online_client = online_client;
        this.id = id;
    }


    public String getName() {
        return this.name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getMax_client() {
        return this.max_client;
    }

    public void setMax_client(int max_client) {
        this.max_client = max_client;
    }

    public int getOnline_client() {
        return this.online_client;
    }

    public void setOnline_client(int online_client) {
        this.online_client = online_client;
    }

    public int getId() {
        return this.id;
    }

    public void setId(int id) {
        this.id = id;
    }


    @Override
    public String toString() {
        return "{" +
            " name='" + getName() + "'" +
            ", max_client='" + getMax_client() + "'" +
            ", online_client='" + getOnline_client() + "'" +
            ", id='" + getId() + "'" +
            "}";
    }

    
}
