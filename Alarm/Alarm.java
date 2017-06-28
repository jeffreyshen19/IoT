import mraa.*;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.UnknownHostException;
import javax.net.ssl.SSLSocket;

public class Alarm {

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

  public static void setAlarm(int status){
    Gpio led = new Gpio(3);
    Gpio buzzer = new Gpio(4);
    led.dir(Dir.DIR_OUT);
    buzzer.dir(Dir.DIR_OUT);

    led.write(status);
    buzzer.write(status);
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

      Aio mic = new Aio(0);
      int soundLevel, soundThreshold = 300;

      String formattedString = "";
      Calendar cal;
      SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss");

      BufferedReader br = new BufferedReader(new InputStreamReader(sslSocket.getInputStream()));
      PrintWriter pw = new PrintWriter(sslSocket.getOutputStream());
      pw.println("Initiating connection from the client");
      System.out.println("\033[1m\033[32mSuccessfully connected to secure server\033[0m");
      pw.flush();
      br.readLine();

      while(true){
        soundLevel = (int) mic.read();

        //Turn alarm on
        if(soundLevel > soundThreshold){
          cal = Calendar.getInstance();
          formattedString = "" + sdf.format(cal.getTime()) + " " + soundLevel;
          setAlarm(1);
          pw.println(formattedString);
          System.out.println("\033[1m\033[31mBREAK-IN DETECTED. SENDING TO SERVER\033[0m");
          System.out.println(formattedString);
          pw.flush();
          br.readLine();
          setAlarm(0);
        }

        Thread.sleep(100);
      }


    } catch (Exception e) {
      e.printStackTrace();
    }
  }

}
