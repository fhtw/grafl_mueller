package esc;

import esc.plugins.TemperaturePlugin;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;

/**
 * @author Alex
 */
@RunWith(JUnit4.class)
public class CustomClassLoaderTest {
    @Test
    public void findFinableClass() {
        CustomClassLoader cl = new CustomClassLoader();
        Class test = cl.findClass("esc.plugins.TemperaturePlugin");
        assertEquals(test, TemperaturePlugin.class);
    }
    @Test
    public void cannotFindClass() {
        CustomClassLoader cl = new CustomClassLoader();
        Class test = cl.findClass("esc.plugins.Woop");
        assertNull(test);
    }
}
