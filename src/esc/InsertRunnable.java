package esc;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.util.Calendar;
import static java.lang.Math.sin;

/**
 * Created with IntelliJ IDEA.
 * User: Phips
 * Date: 07.01.14
 * Time: 11:50
 * To change this template use File | Settings | File Templates.
 */
public class InsertRunnable implements Runnable {
    private Connection con;

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
    public void setRandomValues(int reps, int valuesPerDay, int day, int month, int year)
    {   //Insert multiple sinus Values into DB

        //security stuff
        if(con == null)
        {
            System.out.println("No Connection established yet!");
            return;
        }
        if(reps > 80000)
        {
            System.out.println("The maximum of allowed reps per call has been reached - no values inserted.");
            return;
        }
        /*
        * Declaration and 'real' method
        * */
        int i, dayPerMonth = 31;
        boolean first = true;
        double value;
        String SQL, unitType = "Celsius";
        PreparedStatement stmt = null;
        try
        {
            for(i=0; i < reps; i++)
            {
                SQL = "INSERT into temperatureSensor VALUES(?,?,?,?,?)";

                if(reps > 10) value = 30 * sin(i);
                else value = 30 * sin(i + Math.random() * 15);

                stmt = con.prepareStatement(SQL);
                stmt.setDouble(1,value);
                stmt.setString(2,unitType);
                stmt.setInt(3,day);
                stmt.setInt(4,month);
                stmt.setInt(5,year);
                stmt.executeUpdate();


                if(i % valuesPerDay == 0 && !first)
                {
                    if(month % 2 == 0) dayPerMonth = 30;
                    if(month % 2 == 1) dayPerMonth = 31;
                    if(month == 9) dayPerMonth = 30;
                    if(month == 10) dayPerMonth = 31;
                    if(month == 11) dayPerMonth = 30;
                    if(month == 8) dayPerMonth = 31;
                    if(month == 2) dayPerMonth = 28;
                    if(++day > dayPerMonth)
                    {
                        day = 1;
                        if(++month > 12)
                        {
                            year++;
                            month = 1;
                        }
                    }
                }
                first = false;
            }
            if(stmt != null) stmt.close();

        }
        catch (Exception e) {
            e.printStackTrace();
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

    @Override
    public void run() {
        int year, month, day;

        this.connectDB();
        while (true){
            year = Calendar.getInstance().get(Calendar.YEAR);
            month = Calendar.getInstance().get(Calendar.MONTH) + 1; //January = 0 ... February = 1? not tested yet
            day = Calendar.getInstance().get(Calendar.DAY_OF_MONTH);
            this.setRandomValues(1,1,day,month,year);
            try {
                Thread.sleep(15000);
            } catch (InterruptedException e) {
                e.printStackTrace();
                this.closeDbConnection();
            }
        }
    }
}
