package esc.plugins;

import esc.HttpResponse;
import esc.IPlugin;
import esc.UrlClass;
import org.xml.sax.SAXException;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.Set;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;


/**
 * @author Alex
 */
public class NaviPlugin implements IPlugin{

    private static final String NAVI_HEAD = "<!DOCTYPE html><html><head><title>Embedded Sensor Cloud</title><link rel" +
            "=\"stylesheet\" type=\"text/css\" href=\"/static/style.css\"/></head><body><h1>Navi-Plugin</h1><div>";
     private static final String NAVI_REFRESH = "<div><form method=\"get\"><button type=\"submit\" name=\"refresh\" v" +
             "alue=\"true\">Refresh</button></form></div>";
    private static final String NAVI_FORM = "<form name = \"navi\" method = \"get\" accept-charset=\"UTF-8\">Enter st" +
            "reetname here: <input type=\"text\" name=\"street\"/> <input type=\"submit\" value=\"Submit\"/></form></" +
            "div>";
    public HashMap<String, LinkedList<String>> naviEntires;
    private boolean formReady = false;
    private boolean isParsing = false;
    private SAXParser sp;
    private NaviXmlParser naviParserHandler;
    public String streetName;

    public NaviPlugin() throws ParserConfigurationException, SAXException {
        SAXParserFactory spfac = SAXParserFactory.newInstance();
        sp = spfac.newSAXParser();
        naviParserHandler = new NaviXmlParser();
    }

    @Override
    public boolean acceptRequest(String requestUrl) {
        return requestUrl.equals("navi");
    }

    @Override
    public void runPlugin(Socket socket, UrlClass url) {
        streetName = "";
        if(!url.hasParameters()){
            this.returnPluginPage(socket);
            return;
        }
        if(url.getParameters().containsKey("refresh")){
            if(!this.isParsing){
                this.isParsing = true;
                this.formReady = false;
                this.formReady = this.updateNaviEntries("res/austria-latest.osm");
                this.isParsing = false;
            }
        }
        if(url.getParameters().containsKey("street")){
            streetName = url.getParameters().get("street").toString();
            System.out.println(streetName);
        }
        this.returnPluginPage(socket);
    }

    public synchronized boolean updateNaviEntries(String parseWhat){
        try {
            sp.parse(parseWhat, naviParserHandler);
        } catch (SAXException | IOException e) {
            e.printStackTrace();
            return false;
        }
        naviEntires = naviParserHandler.getResults();
        System.out.println("Finished parsing document!");
        return true;
    }

    @Override
    public void returnPluginPage(Socket socket) {
        String output = "";
        try(BufferedWriter out = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()))) {
            output += "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nConnection: close \r\n\r\n";
            output += NAVI_HEAD;
            if(! this.isParsing) output += NAVI_REFRESH;
            if(this.formReady){
                output += NAVI_FORM;

                if(!streetName.equals("")){
                    if(naviEntires.containsKey(streetName)){
                        output += "<ul id = \"cities\">";
                        for(String city : naviEntires.get(streetName)){
                            output += "<li>" + city + "</li>";
                        }
                        output += "</ul>";
                    }
                    else{
                      output += "Street " + streetName + " not found!";
                    }
                }
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
