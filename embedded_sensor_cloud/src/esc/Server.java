package esc;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * @author Alex
 */
public class Server {
    /**
     * @param args
     *            the command line arguments
     */
    public static void main(String[] args) {

        final int SERVER_PORT = 8080; // port
        // erstellt listener und lauscht
        System.out.println("CWD: "+ System.getProperty("user.dir"));
        try(ServerSocket listener = new ServerSocket(SERVER_PORT)){
            System.out.println("Waiting for connections...");
            while(true){
                //akzeptiert verbindungen
                Socket sock = listener.accept();
                //neuer RequestHandler erstellen und in neuem Thread starten
                Thread t = new Thread(new HttpRequestHandler(sock));
                t.start();
            }
        }
        catch(IOException ex){
            ex.printStackTrace();
        }
    }
}

