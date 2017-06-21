#include <stdio.h>
#include <unistd.h>
#include <mraa/gpio.h>
#include <mraa/aio.h>
#include <mraa.h>

int main(){
  mraa_gpio_context led, mic;
  led = mraa_gpio_init(4);
  uint16_t mic_value = 0;
  mic = mraa_aio_init(0);

  mraa_gpio_dir(led, MRAA_GPIO_OUT);

  while(1){
    mic_value = mraa_aio_read(mic);
    if(mic_value > 200){
      mraa_gpio_write(led, 1);
    }
    sleep(1);
  }

  mraa_gpio_close(led);
  return 0;
}
