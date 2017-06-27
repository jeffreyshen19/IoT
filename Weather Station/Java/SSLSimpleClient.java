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

public class SSLSimpleClient {

  static {
	  System.out.println("testing");
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
      System.out.println("\033[1m\033[32mSuccessfully connected to secure server\x1B[0m");
      pw.flush();
      br.readLine();

      Aio thermometer = new Aio(0);

      int R0 = 100000;
      int B = 4275;
      int temperature;
      double R;

      Calendar cal = Calendar.getInstance();
      SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss");
      String formattedString = "", serverResponse = "";

      //Configuration variables
      boolean fahrenheit = true;
      int period = 1000;
      boolean running = true;

      while(true){
        if(running){
          temperature = (int) thermometer.read();
          R = (1023.0 / (double) temperature - 1.0) * R0;
          temperature = (int) (1.0/(Math.log(R/R0)/B+1/298.15)-273.15);
          if(fahrenheit) temperature = 9 * temperature / 5 + 32;

          formattedString = "" + sdf.format(cal.getTime()) + " " + temperature;

          pw.println(formattedString);
          System.out.println(formattedString);
          pw.flush();
        }

        serverResponse = br.readLine().trim();

        //Process commands
        if(serverResponse.indexOf("SCALE=") != -1){
          if(serverResponse.equals("SCALE=F")) fahrenheit = true;
          else if(serverResponse.equals("SCALE=C")) fahrenheit = false;
        }
        else if(serverResponse.indexOf("PERIOD=") != -1){
          period = (int) (1000 * Double.parseDouble("" + serverResponse.charAt(serverResponse.indexOf("PERIOD=") + 7)));
        }
        else if(serverResponse.equals("OFF")){
          System.out.println("\033[1m\033[31mSHUTTING DOWN\x1B[0m");
          break;
        }
        else if(serverResponse.equals("START")){
          if(!running) System.out.println("\033[1m\033[32mStarted monitoring sensors\x1B[0m");
          running = true;
        }
        else if(serverResponse.equals("STOP")){
          if(running) System.out.println("\033[1m\033[31mStopped monitoring sensors\x1B[0m");
          running = false;
        }

        System.out.println(serverResponse);

        Thread.sleep(period);
      }


    } catch (Exception e) {
      e.printStackTrace();
    }
  }

}
