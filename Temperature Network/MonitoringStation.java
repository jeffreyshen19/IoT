import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.UnknownHostException;
import mraa.Aio;
import mraa.Dir;
import mraa.Gpio;
import mraa.IntelEdison;
import mraa.mraa;
import mraa.Platform;
import mraa.Result;
import javax.net.ssl.SSLSocket;
import java.text.SimpleDateFormat;
import java.util.Calendar;

public class MonitoringStation {

  static {
    try {
      System.loadLibrary("mraajava");
    } catch (UnsatisfiedLinkError e) {
      System.err.println(
      "Native code library failed to load. See the chapter on Dynamic Linking Problems in the SWIG Java documentation for help.\n" +
      e);
      System.exit(1);
    }
  }

  public static void main(String[] args) {

    SSLSocket sslSocket = null;
    SSLClientSocket mSSLClientSocket = new SSLClientSocket(args[0], Integer.parseInt(args[1]));
    if(mSSLClientSocket.checkAndAddCertificates()) {
      sslSocket = mSSLClientSocket.getSSLSocket();
    }
    else {
      return;
    }

    try {
      BufferedReader br = new BufferedReader(new InputStreamReader(sslSocket.getInputStream()));
      PrintWriter pw = new PrintWriter(sslSocket.getOutputStream());
      pw.println("Initiating connection from the client");
      System.out.println("\033[1m\033[32mSuccessfully connected to secure server\033[0m");
      pw.flush();
      br.readLine();

      Aio thermometer = new Aio(0);

      int R0 = 100000;
      int B = 4275;
      int temperature;
      double R;

      Calendar cal;
      SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss");
      String formattedString = "", serverResponse = "";
      upm_jhd1313m1.Jhd1313m1 lcd = new upm_jhd1313m1.Jhd1313m1(1, 0x3E, 0x62);

      //Configuration variables
      boolean fahrenheit = true;
      int period = 1000;
      boolean running = true;

      while(true){
        if(running){
          cal = Calendar.getInstance();
          lcd.clear();
          temperature = (int) thermometer.read();
          R = (1023.0 / (double) temperature - 1.0) * R0;
          temperature = (int) (1.0/(Math.log(R/R0)/B+1/298.15)-273.15);
          if(fahrenheit) temperature = 9 * temperature / 5 + 32;

          formattedString = "" + sdf.format(cal.getTime()) + " " + temperature;

          lcd.setCursor(0,0);
          lcd.write("" + sdf.format(cal.getTime()) + " " + temperature + (fahrenheit ? "F" : "C"));
          lcd.setCursor(1, 0);
          lcd.write("Running");

          pw.println(formattedString);
          System.out.println(formattedString);
          pw.flush();
        }
        else{
          lcd.setCursor(1, 0);
          lcd.write("Paused ");
        }

        serverResponse = br.readLine().trim();

        //Process commands
        if(serverResponse.indexOf("SCALE=") != -1){
          if(serverResponse.equals("SCALE=F")) {
            System.out.println("\033[1m\033[34mChanging temperature to degrees Fahrenheit\033[0m");
            fahrenheit = true;
          }
          else if(serverResponse.equals("SCALE=C")) {
            System.out.println("\033[1m\033[34mChanging temperature to degrees Celsius\033[0m");
            fahrenheit = false;
          }
        }
        else if(serverResponse.indexOf("PERIOD=") != -1){
          period = (int) (1000 * Double.parseDouble("" + serverResponse.charAt(serverResponse.indexOf("PERIOD=") + 7)));
          System.out.println("\033[1m\033[34mChanging period to " + period + " ms\033[0m");
        }
        else if(serverResponse.equals("OFF")){
          System.out.println("\033[1m\033[31mSHUTTING DOWN\033[0m");
          break;
        }
        else if(serverResponse.equals("START")){
          if(!running) System.out.println("\033[1m\033[32mStarted monitoring sensors\033[0m");
          running = true;
        }
        else if(serverResponse.equals("STOP")){
          if(running) System.out.println("\033[1m\033[31mStopped monitoring sensors\033[0m");
          running = false;
        }

        if(serverResponse.length() > 0) System.out.println(serverResponse);

        Thread.sleep(period);
      }


    } catch (Exception e) {
      e.printStackTrace();
    }
  }

}
