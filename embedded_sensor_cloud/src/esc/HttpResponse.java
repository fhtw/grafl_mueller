package esc;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.net.Socket;

/**
 * @author Alex
 */
public class HttpResponse {
    String _path;
    Socket _socket;

    public HttpResponse(Socket socket, int errorCode, String path){
        this._socket = socket;
        this._path = path;
        switch(errorCode){
            case 404:
                pageNotFound();
                break;
            default:
            case 500:
                internalServerError();
                break;
        }
    }

    private void internalServerError(){
        try(BufferedWriter out = new BufferedWriter(new OutputStreamWriter(_socket.getOutputStream()))) {
            out.write("HTTP/1.1 500 Internal Server Error\r\n");
            out.write("Content-Type: text/html\r\n\r\n");
            out.write("<head><title>500 Internal Server Error</title></head>\n");
            out.write("<body><h1>500 Internal Server Error</h1>\r\n");
            out.write("The server encountered an internal error or misconfiguration and was unable to complete"
                    + " your request.\n");
            out.write("<hr>\nEmbedded Sensor Cloud - Grafl & M&#252;ller</body>\r\n");
            System.out.println("Sent 500 Internal Server Error to " + _socket.getRemoteSocketAddress().toString());
            out.flush();
        }
        catch(IOException | NullPointerException e) {
            e.printStackTrace();
        }

    }

    private void pageNotFound()
    {
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
        catch(IOException | NullPointerException e ) {
            e.printStackTrace();
            this.internalServerError();
        }

    }
}
