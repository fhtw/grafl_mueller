package esc;

import java.io.*;
import java.net.Socket;

/**
 * @author Alex
 */
public class HttpRequest implements Runnable{
    Socket socket;
    HttpRequest(Socket socket) {
        this.socket = socket;
    }

    public void run(){
        System.out.println("Connected: " + socket.getInetAddress().getHostAddress());
        processRequest();
    }

    private void processRequest() {
        String requestHeaderLine;
        try(BufferedWriter out = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()))) {
            try(BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()))){
                String line;
                requestHeaderLine = in.readLine();
                //TODO: Request headerline aufsplitten und auswerten, dann plugins ansprechen

                System.out.println(requestHeaderLine);
               /* while((line = in.readLine()).length() != 0) {
                    System.out.println(line);
                }*/
                out.write("HTTP/1.1 200 OK\r\n");
                out.write("Content-Type: text/html\r\n\r\n");
                out.flush();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
