package esc;

import java.net.Socket;

/**
 * @author Alex
 */

//Interface für die Plugins, zum Plugin pluginnen
public interface IPlugin {
    public boolean acceptRequest(String requestUrl);
    public void runPlugin(Socket socket, UrlClass url);
}
