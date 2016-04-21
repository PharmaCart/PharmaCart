//
//  testtotal.cpp
//  Pharmacart_receiver
//
//  Created by Mitch Camacho on 2/29/16.
//  Copyright © 2016 Mitch Camacho. All rights reserved.
//


#include <stdio.h>
#include <queue>
#include <algorithm>
#include <signal.h>
#include "gpio.h"
#include "lego_robot.h"
#include "PharmaCart_command_mgmt.h"
#include "parmaCart.h"
#include <string>
#include "SquareSetup.h"
#include <mraa/gpio.h>
#include <time.h>

using namespace std;


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//
// Main Functions
//
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////


/////// Global Variable Initialization  /////////////

// For detecting IR signal
static volatile int run_flag_IR = 1;
// For detecting ctr C
sig_atomic_t volatile run_flag = 1;

clock_t start_wait_time;


// Initialization of the cart
parmaCart cart(26);

// Initialization of the beacons
beacon b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12;

/////// End of Global Variable Initialization  /////////////



/////// Global Function Initialization  ////////////


bool wait()
{
	if (clock() > (start_wait_time + (2 * CLOCKS_PER_SEC)))
	{
		return true;
	}
	else
	return false;
}

// handler for GPIO interrupt
void do_when_interruptedIR(void *)
{
    run_flag_IR = 0;
//    cout << "IR Interrupt Called" << endl;
}

// handler for stop running robot
void do_when_interrupted(int sig)
{
    
    brake_A();
    brake_B();
    usleep(500000);
    run_flag=0;
}


int main(int argc, char * argv[])
{
    
    ///////////////////////////////////
    // Local Variable Declaration and Initialization
    ///////////////////////////////////
    // Decare a clock for timing
    start_wait_time = clock();
    bool time_to_read;
    bool stopped;
    int ID = 0;
    clock_t stoptime;
    bool starter = true;
    
    parmaCart cart(26);
    
    /////  Local Function Initialization and Setup  //////////
    
    // Set up the mapping layout
    squareSetup(b0, b1, b2, b3, b4, b5, b6, b7, cart);
    
    // Initialize robot mraa ports
    initialize();
    
    // Initailize Line Following IR ports
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
    
    // Set Duty cycle for mraa pwm pins
    mraa_pwm_write(pwm2, 1.0f);
    mraa_pwm_write(pwm1, 1.0f);
    
    // Initialize an Interrupt Service Routine gor GPIO
    mraa_gpio_isr(cart.receiver, MRAA_GPIO_EDGE_RISING, &do_when_interruptedIR, NULL);
    
    // Create signal read for ctrl C
    signal(SIGINT, do_when_interrupted);
    
    string ip_address = argv[1];
    Robot_Communication r_com(ip_address);
    // requesting commands
    string current_command;
    int last_beacon;
    while(run_flag)
    {
        current_command = r_com.get_next_command();
        while (run_flag)//(command != "end of file")
        {
            
            //do the command
            stopped = true;
            int i_count = 0;
            
            while ( run_flag)//not at command destination )
            {
                if (stopped)
                {
                    reverse_A();
                    reverse_B();
                    stopped = false;
                }
                
                //drive toward command destination
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
                if(!valueR && !valueL)
                    traveled +=20;
                if(i_count == 5){
                    //printf("%d-%d-%d\n", counterL, counterM, counterR);
                    i_count = 0;
                    off = offLine(counterL, counterR);
                    if(off)
                        turn(counterL, counterR);
                    counterL = 0;
                    counterM = 0;
                    counterR = 0;
                }
                
                
                
                
                // This code is below
                /*
                 if beacon detected
                 {
                 get beacon id
                 **          string beacon_communicate = client_communicate + " " + to_string(locale_id);
                 **          status = system(beacon_communicate.c_str());
                 **          //IMPORTANT: ALSO SEND LOCATION INFORMATION (ME)
                 **          //UPDATE ROBOT LOCATION (DAVID)
                 
                 }
                 */
                
                // If IR signal gets sensed then we will stop the robot and wait for down time to signify the beginning of a message
                time_to_read = false;
                if (!run_flag_IR && wait()) //&& ((clock() > (stoptime + (.5 * CLOCKS_PER_SEC))) || starter))
                {
                    cout << "reading" << endl;
                    brake_A();
                    brake_B();
                    stopped = true;
                    run_flag_IR = 1;		// reset the ISR
                    time_to_read = true;
                    //cout << "First Rising Edge Detected" << endl;
                    starter = false;
                    
                }

		// Setup/Initialize the receiver for IR
                int gpio_Pin = cart.get_pin();
                IR_receive receiver(gpio_Pin);
                
                // declare beacon ID variable
		int beacID;
                
            	if(time_to_read)
            	{
            		beacID = receiver.recv();
            		start_wait_time = clock();
            	}
                
                
                
                
                
                
                
                
                
                
                /*
                
                
                // Initialize a start time to the clock
                init_time = clock();
                
                // Start while loop for read time
                // Note: read for twice the period because of the down time
                // At this point the robot is stopped
                
                bool cont = false;
                // Enter loop if we need to read IR signal
                while(time_to_read)
                {
                    cout << "Received Rising Edge" << endl;
                    init_time = clock();
                    // Start a timer
                    while (clock() < (init_time + (0.01 * CLOCKS_PER_SEC)))
                    {
                        // If we get another signal continue through the loop, if we dont and the clock expires then we will break
                        if(!run_flag_IR)
                        {
                            cout << "Edge detected too soon" << endl;
                            cont = true;
                            run_flag_IR = 1;
                            break;
                        }
                        
                    }
                    if (cont)
                    {
                        cont = false;
                        continue;
                    }
                    else
                    {
                        cout << "Received Dead Time" << endl;
                        break;
                    }
                    
                }
                
                // Look For first signal after the down time
                while(time_to_read)
                {
                    // If IR signal gets sensed then we will break from the loop and wait for down time to signify the beginning of a message
                    if (!run_flag_IR)
                    {
                        cout << " Recieved Start of message" << endl;
                        cart.rising_count++;
                        run_flag_IR = 1;		// reset the ISR
                        break;			// get out of loop that looks for signal
                    }
                }
                
                init_time = clock();
                
                // Start receving informative IR signal
                while(time_to_read && clock() < (init_time + (2 * CLOCKS_PER_SEC)))
                {
                    if(!run_flag_IR)
                    {
                        //cout << "Counting" << endl;
                        cart.rising_count++;
                        run_flag_IR = 1;
                    }
                }
                
                if (time_to_read)
                {
                    printf("Rising Count = %d\n", cart.rising_count);
                    ID = cart.get_beacon_ID(cart.rising_count);
                    printf("This is Beacon #%d\n", ID);
                    // send updated location to server
                    r_com.update_location(ID);
                    printf("RunFlagIR = %d\n", run_flag_IR);
                    cart.rising_count = 0;
                    stoptime = clock();
                    if (ID == 3)
                    {
                        brake_A();
                        brake_B();
                    }
                }
                
                */
                
                usleep(10000);
                
            }
            // get a new command
            current_command = r_com.get_next_command();
        }
        // need more commands, communicate with server
        r_com.request_commands();
    }
}



























