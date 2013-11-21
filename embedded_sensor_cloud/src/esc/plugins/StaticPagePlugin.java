package esc.plugins;

import esc.HttpResponse;
import esc.IPlugin;
import esc.UrlClass;

import java.io.*;
import java.net.Socket;
import java.nio.file.Files;

/**
 * @author Alex
 */
public class StaticPagePlugin implements IPlugin {
    public boolean acceptRequest(String requestUrl){
          if(requestUrl.equals("/") ||  requestUrl.equals("static")){
              return true;
          }
        return false;
}

    public void runPlugin(Socket socket, UrlClass url){

        String line, filePath;
        filePath = url.getFullPath();
        if(url.getPluginPath() == "/"){
             filePath = "static/index.html";
        }
        File requestedFile = new File("res/" + filePath);
        if(requestedFile == null){
            new HttpResponse(socket, 404, "bla");
            return;
        }
        try(BufferedWriter out = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()))) {
            try(BufferedReader fileReader = new BufferedReader(new FileReader(requestedFile))){
                int byteCount = 0;
                out.write("HTTP/1.1 200 OK\r\n");
                out.write("Content-Type: " + Files.probeContentType(requestedFile.toPath()) + "\r\n");
                out.write("Content-Length: " + requestedFile.length() + "\r\n\r\n");
                while(byteCount < requestedFile.length()){
                    line = fileReader.readLine();
                    out.write(line.toCharArray());
                    byteCount += line.length();
                }
                System.out.println("Sent 200 OK to " + socket.getRemoteSocketAddress().toString());
                out.flush();
            }
        }
        catch (FileNotFoundException e){
            new HttpResponse(socket, 404, "bla");
        }
        catch(IOException | NullPointerException  e) {
            e.printStackTrace();
            new HttpResponse(socket, 500, "bla");
        }

    }
}