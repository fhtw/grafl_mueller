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
    @Override
    public boolean acceptRequest(String requestUrl){
        return requestUrl.equals("/") || requestUrl.equals("static");
    }
    @Override
    public void runPlugin(Socket socket, UrlClass url){

        String filePath;
        filePath = url.getFullPath();
        if(url.getPluginPath().equals("/")){
             this.returnPluginPage(socket);
        }
        File requestedFile = new File("res/" + filePath);
        if(! requestedFile.exists()){
            new HttpResponse(socket, 404, url.getFullPath());
            return;
        }
        if(requestedFile.isDirectory()){
            this.respondDirContent(socket, filePath);
        }
        try(BufferedWriter out = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()))) {
            try(FileInputStream fileReader = new FileInputStream(requestedFile)){
                out.write("HTTP/1.1 200 OK\r\n");
                out.write("Content-Type: " + Files.probeContentType(requestedFile.toPath()) + "\r\n");
                out.write("Content-Length: " + requestedFile.length() + "\r\n");
                out.write("Connection: close \r\n\r\n");
                byte[] b = new byte[fileReader.available()];
                fileReader.read(b);
                out.flush();
                socket.getOutputStream().write(b);

                System.out.println("Sent 200 OK to " + socket.getRemoteSocketAddress().toString());

            }
        }
        catch (FileNotFoundException e){
            new HttpResponse(socket, 404, url.getFullPath());
        }
        catch(IOException | NullPointerException  e) {
            e.printStackTrace();
            new HttpResponse(socket, 500, "bla");
        }

    }

    private void respondDirContent(Socket socket, String dirPath){
        File dir = new File("res/" + dirPath);
        String[] dirContent = dir.list();
        try(BufferedWriter out = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()))) {
                int byteCount = 0;
                out.write("HTTP/1.1 200 OK\r\n");
                out.write("Content-Type: text/html\r\n");
                out.write("Connection: close \r\n\r\n");
                out.write("<ul>");
                for(String file: dirContent){
                    if(! file.equals("..") || ! file.equals(".")){
                        out.write("<li><a href=\"" + dirPath + "/" + file + "\"/>" + file + "</li>");
                    }
                }
                out.write("</ul>");
                System.out.println("Sent 200 OK to " + socket.getRemoteSocketAddress().toString());
                out.flush();
        }
        catch(IOException | NullPointerException  e) {
            e.printStackTrace();
            new HttpResponse(socket, 500, "bla");
        }

    }

    @Override
    public void returnPluginPage(Socket socket) {
        String filePath = "static/index.html";
        File requestedFile = new File("res/" + filePath);
        try(BufferedWriter out = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()))) {
            try(FileInputStream fileReader = new FileInputStream(requestedFile)){
                out.write("HTTP/1.1 200 OK\r\n");
                out.write("Content-Type: " + Files.probeContentType(requestedFile.toPath()) + "\r\n");
                out.write("Content-Length: " + requestedFile.length() + "\r\n");
                out.write("Connection: close \r\n\r\n");
                byte[] b = new byte[fileReader.available()];
                fileReader.read(b);
                out.flush();
                socket.getOutputStream().write(b);

                System.out.println("Sent 200 OK to " + socket.getRemoteSocketAddress().toString());

            }
        }
        catch(IOException | NullPointerException  e) {
            e.printStackTrace();
            new HttpResponse(socket, 500, "bla");
        }

    }
}