package esc;

import java.io.*;
import java.net.Socket;

/**
 * @author Alex
 */
public class ConnectionHandler implements Runnable{
    private Socket _socket;
    ConnectionHandler(Socket socket) {
        this._socket = socket;
    }

    public void run(){
        System.out.println("Connected: " + _socket.getRemoteSocketAddress().toString());
        processRequest();
    }

    private void processRequest() {
        String requestHeaderLine;
        try(BufferedReader in = new BufferedReader(new InputStreamReader(_socket.getInputStream()))){
            //holt sich erste zeile
            requestHeaderLine = in.readLine();
            System.out.println(requestHeaderLine);
            //ruft funktion auf die die erste zeile verarbeitet
            String[] requestStrings = processRequestHeader(requestHeaderLine);

            if(requestStrings != null){
                HttpRequest request = new HttpRequest(requestStrings, this._socket);
                String line;
                while(!(line = in.readLine()).equals("")){
                    request.addLine(line);
                }
                if(request.getProtocol().equals("POST")){
                    request.addLine(in.readLine());
                }
                request.processRequest();
            }


        } catch (IOException | NullPointerException e) {
                e.printStackTrace();
                new HttpResponse(_socket, 500, "bla");
        }
    }

    public String[] processRequestHeader(String headLine) {
        try {
            if(headLine != null){
                //not null -> aufteilen
                String[] splitHeadLine = headLine.split(" ");
                if(splitHeadLine.length > 2) {
                    //hat 3 teile
                    if(splitHeadLine[2].startsWith("HTTP")) {
                        return splitHeadLine;
                    }
                }
            }
            System.out.println("Invalid HTTP request headline: " + headLine);
            return null;
        }
        catch (Exception e) {
            e.printStackTrace();
            new HttpResponse(_socket, 500,"bla");
            return null;
        }
    }
}
