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
    private String fullPath;
    private String[] splitFullPath;
    private Map parameters;
    private String pluginPath;
    private FileThing file;

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
    }

    public String[] getSplitFullPath(){
        return splitFullPath;
    }
    public String getRawUrl(){
        return rawUrl;
    }

    public String getFullPath(){
        return fullPath;
    }

    public String getPluginPath(){
        return pluginPath;
    }

    public Map getParameters(){
        return parameters;
    }

    public FileThing getFile(){
        return file;
    }

    public boolean parseUrl() {
        try{
            String[] urlParts = rawUrl.split("\\?");
            this.fullPath = urlParts[0];
            String[] pathParts = fullPath.split("/");
            if(pathParts.length > 0) {
                pluginPath = pathParts[1];
                splitFullPath =  Arrays.copyOfRange(pathParts, 1, pathParts.length);
            }
            else {
                pluginPath = fullPath = "/";
                return true;
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
            return true;
        }
        catch(ArrayIndexOutOfBoundsException | NullPointerException | IllegalArgumentException e){
            System.out.println("URL parse error!");
            e.printStackTrace();
            return false;
        }
    }
}

class FileThing {
    private String name;
    private String extension;

    String getName(){
        return name;
    }

    void setName(String name){
        this.name = name;
    }

    String getExtension(){
        return extension;
    }

    void setExtension(String extension){
        this.extension = extension;
    }
}
