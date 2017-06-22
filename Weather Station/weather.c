#include <stdio.h>
#include <unistd.h>
#include <mraa/aio.h>
#include <mraa.h>

int main(){
  int R0 = 100000;
  int B = 4275
  mraa_aio_context thermometer;
  float thermometer_value = 0;
  thermometer = mraa_aio_init(0);
  
  while(1){
    thermometer_value = 1.0/(log(1023.0/mraa_aio_readFloat(thermometer)-1.0)/B+1/298.15)-273.15;
    printf("Temperature is: %d\n", thermometer_value);
    usleep(100000);
  }

  return 0;
}
