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

        if(requestUrl.equals("temp")) return true;
        else return false;
    }

    @Override
    public void runPlugin(Socket socket, UrlClass url) {
        //TODO: Implement Plugin here.
    }

    @Override
    public void returnPluginPage(Socket socket) {
        //To change body of implemented methods use File | Settings | File Templates.
    }
}
