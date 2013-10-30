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

        assertEquals("valid", url.pluginPath);
        assertArrayEquals(expectedPath, url.splitFullPath);
        assertTrue(url.parameters.isEmpty());
    }

    @Test
    public void complexValidUrlPasses(){
        UrlClass url = new UrlClass("/valid/complex/url.xml?parameter1=123&parameter2=asdf");
        String[] expectedPath =  {"valid", "complex", "url.xml"};
        FileThing expectedFile = new FileThing();
        expectedFile.name = "url";
        expectedFile.ending = "xml";
        Map expectedMap = new HashMap<String, String>();
        expectedMap.put("parameter1", "123");
        expectedMap.put("parameter2", "asdf");

        assertEquals(expectedMap, url.parameters);
        assertEquals(expectedFile.name, url.file.name);
        assertEquals(expectedFile.ending, url.file.ending);
        assertArrayEquals(expectedPath, url.splitFullPath);
        assertEquals("valid", url.pluginPath);
    }

    @Test
    public void rootUrlPasses(){
        UrlClass url = new UrlClass("/");

        assertEquals("/", url.pluginPath);
        assertTrue(url.parameters.isEmpty());
    }
}
