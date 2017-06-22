#include <stdio.h>
#include <unistd.h>
#include <mraa/gpio.h>
#include <mraa/aio.h>
#include <mraa.h>

sig_atomic_t volatile run_flag = 1;

void do_when_interrupted(int sig) {
	if (sig == SIGINT)
		run_flag = 0;
}

int main(){
  mraa_gpio_context led, buzzer;
  mraa_aio_context mic;
  led = mraa_gpio_init(4);
  buzzer = mraa_gpio_init(3);
  uint16_t mic_value = 0;
  mic = mraa_aio_init(0);

  mraa_gpio_dir(led, MRAA_GPIO_OUT);
  mraa_gpio_dir(buzzer, MRAA_GPIO_OUT);

  while(1){
    mic_value = mraa_aio_read(mic);
    if(mic_value > 350){
      mraa_gpio_write(led, 1);
      mraa_gpio_write(buzzer, 1);
    }
    sleep(1);
  }

  mraa_gpio_close(led);
  mraa_gpio_close(buzzer);
  return 0;
}
