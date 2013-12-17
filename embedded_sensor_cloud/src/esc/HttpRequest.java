package esc;


import java.net.Socket;
import java.util.HashMap;


/**
 * @author Alex
 */
public class HttpRequest {
    private String protocol;
    private UrlClass url;
    private String httpVersion;
    private Socket socket;
    private HashMap<String, String> requestOptions;

    HttpRequest(String[] s, Socket socket) {
        //Request headline aufteilen und decodieren
        this.protocol = s[0];
        this.url = new UrlClass(s[1]);
        this.httpVersion = s[2];
        this.socket = socket;
        this.requestOptions = new HashMap<>();
		if(! url.parseUrl()){
            new HttpResponse(this.socket, 500, "");
        }
    }
    public String getProtocol(){
        return this.protocol;
    }
    public void processRequest(){

        if(! Server.pluginManager.findPlugin(this.socket, this.url)){
                //page not found, yo
                new HttpResponse(this.socket, 404, url.getFullPath());
        }

    }

    public void addLine(String line){
        String[] foo = line.split(": ");
        if(foo.length > 1){
            this.requestOptions.put(foo[0], foo[1]);
        }
        else{
            url.parseParameters(line);
        }
    }
}
