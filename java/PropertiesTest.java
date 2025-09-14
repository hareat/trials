/* Trial for Java properties.
 * compile: javac PropertiesTest.java
 */

import java.io.FileInputStream;
import java.util.Properties;

/**
 * Testet, ob es moeglich ist, die System-Properties mit Hilfe von
 * System.getProperties().load() um die eigenen Properties zu erweitern.
 * Antwort: ja
 * Wenn statt dessen System.setProperties() verwendet wird,
 * dann ist darauf zu achten, dass die neuen Properties die
 * alten System-Properties als default mit bekommen.
 */
public class PropertiesTest {
	public static void main(String[] args) throws Exception {
		System.out.println(System.getProperties());
		System.getProperties().load(new FileInputStream("PropertiesTest.properties"));
		System.out.println("\nnachher:");
		System.out.println(System.getProperties());
		System.out.println(System.getProperty("user.home"));

		// und wenn statt dessen System.setProperties() verwendet wird
		Properties props = new Properties(System.getProperties());
		props.load(new FileInputStream("PropertiesTest.properties"));
		System.setProperties(props);
		System.out.println("\nnachher:");
		System.out.println(System.getProperties());
		System.out.println(System.getProperty("user.home"));

		// und wenn auf die alten System-Properties vergessen wird
		props = new Properties();
		props.load(new FileInputStream("PropertiesTest.properties"));
		System.setProperties(props);
		System.out.println("\nnachher:");
		System.out.println(System.getProperties());
		System.out.println(System.getProperty("user.home"));
	}
}
