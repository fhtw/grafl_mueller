package esc.plugins;

import com.sun.org.apache.xpath.internal.operations.Bool;
import esc.HttpResponse;
import esc.IPlugin;
import esc.UrlClass;

import java.io.*;
import java.net.Socket;
import java.util.ArrayList;

/**
 * @author Alex
 */
public class CustomPlugin implements IPlugin{

    private static final String HTML_HEAD = "<!DOCTYPE html><html><head><title>Embedded Sensor Cloud</title><link rel" +
            "=\"stylesheet\" type=\"text/css\" href=\"/static/style.css\"/></head><body><h1>Custom-Plugin</h1><div><d" +
            "iv><form method=\"get\"><button type=\"submit\" name=\"random\" value=\"true\">Random</button></form></d" +
            "iv>";
    private static final String YOUTUBE_HEAD = "<iframe width=\"560\" height=\"315\" src=\"//www.youtube.com/embed/?l" +
            "istType=search&list=";
    private static final String YOUTUBE_REST = "\" frameborder=\"0\" allowfullscreen autoplay=\"1\"></iframe>";
    private Boolean buttonPressed = false;

    @Override
    public boolean acceptRequest(String requestUrl) {

        return requestUrl.equals("custom");
    }

    @Override
    public void runPlugin(Socket socket, UrlClass url) {
        buttonPressed = url.hasParameters();
        this.returnPluginPage(socket);
    }

    @Override
    public void returnPluginPage(Socket socket) {
        String output = "";
        try(BufferedWriter out = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()))) {
            output += "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nConnection: close \r\n\r\n";
            output += HTML_HEAD;
            if(this.buttonPressed){
                output += YOUTUBE_HEAD + getRandomSong() + YOUTUBE_REST;
            }
            output += "</body></html>";
            out.write(output);
            out.flush();
            System.out.println("Sent 200 OK to " + socket.getRemoteSocketAddress().toString());
        }
        catch(IOException | NullPointerException  e) {
            e.printStackTrace();
            new HttpResponse(socket, 500, "bla");
        }
    }

    private String getRandomSong(){
        ArrayList<String> tempList = new ArrayList<>();
        try(BufferedReader fileReader = new BufferedReader(new FileReader("res/song_data.dat"))){
            String temp;
            while ((temp = fileReader.readLine()) != null) {
                tempList.add(temp);
            }
        }
        catch(IOException | NullPointerException  e) {
            e.printStackTrace();
        }
        return tempList.get((int) Math.floor(Math.random()* (tempList.size() - 1)));
    }

}
