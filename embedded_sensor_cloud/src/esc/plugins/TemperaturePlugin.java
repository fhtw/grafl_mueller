package esc.plugins;

import esc.HttpResponse;
import esc.IPlugin;
import esc.UrlClass;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.sql.*;

public class TemperaturePlugin implements IPlugin{
    //JDBC objects.
    private Connection con;
    private  ResultSet rs;
    private int page, month, day, year;
    private boolean toDownload;

    private static final String TEMP_HEAD = "<!DOCTYPE html><html><head><title>Embedded Sensor Cloud</title><link rel" +
            "=\"stylesheet\" type=\"text/css\" href=\"/static/style.css\"/></head><body><h1>Temperature-Plugin</h1><div>";

    private static final String TEMP_FORM = "<form name = \"temp\" method = \"get\" accept-charset=\"UTF-8\">Enter " +
            "day here: <input type=\"text\" name=\"day\"/> month here: <input type=\"text\" name=\"month\"/> year here:" +
            "<input type=\"text\" name=\"year\"/> <input type=\"submit\" value=\"Search\"/> " +
            "<button type=\"submit\" name=\"dl\" value=\"true\">Download</button>" +
            "</form></div>";

    private String XML_HEAD = "<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n\t<Temperature>";

    @Override
    public boolean acceptRequest(String requestUrl) {

        return requestUrl.equals("temp");
    }

    @Override
    public void runPlugin(Socket socket, UrlClass url) throws SQLException {
        boolean next = false, prev = false;

        if(url.getParameters().containsKey("prev")){
            prev = Boolean.parseBoolean(url.getParameters().get("prev").toString());
        }
        if(url.getParameters().containsKey("next")){
            next = Boolean.parseBoolean(url.getParameters().get("next").toString());
        }
        if(url.getParameters().containsKey("dl")){
            toDownload =  Boolean.parseBoolean(url.getParameters().get("dl").toString());
            prev = next = false;
        }
        else{
            toDownload = false;
        }
        if(this.connectDB() == -1)
        {
            System.out.println("Connection failed!");
            return;
        }

        if( next ^ prev ){  //XOR
            if(prev) page--;
            if(next) page++;
            if(page <= 0) page = 1;
            System.out.println(page);
            rs = this.selectDate(day, month, year);
            this.returnPluginPage(socket);
            return;
        }

        if(!url.hasParameters()){
            month = 0;
            day = 0;
            year = 0;
            toDownload = false;
        }
        page = 1;
        if(rs != null) rs.close();

        if(url.getParameters().containsKey("day")){
            if(!(isNumeric(url.getParameters().get("day").toString()))) day = 0;
            else day = Integer.parseInt(url.getParameters().get("day").toString());
            System.out.println(day);
        }
        if(url.getParameters().containsKey("month")){
            if(!(isNumeric(url.getParameters().get("month").toString()))) month = 0;
            else month = Integer.parseInt(url.getParameters().get("month").toString());
            System.out.println(month);
        }

        if(url.getParameters().containsKey("year")){
            if(!(isNumeric(url.getParameters().get("year").toString()))) year = 0;
            else year = Integer.parseInt(url.getParameters().get("year").toString());
            System.out.println(year);
        }

        rs = this.selectDate(day, month, year);
        this.returnPluginPage(socket);
        //this.setRandomValues(5000,10,8,4,2012);

    }

    @Override
    public void returnPluginPage(Socket socket) {
        int valuesPerPage = 27, i = 0;

        String output = "", date = "";
        try(BufferedWriter out = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()))) {

            if(!toDownload)
            {
                output += "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nConnection: close \r\n\r\n";
                output += TEMP_HEAD;
                if(con != null){
                    output += TEMP_FORM;
                    output += "<div><form method=\"get\">" +
                        "<button type=\"submit\" name=\"prev\" value=\"true\">Prev</button>" +
                        " #" + page + " " +
                        " <button type=\"submit\" name=\"next\" value=\"true\">Next</button>" +
                        "</form>";
                    /*
                    output +="<br/><a href=\"http://www.funelf.net/photos/I-have-no-idea-what-i%60m-doing.jpg\" download>" +
                            "Download something</a>";
                    */

                    output += "</div><div><p>";
                    try
                    {
                        if(rs != null){
                            while (rs.next()) {
                                i++;
                                if(i <= valuesPerPage * page && i > valuesPerPage * (page-1))
                                {
                                    output += rs.getString("day")
                                        + "." + rs.getString("month") + "." + rs.getString("year") + ": " +
                                            ((double) Math.round(rs.getDouble("Temperature") * 100) / 100)  +
                                    " " + rs.getString("unit") + "<br/>";
                                }
                            }
                            rs.close();
                        }
                        else
                        {
                            output += "<span style=\"color:blue\"> No Result found! </span>";
                        }

                    }
                    catch(Exception e)
                    {
                        e.printStackTrace();
                    }
                    output += "</p></div>";
                }
                else{
                    output += "<span style=\"color:red\"> No Connection established yet! </span>";
                }

                output += "</body></html>";
            }
            else{
                try{
                    if (rs != null){
                        output += XML_HEAD;
                        while (rs.next()) {

                            date = "\n\t\t\t<date>"+rs.getInt("day") + "." + rs.getInt("month") + "."
                                    + rs.getInt("year") + "</date>";


                            output += "\n\t\t<data>" + date + "\n\t\t\t<value>" +
                                    ((double) Math.round(rs.getDouble("Temperature") * 100) / 100) + "</value>" +
                                    "\n\t\t\t<unit>" + rs.getString("unit") + "</unit>" +
                                    "\n\t\t</data>";

                        }
                        output += "\n\t</Temperature>";
                        rs.close();
                    }
                }
                catch(Exception e){
                    e.printStackTrace();
                }
            }
            out.write(output);
            out.flush();
            System.out.println("Sent 200 OK to " + socket.getRemoteSocketAddress().toString());
        }
        catch(IOException | NullPointerException  e) {
            e.printStackTrace();
            new HttpResponse(socket, 500, "huehue");
        }

    }

    //DB Handle stuff:
    public int connectDB() {

        try
        {
            if (con != null) con.close();

            // Establish the connection.
            Class.forName("com.microsoft.sqlserver.jdbc.SQLServerDriver");
            String connectionUrl = "jdbc:sqlserver://PHIPS-THINK\\SQLEXPRESS;databaseName=SensorCloudSWE1";
            con =  DriverManager.getConnection(connectionUrl, "Remote", "1q2w3e");
        }
        catch (Exception e) {
            e.printStackTrace();
            return -1;
        }
        return 1;
    }

    public ResultSet selectDate(int day, int month, int year)
    {
        if (con == null)
        {
            System.out.println("No Connection established yet!");
            return null;
        }
        PreparedStatement stmt;
        ResultSet rs;
        String SQL;
        int indexDay = 0, indexMonth = 0, indexYear = 1;

        try
        {

            SQL = "SELECT * FROM temperatureSensor WHERE";

            if (day == 0) SQL += " day is not null";
            else
            {
                SQL += " day = ?";
                indexDay = 1;
                indexYear++;
            }

            if (month == 0) SQL += " and month is not null";
            else
            {
                SQL += " and month = ?";
                indexMonth = indexDay + 1;
                indexYear++;
            }

            if (year == 0) SQL += " and year is not null";
            else SQL += " and year = ?";


            stmt = con.prepareStatement(SQL);
            if(day != 0) stmt.setInt(indexDay, day);
            if(month != 0) stmt.setInt(indexMonth, month);
            if(year != 0) stmt.setInt(indexYear, year);

            rs = stmt.executeQuery();
            return rs;
        }
        catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }
    public int closeDbConnection()
    {
        try
        {
            if (con != null) con.close();
        }
        // Handle any errors that may have occurred.
        catch (Exception e) {
            e.printStackTrace();
            return -1;
        }
        return 1;
    }
    private static boolean isNumeric(String str)
    {
        try
        {
            double d = Double.parseDouble(str);
        }
        catch(NumberFormatException nfe)
        {
            return false;
        }
        return true;
    }
}