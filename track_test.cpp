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

	gpio_init(photoR);
	gpio_init(photoM);
	gpio_init(photoL);

	gpio_direction(photoL, INPUT);
	gpio_direction(photoM, INPUT);
	gpio_direction(photoR, INPUT);

	int off, traveled = 0;
	int valueL, valueM, valueR, counterL = 0, counterM = 0, counterR = 0;
	bool stopped;
	bool turning = false;

	mraa_pwm_write(pwm2, 1.0f);
	mraa_pwm_write(pwm1, 1.0f);

	signal(SIGINT, do_when_interrupted);

	while (run_flag)
	{
		stopped = true;
		int i_count = 0;
		while (run_flag)
		{
			if (stopped)
			{
				reverse_A();
				reverse_B();
				stopped = false;
			}
			if (!gpio_read(photoL) && !gpio_read(photoM) && !gpio_read(photoR))
			{
				if (turning)
					turning = false;
				else
					turning = true;
				while (!gpio_read(photoL) && !gpio_read(photoM) && !gpio_read(photoR)){}
			}
			i_count++;
			//cout << "i_count = " << i_count << endl;
			//cout << "mraa_gpio_35" << mraa_gpio_read(cart.receiver) << endl;
			valueL = gpio_read(photoL);
			valueM = gpio_read(photoM);
			valueR = gpio_read(photoR);
			counterL += valueL;
			counterM += valueM;
			counterR += valueR;
			//printf("%d-%d-%d\n", valueL, valueM, valueR);
			if (!valueR && !valueL)
				traveled += 20;
			if (i_count == 5){
				//printf("%d-%d-%d\n", counterL, counterM, counterR);
				i_count = 0;
				off = offLine(counterL, counterR);
				if (off)
					turn(counterL, counterR, turning);
				counterL = 0;
				counterM = 0;
				counterR = 0;
			}
			usleep(10000);

		}
	}

}