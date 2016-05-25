#include <stdio.h>
#include <signal.h>
#include "gpio.h"
#include "lego_robot.h"
#include <mraa/gpio.h>
#include <iostream>

#include <queue>
#include <algorithm>
#include "parmaCart.h"
#include <string>
//#include "SquareSetup.h"
#include <time.h>
#include "recv.h"

using namespace std;

parmaCart cart(26);

// Initialization of the beacons
//beacon b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12;

clock_t start_wait_time;
bool time_to_read;
double wait_mult = .2;


int turn(int L, int R, bool turn);
int offLine(int L, int R);

sig_atomic_t volatile run_flag = 1;

// For detecting IR signal
static volatile int run_flag_IR = 1;

static volatile int run_flag_wait = 0;



void wait()
{

   //cout << "Start wait time:" <<(double)(start_wait_time + wait_mult* CLOCKS_PER_SEC) << endl;
   //cout << "Clock: " << (double)clock() << endl;

    if ((double)clock() > (double)(start_wait_time + (wait_mult * CLOCKS_PER_SEC)))
    {
        //cout << "Done waiting" << endl;
		run_flag_wait = 1;
    }
    else
    {
        run_flag_wait = 0;
    }
					// wait for less time on the first run-through, afterwards have longer pause
					// wait multiplier is initially .2, changed in if(wait()) clause below 
}


// handler for GPIO interrupt
void do_when_interruptedIR(void *)
{
    //cout << "ISR";//<< endl;
    if (1)
    {
        run_flag_IR = 0;  // This is now called in the main function to prevent a missing the intrupt
        //cout << "IR Interrupt Called" << endl << endl <<endl;
    }

}


void do_when_interrupted(int sig)
{
    
    {
        brake_A();
        brake_B();
        usleep(500000);
        run_flag = 0;
    }
}




int main()
{
    ///////////////////////////////////
    // Local Variable Declaration and Initialization
    ///////////////////////////////////
    // Decare a clock for timing
    
    int ID = 0;
    start_wait_time = clock();

    
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
    int junction = 0;			//if junction = 0 - straight, 1 = left, 2 = right
    bool atJunction, stopped;
    bool turning = false;
    
    mraa_pwm_write(pwm2, 0.5f);
    mraa_pwm_write(pwm1, 0.5f);
	
    signal(SIGINT, do_when_interrupted);
    
    // Initialize an Interrupt Service Routine gor GPIO
    mraa_gpio_isr(cart.receiver, MRAA_GPIO_EDGE_RISING, &do_when_interruptedIR, NULL);
    
    
    junction = 0;
    atJunction = false;
    while (run_flag)
    {
        stopped = true;
        int i_count = 0;
        while (run_flag)
        {
            if (stopped)
            {
				
				cout << "move" << endl;
                reverse_A();
                reverse_B();
                stopped = false;
            }
            /*if (!gpio_read(photoL) && !gpio_read(photoM) && !gpio_read(photoR))				//when passing a tick mark
             {
             if (turning){																//if already turning, change to straight
             //printf("turning to straight\n");
             turning = false;
             junction = 0;
             atJunction = false;
             }
             else{																		//if going straight
             if (atJunction){															//and at a junction
             if (junction != 0){															//w/ command to keep left or right
             turning = true;															//change to turning
             //printf("straight to turning junction\n");
             }
             else{
             usleep(500000);															//if at a junction & going straight, sleep so you miss turn
             //printf("junction straight\n");
             }
             }else{																		//if not at junction
             turning = true;															//change to turning
             //printf("straight to turning\n");
             }
             }
             if(junction = 0)
             turning = false;
             else
             turning = true;*/
            //while (!gpio_read(photoL) && !gpio_read(photoM) && !gpio_read(photoR)){}
			if (!gpio_read(photoL) && !gpio_read(photoM) && !gpio_read(photoR)){
				counterL += 1;
				counterM += 1;
				counterR += 1;
			}
			//cout << "line follow" << endl;	

            
            i_count++;
            //cout << "i_count = " << i_count << endl;
            //cout << "mraa_gpio_35" << mraa_gpio_read(cart.receiver) << endl;
            valueL = gpio_read(photoL);
            valueM = gpio_read(photoM);
            valueR = gpio_read(photoR);
            if (junction == 2)
                counterL += 1;
            else
                counterL += valueL;
            counterM += valueM;
            if (junction == 1)
                counterR += 1;
            else
                counterR += valueR;
            //printf("%d-%d-%d\n", valueL, valueM, valueR);
            /*if (!valueR && !valueL)
             traveled += 20;*/
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
            /*
             
             
             IR Integration
             
             
             */
			
            //cout << "Rising Edge Detected = " << !run_flag_IR << endl;
            wait();
            time_to_read = false;
            //cout << "run_flag_IR =" << run_flag_IR << endl;
            //cout << "run_flag_wait = " << run_flag_wait <<  endl;
            if (!run_flag_IR && run_flag_wait)
            {
                start_wait_time = clock();      // restart the start wait time to wait another period of time
                //cout << "reading" << endl;
                brake_A();
                brake_B();
                stopped = true;
                run_flag_IR = 1;    // reset the ISR
                run_flag_wait = 0;
                time_to_read = true;
                cout << "First Rising Edge Detected" << endl;
                mraa_gpio_isr_exit(cart.receiver);
                
            }
            
            // Setup/Initialize the receiver for IR
            int gpio_Pin = cart.get_pin();
            IR_receive receiver(26);
            
            // declare beacon ID variable
            int beacID;
            
            if(time_to_read)
            {
                cout << "trying to receive" << endl;
                beacID = receiver.recv();   // read IR signal until something is received
				/* 
				if ( beacID == 69);
				{
					continue;
				}
				*/
				usleep(1000000);
				cout<< "Beacon Read is Beacon #"<< beacID << endl;
                start_wait_time = clock();      // restart the start wait time to wait another period of time
                time_to_read = false;
                cout << "Done Reading" << endl;
                mraa_gpio_isr(cart.receiver, MRAA_GPIO_EDGE_RISING, &do_when_interruptedIR, NULL);
                run_flag_IR = 1;
                run_flag_wait = 0;
				if (beacID == 69)
					wait_mult = 1;
				else
					wait_mult = 3;
                
            }
            
        }
    }
	brake_A();
	brake_B();
	cout << "end of main" << endl;
    
}


