package esc.plugins;

import org.xml.sax.*;
import org.xml.sax.helpers.*;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.Stack;

/**
 * @author Alex
 */
public class NaviXmlParser extends DefaultHandler{

    private HashMap<String, LinkedList<String>> tags;
    private Stack<String> nodeStack;
    private String tmpCity;
    private String tmpStreet;

    @Override
    public void startDocument() throws SAXException {
        System.out.println("Started parsing document!");
        nodeStack = new Stack<>();
        tags = new HashMap<>();
    }

    @Override
    public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException {
        if(qName.equals("node")){
            nodeStack.push(qName);
        }
        else if(qName.equals("tag")){
            nodeStack.push(qName);
            if(attributes.getValue("k").equals("addr:city")){
                tmpCity = attributes.getValue("v");
            }
            if(attributes.getValue("k").equals("addr:street")){
                tmpStreet = attributes.getValue("v");
            }
            pushToMap();
        }
    }

    @Override
    public void endElement(String uri, String localName, String qName) throws SAXException {
        if(! nodeStack.empty()) {
            if(nodeStack.peek().equals("node")){
                tmpCity = "";
                tmpStreet = "";
            }
            nodeStack.pop();
        }
    }

    private void pushToMap(){
        if(!tmpCity.equals("") && !tmpStreet.equals("")){
            if(tags.containsKey(tmpStreet)){
                if(! tags.get(tmpStreet).contains(tmpCity)) tags.get(tmpStreet).add(tmpCity);
            }
            else{
                LinkedList<String> tmp = new LinkedList<>();
                tmp.add(tmpCity);
                tags.put(tmpStreet, tmp);
            }
        }
    }

    public HashMap<String, LinkedList<String>> getResults(){
        return tags;
    }
}
