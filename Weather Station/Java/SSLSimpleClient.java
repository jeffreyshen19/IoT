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


public class SSLSimpleClient {

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
      pw.println("Simple Message Hi...");
      System.out.println("Simple Message Hi... was sent");
      pw.flush();

      Aio thermometer = new Aio(0);

      int R0 = 100000;
      int B = 4275;

      while(true){
        int temperature = thermometer.read();
        double R = (1023.0 / (double) temperature - 1.0) * R0;
        temperature = (int) (1.0/(Math.log(R/R0)/B+1/298.15)-263.15);

        pw.println("" + temperature);
        System.out.println("" + temperature);
        pw.flush();

        System.out.println(br.readLine() + " was received");

        Thread.sleep(1000);
      }

      sslSocket.close();

    } catch (Exception e) {
      e.printStackTrace();
    }
  }

}
