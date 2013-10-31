package esc;

import esc.plugin.IPlugin;


import java.io.*;
import java.net.Socket;
import java.util.LinkedList;
import java.util.List;


/**
 * @author Alex
 */
public class HttpRequest {
    private String _protocol;
    private UrlClass _url;
    private String _httpVersion;
    private Socket _socket;

    HttpRequest(String[] s, Socket socket) throws UnsupportedEncodingException {
        //Request headline aufteilen und decodieren
        this._protocol = s[0];
        this._url = new UrlClass(s[1]);
        this._httpVersion = s[2];
        this._socket = socket;
        if(_url.parseUrl()){
            this.checkPluginAcceptance();
        }
        else{
            new HttpResponse(_socket, 500, "");
        }
    }

    private void checkPluginAcceptance()
    {
        //Alle plugins durchgehen ob sie den Request verarbeiten können/wollen
        boolean requestProcessed = false;
        try(BufferedReader fileReader = new BufferedReader(new FileReader("res/PluginConfig.conf"))){
            String pluginName;
            List<IPlugin> pluginList = new LinkedList<>();
            CustomClassLoader classLoader = new CustomClassLoader();
            //ClassLoader classLoader = new ClassLoader();
            while((pluginName = fileReader.readLine()) != null){
                //magic
                Object o;
                Class c = classLoader.loadClass(pluginName);
                o = c.newInstance();
                pluginList.add((IPlugin) o);
            }
            for(IPlugin iPlugin : pluginList){
                if(iPlugin.acceptRequest(_url.pluginPath)){
                    iPlugin.runPlugin(_socket);
                    requestProcessed = true;
                    break;
                }
            }
        }
        catch (IOException | NullPointerException | ClassNotFoundException | InstantiationException |
                IllegalAccessException e) {
            e.printStackTrace();
            new HttpResponse(_socket, 500, _url.fullPath);
        }
        finally{
            if(!requestProcessed){
                //page not found, yo
                new HttpResponse(_socket, 404, _url.fullPath);
            }
        }
    }
}
                         f