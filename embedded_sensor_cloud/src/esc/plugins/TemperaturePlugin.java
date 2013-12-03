package esc.plugins;

import esc.IPlugin;
import esc.UrlClass;

import java.net.Socket;

/**
 * @author Alex
 */
public class TemperaturePlugin implements IPlugin{
    public boolean acceptRequest(String requestUrl) {

        if(requestUrl.equals("temp")) return true;
        else return false;
    }

    public void runPlugin(Socket socket, UrlClass url) {
        //TODO: Implement Plugin here.
    }
}
