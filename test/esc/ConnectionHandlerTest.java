package esc;

import static org.junit.Assert.*;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

import java.net.Socket;

/**
 * @author Alex
 */
@RunWith(JUnit4.class)
public class ConnectionHandlerTest {

    @Test
    public void validHttpHeaderTest(){
        Socket s = new Socket();
        String[] refHeadline = {"GET", "/", "HTTP/1.1"};
        ConnectionHandler conn = new ConnectionHandler(s);
        String[] actualHeadline = conn.processRequestHeader("GET / HTTP/1.1");
        assertArrayEquals(refHeadline, actualHeadline);
    }

    @Test
    public void invalidHttpHeaderTest(){
        Socket s = new Socket();
        ConnectionHandler conn = new ConnectionHandler(s);
        String[] actualHeadline = conn.processRequestHeader("");
        assertNull(actualHeadline);
    }

    @Test
    public void invalidHttpHeaderTest2(){
        Socket s = new Socket();
        ConnectionHandler conn = new ConnectionHandler(s);
        String[] actualHeadline = conn.processRequestHeader("POST HTTP/1.0 woop");
        assertNull(actualHeadline);
    }
}
