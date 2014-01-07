package esc;

import java.net.Socket;
import java.sql.SQLException;

/**
 * @author Alex
 */

//Interface für die Plugins, zum Plugin pluginnen
public interface IPlugin {
    public boolean acceptRequest(String requestUrl);
    public void runPlugin(Socket socket, UrlClass url) throws SQLException;
    void returnPluginPage(Socket socket);
}
