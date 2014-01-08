package esc;

import static org.junit.Assert.*;
import esc.plugins.NaviPlugin;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;
import org.xml.sax.SAXException;

import javax.xml.parsers.ParserConfigurationException;
import java.io.IOException;
import java.net.Socket;
import java.util.HashMap;
import java.util.LinkedList;

/**
 * @author Alex
 */
@RunWith(JUnit4.class)
public class NaviPluginTest {

    @Test
    public void pluginAcceptanceTest() throws ParserConfigurationException, SAXException {
        NaviPlugin navi = new NaviPlugin();
        assertEquals(navi.acceptRequest("navi"), true);
        assertEquals(navi.acceptRequest("asd"), false);
        assertEquals(navi.acceptRequest(""), false);
    }

    @Test
    public void parseSampleTest() throws IOException, SAXException, ParserConfigurationException {
        NaviPlugin navi = new NaviPlugin();
        HashMap<String, LinkedList<String>>  refMap = new HashMap<>();
        LinkedList<String> temp1 = new LinkedList<>();
        LinkedList<String> temp2 = new LinkedList<>();
        temp1.add("Aspern");
        temp1.add("Wien");
        temp2.add("Wolkersdorf");
        refMap.put("Bergengasse", temp1);
        refMap.put("In Freybergen", temp2);
        assertEquals(navi.updateNaviEntries("res/sampleXml.xml"), true);
        assertEquals(navi.naviEntires, refMap);
    }

    @Test
    public void runPluginTest() throws ParserConfigurationException, SAXException {
        NaviPlugin navi = new NaviPlugin();
        Socket s = new Socket();
        UrlClass url = new UrlClass("/navi?street=asdf");
        url.parseUrl();
        navi.runPlugin(s, url);
        assertEquals(navi.streetName, "asdf");
    }
}
