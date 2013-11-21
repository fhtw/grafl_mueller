package esc;

import java.io.File;
import java.io.IOException;
import java.net.Socket;
import java.util.LinkedList;

/**
 * @author Alex
 */
public class PluginManager {

    private LinkedList<IPlugin> pluginList = new LinkedList<>();
    private CustomClassLoader classLoader = new CustomClassLoader();
    private Socket socket;
    private UrlClass url;

    public PluginManager(Socket socket, UrlClass url){
        this.socket = socket;
        this.url = url;
        //Alle plugins durchgehen ob sie den Request verarbeiten können/wollen
        try{
            File dir = new File("./src/esc/plugins");
            File[] fileList = dir.listFiles();
            for(File file : fileList){
                //magic
                Object o;
                Class c = classLoader.loadClass("esc.plugins." + (file.getName()).split("\\.")[0]);
                o = c.newInstance();
                pluginList.add((IPlugin) o);
            }
        }
        catch ( NullPointerException | ClassNotFoundException | InstantiationException |
                IllegalAccessException e) {
            e.printStackTrace();
            new HttpResponse(this.socket, 500, url.getFullPath());
        }
    }


    public boolean findPlugin(String pluginPath){
        for(IPlugin iPlugin : this.pluginList){
            if(iPlugin.acceptRequest(pluginPath)){
                iPlugin.runPlugin(this.socket, this.url);
                return true;
            }
        }
        return false;
    }


}
