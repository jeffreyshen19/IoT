#include <stdio.h>
#include <unistd.h>
#include <mraa/aio.h>
#include <mraa.h>

int main(){
  mraa_aio_context thermometer;
  uint16_t thermometer_value = 0;
  thermometer = mraa_aio_init(0);
  while(1){
    thermometer_value = mraa_aio_read(thermometer);
    printf("Temperature is: %d\n", thermometer_value);
    usleep(100000);
  }

  return 0;
}
