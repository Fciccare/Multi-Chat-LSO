import java.net.*;
import java.util.Scanner;
import java.io.*;

public class Client{

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
            case 1:{
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
            case 2:{
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


    public static void main(String argv[]){
        Client c = new Client("127.0.0.1", 9192);
        boolean status = false;
        while(status == false){
            status = c.accessMenu();
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
