import mraa.*;

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
    
    try {

      Aio mic = new Aio(0);
      int soundLevel, soundThreshold = 250;

      while(true){
        soundLevel = (int) mic.read();

        //Turn alarm on
        if(soundLevel > soundThreshold){
          setAlarm(1);
          Thread.sleep(3000);
          setAlarm(0);
        }

        Thread.sleep(100);
      }


    } catch (Exception e) {
      e.printStackTrace();
    }
  }

}
