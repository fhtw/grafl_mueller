package esc;

import static org.junit.Assert.*;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

/**
 * @author Alex
 */
@RunWith(JUnit4.class)
public class PluginManagerTest {
    @Test
    public void initializePluginManager() {
        PluginManager pm = new PluginManager();
        assertNotNull(pm);
    }
}
