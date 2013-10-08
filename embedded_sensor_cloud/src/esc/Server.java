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

        final int SERVER_PORT = 8080;
        try(ServerSocket listener = new ServerSocket(SERVER_PORT)){
            System.out.println("Waiting for connections...");
            while(true){
                Socket sock = listener.accept();
                Thread t = new Thread(new HttpRequestHandler(sock));
                t.start();
            }
        }
        catch(IOException ex){
            ex.printStackTrace();
        }
    }
}

