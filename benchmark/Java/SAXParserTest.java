import java.io.FileInputStream;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

public class SAXParserTest {

  public static void main(String[] args) throws Exception {
    SAXParserFactory parserFactor = SAXParserFactory.newInstance();
    parserFactor.setFeature("http://javax.xml.XMLConstants/feature/secure-processing", false);
    SAXParser parser = parserFactor.newSAXParser();
    SAXHandler handler = new SAXHandler();
    parser.parse(new FileInputStream(args[0]), 
                 handler);

    System.out.println(handler.n);
  }
}
/**
 * The Handler for SAX Events.
 */
class SAXHandler extends DefaultHandler {

  int n = 0;
  @Override
  //Triggered when the start of tag is found.
  public void startElement(String uri, String localName, 
                           String qName, Attributes attributes) 
                           throws SAXException {

    n = n + 1;
  }

  @Override
  public void endElement(String uri, String localName, 
                         String qName) throws SAXException {
  }

  @Override
  public void characters(char[] ch, int start, int length) 
          throws SAXException {
    
  }

}
