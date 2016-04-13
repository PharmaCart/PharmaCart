#include <stdlib.h>
#include <unistd.h>
#include <mraa/gpio.h>
#include "lego_robot.h"
#include "gpio.h"

using namespace std;

int main(){
	initialize();

	mraa_pwm_write(pwm2, 1.0f);
	mraa_pwm_write(pwm1, 1.0f);

	brake_A();
	usleep(1000);
	brake_B();
}
