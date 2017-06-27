import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
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
    boolean receiving = true, running = true;
    ArrayList<String> messages = new ArrayList<>();
    int sleepTime = 0;
    try {
      while (running) {
        BufferedReader br = new BufferedReader(new InputStreamReader(sock.getInputStream()));
        PrintWriter pw = new PrintWriter(sock.getOutputStream());
        if (receiving) {
          String data = br.readLine();
          messages.add(data);
          System.out.println(data + " is echoed");
        }

        BufferedReader msgTaker = new BufferedReader(new InputStreamReader(System.in));

        try {
          Thread.sleep(sleepTime);
        } catch (Exception e) {
          System.out.println("couldnt sleep");
        }

        long end=System.currentTimeMillis()+100;
        String message = "";
        while((System.currentTimeMillis()<end)) {
          if (msgTaker.ready())
            message += msgTaker.readLine();
        }
        if (message.equals("")) {
          System.out.println("No input");
        }
        else {
          System.out.println("The message is " + message);
        }
        if (message.equals("")) {
          pw.println(message);
        }
        else {
          if (message.equals("STOP")) {
            receiving = false;
          }
          else if (message.equals("OFF")) {
            running = false;
          }
          else if (message.equals("START")) {
            receiving = true;
          }
          else if (message.contains("PERIOD=")) {
            sleepTime = Integer.parseInt(message.substring(message.indexOf("=") + 1))*1000;
          }
          messages.add(message);
          pw.println(message);
        }
        pw.flush();
      }

      sock.close();

      try{
        PrintWriter writer = new PrintWriter("log.txt", "UTF-8");
        for (int i = 0; i < messages.size(); i++){
          writer.println(messages.get(i));
        }
        writer.close();
      } catch (IOException e) {
        // do something
      }
    } catch (IOException ioe) {
      // Client disconnected
    }
    System.out.println("got here");
  }
}
