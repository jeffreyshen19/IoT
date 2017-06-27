import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

import javax.net.ServerSocketFactory;
import javax.net.ssl.SSLServerSocketFactory;

public class SSLSimpleServer extends Thread {

  public static void main(String[] args) throws Exception {
    ServerSocketFactory ssf = SSLServerSocketFactory.getDefault();
    ServerSocket ss = ssf.createServerSocket(9096);

    System.out.println("Ready...");
    while (true) {
      new SSLSimpleServer(ss.accept()).start();
    }
  }

  private Socket sock;

  public SSLSimpleServer(Socket s) {
    sock = s;
  }

  public void run() {
    try {
      BufferedReader br = new BufferedReader(new InputStreamReader(sock.getInputStream()));
      PrintWriter pw = new PrintWriter(sock.getOutputStream());

      String data = br.readLine();
	System.out.println(data + " is echoed");
      pw.println(data);
      pw.close();
      sock.close();
    } catch (IOException ioe) {
      // Client disconnected
    }
  }
}
