import java.net.*;
import java.io.*;
import javax.net.ssl.*;

public class Client {
  public static void main(String[] args) throws Exception {
    try {
      SSLSocketFactory factory = (SSLSocketFactory) SSLSocketFactory.getDefault();
      SSLSocket socket = (SSLSocket) factory.createSocket(args[0], args[1]);

      socket.startHandshake();

      PrintWriter out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(socket.getOutputStream())));

      out.println("GET / HTTP/1.0");
      out.println();
      out.flush();

      if (out.checkError()) System.out.println("SSLSocketClient:  java.io.PrintWriter error");

      /* read response */
      BufferedReader in = new BufferedReader(
      new InputStreamReader(
      socket.getInputStream()));

      String inputLine;
      while ((inputLine = in.readLine()) != null)
      System.out.println(inputLine);

      in.close();
      out.close();
      socket.close();

    }
    catch (Exception e) {
      e.printStackTrace();
    }
  }
}
