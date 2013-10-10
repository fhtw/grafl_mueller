package esc;

import java.io.*;
import java.net.Socket;

/**
 * @author Alex
 */
public class StaticPagePlugin implements IPlugin{
    public boolean acceptRequest(String requestUrl){
          return  requestUrl.equals("/");
}

    public void runPlugin(Socket socket){

        String line;
        try(BufferedWriter out = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()))) {
            try(BufferedReader fileReader = new BufferedReader(new FileReader("res/index.html"))){
                out.write("HTTP/1.1 200 OK\r\n");
                out.write("Content-Type: text/html\r\n\r\n");

                while((line = fileReader.readLine()).length() != 0){
                    out.write(line.toCharArray());
                }
                System.out.println("Sent 200 OK to " + socket.getRemoteSocketAddress().toString());
                out.flush();
            }
        }
        catch(IOException | NullPointerException e) {
            e.printStackTrace();
        }

    }
}