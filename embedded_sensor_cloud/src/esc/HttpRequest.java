package esc;

import com.sun.org.apache.bcel.internal.util.ClassLoader;


import java.io.*;
import java.net.Socket;
import java.net.URLDecoder;
import java.util.LinkedList;
import java.util.List;


/**
 * @author Alex
 */
public class HttpRequest {
    private String _protocol;
    private String _path;
    private String _httpVersion;
    private Socket _socket;

    HttpRequest(String[] s, Socket socket) throws UnsupportedEncodingException {
        //Request headline aufteilen und decodieren
        this._protocol = s[0];
        this._path = URLDecoder.decode(s[1], "UTF-8");
        this._httpVersion = s[2];
        this._socket = socket;
        this.checkPluginAcceptance();
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
                if(iPlugin.acceptRequest(_path)){
                    iPlugin.runPlugin(_socket);
                    requestProcessed = true;
                    break;
                }
            }
        }
        catch (IOException | NullPointerException | ClassNotFoundException | InstantiationException |
                IllegalAccessException e) {
            e.printStackTrace();
        }
        finally{
            if(!requestProcessed){
                //page not found, yo
                respondPageNotFound();
            }
        }
    }

    private void respondPageNotFound(){
        //PAGE NOT FOUND RESPONSE HERST
        try(BufferedWriter out = new BufferedWriter(new OutputStreamWriter(_socket.getOutputStream()))) {
            out.write("HTTP/1.1 404 Not Found\r\n");
            out.write("Content-Type: text/html\r\n\r\n");
            out.write("<head><title>404 Page Not Found</title></head>\n");
            out.write("<body><h1>404 Page Not Found</h1>\r\n");
            out.write("The requested URL " + _path + " was not found on this server.\n");
            out.write("<hr>\nEmbedded Sensor Cloud - Grafl & M&#252;ller</body>\r\n");
            System.out.println("Sent 404 Not Found to " + _socket.getRemoteSocketAddress().toString());
            out.flush();
        }
        catch(IOException | NullPointerException e) {
            e.printStackTrace();
        }
    }
}
