package esc.plugins;

import esc.HttpResponse;
import esc.IPlugin;
import esc.UrlClass;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.util.HashMap;

import org.xml.sax.helpers.DefaultHandler;
/**
 * @author Alex
 */
public class NaviPlugin extends DefaultHandler implements IPlugin{

    private HashMap<String, String> naviEntires;
    private String NAVI_HEAD = "<!DOCTYPE html><html><head><title>Embedded Sensor Cloud</title><link rel=\"styl" +
            "esheet\" type=\"text/css\" href=\"/static/style.css\"/></head><body><h1>Navi-Plugin</h1><div><div>" +
            "<form method=\"get\"><button type=\"submit\" name=\"refresh\" value=\"true\">Refresh</button></for" +
            "m></div>";
    private String NAVI_FORM = "<form name = \"navi\" method = \"get\">Enter streetname here: <input type=\"tex" +
            "t\" name=\"street\"/> <input type=\"submit\" value=\"Submit\"/></form></div>";

    private boolean formReady = false;
    private boolean isParsing = false;

    @Override
    public boolean acceptRequest(String requestUrl) {
        if(requestUrl.equals("navi")) return true;
        else return false;
    }

    @Override
    public void runPlugin(Socket socket, UrlClass url) {

        if(!url.hasParameters()){
            this.returnPluginPage(socket);
            return;
        }
        if(url.getParameters().containsKey("refresh")){
            if(!this.isParsing){
                this.isParsing = true;
                this.formReady = false;
                this.formReady = this.updateNaviEntries();
                this.isParsing = false;
            }
        }
    }

    private boolean updateNaviEntries(){


        return true;
    }

    @Override
    public void returnPluginPage(Socket socket) {
        String output = "";
        try(BufferedWriter out = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()))) {
            output += "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close \r\n\r\n";
            output += NAVI_HEAD;
            if(this.formReady){
                output += NAVI_FORM ;
            }
            else{
                output += "<span style=\"color:red\"> Streetnames not parsed or currently in the process</span>";
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
}
