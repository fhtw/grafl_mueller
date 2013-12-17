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

    public PluginManager(){
        //Alle plugins durchgehen ob sie den Request verarbeiten können/wollen
        try{
            File dir = new File("./src/esc/plugins");
            File[] fileList = dir.listFiles();
            if (fileList == null) throw new AssertionError();
            for(File file : fileList){
                //magic
                Object o;
                CustomClassLoader classLoader = new CustomClassLoader();
                Class c = classLoader.loadClass("esc.plugins." + (file.getName()).split("\\.")[0]);
                if(IPlugin.class.isAssignableFrom(c)){
                    o = c.newInstance();
                    pluginList.add((IPlugin) o);
                }
            }
        }
        catch ( NullPointerException | ClassNotFoundException | InstantiationException |
                IllegalAccessException e) {
            e.printStackTrace();
        }
    }


    public boolean findPlugin(Socket socket, UrlClass url){
        for(IPlugin iPlugin : this.pluginList){
            if(iPlugin.acceptRequest(url.getPluginPath())){
                iPlugin.runPlugin(socket, url);
                return true;
            }
        }
        return false;
    }


}
