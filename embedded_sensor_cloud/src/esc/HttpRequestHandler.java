package esc;

import java.io.*;
import java.net.Socket;

/**
 * @author Alex
 */
public class HttpRequestHandler implements Runnable{
    Socket socket;
    HttpRequestHandler(Socket socket) {
        this.socket = socket;
    }

    public void run(){
        System.out.println("Connected: " + socket.getRemoteSocketAddress().toString());
        processRequest();
    }

    private void processRequest() {
        String requestHeaderLine;
        try(BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()))){
            String line;
            requestHeaderLine = in.readLine();
            System.out.println(requestHeaderLine);
            processRequestHeader(requestHeaderLine);
            /*while((line = in.readLine()).length() != 0) {
                System.out.println(line);
            } */

        } catch (IOException | NullPointerException e) {
                e.printStackTrace();
        }
    }

    private void processRequestHeader(String headLine) {
        try {
            String[] splitHeadLine = headLine.split(" ");
            if(splitHeadLine.length > 2) {
                if(splitHeadLine[2].startsWith("HTTP")) {
                    HttpRequest request = new HttpRequest(splitHeadLine, this.socket);
                    request.respondIndex();
                    return;
                }
            }
            throw new Exception("Invalid HTTP request headline!");
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }
}
