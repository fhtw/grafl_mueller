package esc;

import java.io.*;
import java.net.Socket;
import java.net.URI;
import java.net.URLDecoder;


/**
 * @author Alex
 */
public class HttpRequest {
    private String protocol;
    private String path;
    private String httpVersion;
    private Socket socket;

    HttpRequest(String[] s, Socket socket) throws UnsupportedEncodingException {
        this.protocol = s[0];
        this.path = URLDecoder.decode(s[1], "UTF-8");
        this.httpVersion = s[2];
        this.socket = socket;
    }

    public void respondIndex()
    {
        String line;
        try(BufferedWriter out = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()))) {
            out.write("HTTP/1.1 200 OK\r\n");
            out.write("Content-Type: text/html\r\n\r\n");
            BufferedReader fileReader = new BufferedReader(new FileReader("res/index.html"));
            while((line = fileReader.readLine()).length() != 0){
                out.write(line.toCharArray());
            }
            System.out.println("Sent 200 OK to " + socket.getRemoteSocketAddress().toString());
            out.flush();
        }
        catch(IOException | NullPointerException e) {
            e.printStackTrace();
        }
    }
}
