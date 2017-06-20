#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <mraa/aio.h>
#include <mraa.h>
sig_atomic_t volatile run_flag = 1;

void do_when_interrupted(int sig) {
	if (sig == SIGINT)
		run_flag = 0;
}

int main() {
	mraa_aio_context adc_a0;
	uint16_t adc_value = 0;
	float adc_value_float = 0.0;
	mraa_result_t r = MRAA_SUCCESS;

	adc_a0 = mraa_aio_init(0);

	if (adc_a0 == NULL) {
		return 1;
	}

	signal(SIGINT, do_when_interrupted);
	
	int values [4];
	int i;
	for (i = 0; i < 4; i++) {
		sleep(1);
		printf("taking input now\n");
		values[i] = mraa_aio_read(adc_a0);
		//adc_value_float = mraa_aio_read_float(adc_a0);
	}
	int j;
	int password [4];
	for (j = 0; j < 4; j++) {
		if (values[j] > 150) {
			password[j] = 1;
		}
		else {
			password[j] = 0;
		}
		printf("here is value %d: %d\n", j, password[j]);
	}
	
	
	
	r = mraa_aio_close(adc_a0);
	if (r != MRAA_SUCCESS) {
		mraa_result_print(r);
	}
	return r;
}



