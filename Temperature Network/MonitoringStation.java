import mraa.*;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.UnknownHostException;
import javax.net.ssl.SSLSocket;

public class MonitoringStation{

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

      /*Aio mic = new Aio(0);
      int soundLevel, soundThreshold = 300;

      String formattedString = "", serverResponse = "";
      Calendar cal;
      SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss");

      BufferedReader br = new BufferedReader(new InputStreamReader(sslSocket.getInputStream()));
      PrintWriter pw = new PrintWriter(sslSocket.getOutputStream());

      boolean alarmEngaged = true;*/

      upm_jhd1313m1.Jhd1313m1 lcd = new upm_jhd1313m1.Jhd1313m1(1, 0x3E, 0x62);
      lcd.setCursor(0,0);
      lcd.write("Hello World");
      lcd.setCursor(1,2);
      lcd.write("Hello World");



    } catch (Exception e) {
      e.printStackTrace();
    }
  }
}
