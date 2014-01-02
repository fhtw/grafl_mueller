package esc.plugins;

import esc.IPlugin;
import esc.UrlClass;

import java.net.Socket;

/**
 * @author Alex
 */
public class TemperaturePlugin implements IPlugin{
    @Override
    public boolean acceptRequest(String requestUrl) {

        return requestUrl.equals("temp");
    }

    @Override
    public void runPlugin(Socket socket, UrlClass url) {
        //TODO: Implement Plugin here.
    }

    @Override
    public void returnPluginPage(Socket socket) {
        //Hier zumindest die startpage, bzw man kanns auch bauen, so das immer die seite hier ausgebeben wird, siehe navi
    }
}
