package esc;

import esc.plugins.StaticPagePlugin;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

import java.net.Socket;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 * @author Alex
 */
@RunWith(JUnit4.class)
public class StaticPagePluginTest {
    @Test
    public void pluginAcceptanceTest() {
        StaticPagePlugin sp = new StaticPagePlugin();
        assertEquals(sp.acceptRequest("/"), true);
        assertEquals(sp.acceptRequest("static"), true);
        assertEquals(sp.acceptRequest("wooooop"), false);
    }

    @Test
    public void runPluginTest(){
        StaticPagePlugin sp = new StaticPagePlugin();
        UrlClass url = new UrlClass("/static");
        url.parseUrl();
        sp.runPlugin(new Socket(), url);
    }

    public void runDirectoryPluginTest(){
        StaticPagePlugin sp = new StaticPagePlugin();
        UrlClass url = new UrlClass("/static/stuff");
        url.parseUrl();
        sp.runPlugin(new Socket(), url);
    }
}
