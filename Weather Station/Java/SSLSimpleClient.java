import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.UnknownHostException;

import javax.net.ssl.SSLSocket;


public class SSLSimpleClient {

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

                    System.out.println(br.readLine() + " was received");
                    sslSocket.close();
                } catch (Exception e) {
                        e.printStackTrace();
                }
        }

}
