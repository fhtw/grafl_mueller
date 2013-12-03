package esc.plugins;

import esc.IPlugin;
import esc.UrlClass;

import java.net.Socket;

/**
 * @author Alex
 */
public class NaviPlugin implements IPlugin{
    public boolean acceptRequest(String requestUrl) {
        if(requestUrl.equals("navi")) return true;
        else return false;
    }

    public void runPlugin(Socket socket, UrlClass url) {
        //TODO: Implement Navi plugin here
    }
}
