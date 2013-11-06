package esc;

import static org.junit.Assert.*;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

import java.util.HashMap;
import java.util.Map;

/**
 * @author Alex
 */
@RunWith(JUnit4.class)
public class UrlClassTest {

    @Test
    public void simpleValidUrlPasses() {
        UrlClass url = new UrlClass("/valid/simple/url");
        String[] expectedPath = {"valid", "simple", "url"};

        assertEquals("valid", url.getPluginPath());
        assertArrayEquals(expectedPath, url.getSplitFullPath());
        assertTrue(url.getParameters().isEmpty());
    }

    @Test
    public void complexValidUrlPasses(){
        UrlClass url = new UrlClass("/valid/complex/url.xml?parameter1=123&parameter2=asdf");
        String[] expectedPath =  {"valid", "complex", "url.xml"};
        FileThing expectedFile = new FileThing();
        expectedFile.setName("url");
        expectedFile.setExtension("xml");
        Map expectedMap = new HashMap<String, String>();
        expectedMap.put("parameter1", "123");
        expectedMap.put("parameter2", "asdf");

        assertEquals(expectedMap, url.getParameters());
        assertEquals(expectedFile.getName(), url.getFile().getName());
        assertEquals(expectedFile.getExtension(), url.getFile().getExtension());
        assertArrayEquals(expectedPath, url.getSplitFullPath());
        assertEquals("valid", url.getPluginPath());
    }

    @Test
    public void rootUrlPasses(){
        UrlClass url = new UrlClass("/");

        assertEquals("/", url.getPluginPath());
        assertTrue(url.getParameters().isEmpty());
    }
}
