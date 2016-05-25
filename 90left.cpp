#include <stdio.h>
#include <signal.h>
#include "gpio.h"
#include "lego_robot.h"
#include <mraa/gpio.h>

using namespace std;

sig_atomic_t volatile run_flag = 1;

void do_when_interrupted(int sig)
{

	brake_A();
	brake_B();
	usleep(500000);
	run_flag = 0;
}

int main()
{
	initialize();

	GPIO photoR = GPIO45;
	GPIO photoM = GPIO46;
	GPIO photoL = GPIO44;
	
	mraa_pwm_write(pwm2, 1.0f);
	mraa_pwm_write(pwm1, 1.0f);

	signal(SIGINT, do_when_interrupted);
	
	gpio_init(photoR);
	gpio_init(photoM);
	gpio_init(photoL);

	gpio_direction(photoL, INPUT);
	gpio_direction(photoM, INPUT);
	gpio_direction(photoR, INPUT);


	forward_A();
	usleep(1000);
	reverse_B();
	usleep(1200000);
	//while(!gpio_read(photoL) || !gpio_read(photoM) || !gpio_read(photoR)){}
	//while(gpio_read(photoL)){}
	brake_A();
	brake_B();
}

