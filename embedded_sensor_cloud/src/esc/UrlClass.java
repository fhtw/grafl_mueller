package esc;

import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

/**
 * @author Alex
 */
public class UrlClass {
    private String rawUrl;
    public String fullPath;
    public String[] splitFullPath;
    public Map parameters;
    public String pluginPath;
    public FileThing file;

    UrlClass(String raw) {
        try {
            this.rawUrl = URLDecoder.decode(raw, "UTF-8");
        } catch (UnsupportedEncodingException e) {
            System.out.println("Uh oh, something bad happend!");
            return;
        }
        parameters = new HashMap<String, String>();
        file = new FileThing();
        splitFullPath = null;
        this.parseUrl();
    }

    private void parseUrl() {
        String[] urlParts = rawUrl.split("\\?");
        this.fullPath = urlParts[0];
        String[] pathParts = fullPath.split("/");
        if(pathParts.length > 0) {
            pluginPath = pathParts[1];
            splitFullPath =  Arrays.copyOfRange(pathParts, 1, pathParts.length);
        }
        else {
            pluginPath = fullPath = "/";
            return;
        }
        String[] foo = pathParts[pathParts.length - 1].split("\\.");
        if(foo.length > 1) {
            file.name = foo[0];
            file.ending = foo[1];
        }
        if(urlParts.length > 1){
            for(String part : urlParts[1].split("&")){
                parameters.put(part.split("=")[0], part.split("=")[1]);
            }
        }
    }
}

class FileThing {
    public String name;
    public String ending;
}
