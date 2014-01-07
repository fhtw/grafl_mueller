package esc;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.HashMap;

/**
 * @author Alex
 */
public class Server {
    /**
     * @param args
     *            the command line arguments
     */

    /*
    *   Singelton verwenden um cache zu implmenetieren
    *   Eigenes Objekt
    *   Update auch dadrin
    *   Plugins nicht neu laden
    * */
    public static HashMap<String, String> naviEntries = new HashMap<>();
    public static PluginManager pluginManager = new PluginManager();

    public static void main(String[] args) {

        final int SERVER_PORT = 8080; // port
        // Thread um kontinuierlich in die DB zu inserten
        Thread insertThread = new Thread(new InsertRunnable());
        //insertThread.start();
        // erstellt listener und lauscht
        try(ServerSocket listener = new ServerSocket(SERVER_PORT)){
            System.out.println("Waiting for connections...");
            while(true){
                //akzeptiert verbindungen
                Socket sock = listener.accept();
                //neuer ConnectionHandler erstellen und in neuem Thread starten
                Thread t = new Thread(new ConnectionHandler(sock));
                t.start();
            }
        }
        catch(IOException ex){
            ex.printStackTrace();
        }
    }
}

