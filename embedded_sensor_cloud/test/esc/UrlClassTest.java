package esc;

import static org.junit.Assert.*;

import org.junit.Ignore;
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
        url.parseUrl();
        String[] expectedPath = {"valid", "simple", "url"};

        assertEquals("valid", url.getPluginPath());
        assertArrayEquals(expectedPath, url.getSplitFullPath());
        assertTrue(url.getParameters().isEmpty());
    }

    @Test
    public void complexValidUrlPasses(){
        UrlClass url = new UrlClass("/valid/complex/url.xml?parameter1=123&parameter2=asdf");
        url.parseUrl();
        String[] expectedPath =  {"valid", "complex", "url.xml"};
        FileThing expectedFile = new FileThing();
        expectedFile.setName("url");
        expectedFile.setExtension("xml");
        HashMap expectedMap = new HashMap<String, String>();
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
        url.parseUrl();

        assertEquals("/", url.getPluginPath());
        assertTrue(url.getParameters().isEmpty());
    }

    @Test
    public void complexInvalidUrlPasses(){
        UrlClass url = new UrlClass("/foo//foo.bar?=bla*&a=b&moo='?sds&qwwe&ai");
        boolean ok = url.parseUrl();
        assertEquals(ok, true);

        String[] expectedPath = {"foo", "", "foo.bar"};
        FileThing expectedFile = new FileThing();
        expectedFile.setName("foo");
        expectedFile.setExtension("bar");
        HashMap expectedMap = new HashMap<String, String>();
        expectedMap.put("", "bla*");
        expectedMap.put("a", "b");
        expectedMap.put("moo", "'");

        assertEquals(expectedMap, url.getParameters());
        assertEquals(expectedFile.getName(), url.getFile().getName());
        assertEquals(expectedFile.getExtension(), url.getFile().getExtension());
        assertArrayEquals(expectedPath, url.getSplitFullPath());
        assertEquals("foo", url.getPluginPath());
    }
}
