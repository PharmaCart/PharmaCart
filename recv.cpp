//
//  recv.cpp
//
//
//  Created by Mitch Camacho on 4/8/16.
//
//

#include "recv.h"


/*
 IR_receive.cpp
 
 
 Created by Mitch Camacho on 4/3/16.
 
 
 NOTE:
 Write now the bit processing is programmed with the expectation the the modulation is already accounted for.
 This may not be the case (we might be getting double numbers)
 
 
 IR reading options of cart integeration:
 
 
 */


#include <mraa/gpio.h>	//gpio
#include <iostream>		// for cout cin
#include <sys/time.h>	// for timer (interrupt service routine)
#include <signal.h>		//
#include <cstdlib>		//
#include <bitset>		// for bit set and converting to strings
#include <string>		// for strings
#include <queue> 		// for queue
#include <stdint.h>		// for uint_t (specific bit sizes)
#include <set>			// for set
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>



using namespace std;

// Definitions
#define INTERVAL 8	// For defining interval of lowest milliseconds

// Constants
const int max_msg_size = 5;
mraa_gpio_context gpio;	// declares gpio class
queue<uint8_t> recv_queue; 	//queue for all of received bits
queue<int> msg_bits;    // queue for information bits
bool msg_recvd;			// used for keeping track of received msgs
int looping = 0;

/////////////////////////////////////////////////////////////////////
// Non-member Signal Handler Functions
/////////////////////////////////////////////////////////////////////

// Empty Signal handle function
void sig_handler_void(int signum){
}


// Auto function to push to queue
void push_recv_queue(uint8_t gpio_value)
{
    recv_queue.push(gpio_value);
}


// This interrupt is triggerred by the itimer
void intrrupt_handler(int signum)
{
    signal(SIGALRM, SIG_DFL);
}


// This function is called at the timer when the signal handler reads the gpio value
void sig_handler_recv(int signum){
    
    if(!msg_recvd)
    {
        push_recv_queue((uint8_t) !mraa_gpio_read(gpio));
        cout << !mraa_gpio_read(gpio);
        
        if( looping == 26)
        {
            looping  = 0;
            cout << endl;
        }
        looping++;
    }
}

///////////////////////////////////////////////////////
// General non-member functions
///////////////////////////////////////////////////////

// Converts a binary queue to integer value
int BinaryToInt(queue<int> binary_queue)
{
    int size = binary_queue.size();
    int current_bit;
    int integer = 0;
    int add = 0;
    while(size > 1)
    {
        size = binary_queue.size();
        current_bit = binary_queue.front();
        binary_queue.pop();
        
        if (current_bit == 1)
        {
            integer = integer + (exp2(size-1));
        }
    }
    return integer;
}




// Constructor
IR_receive::IR_receive(int pin)
{
    // Mraa intializations
    gpio = mraa_gpio_init(pin);		// intitalize gpio as argument pin
    mraa_gpio_dir(gpio, MRAA_GPIO_IN);	// Set GPIO to input setting
    
    // Memeber state initializations
    msg_recvd = false;
    handler_registered = false;
}



// IR_device destructor (close gpio ports)
IR_receive::~IR_receive()
{
    mraa_gpio_close(gpio); 	// close gpio port
}


// Accessor member functions
bool IR_receive::get_msg_recvd()
{
    return msg_recvd;
}

bool IR_receive::get_handler_registered()
{
    return handler_registered;
}


// Mutator member functions
void IR_receive::set_msg_recvd(bool recvd)
{
    msg_recvd = recvd;
}

void IR_receive::set_handler_registered(bool regstrd)
{
    handler_registered = regstrd;
}



int IR_receive::recv()
{
    /////////////////////////////////////////
    // set local variables
    /////////////////////////////////////////
    
    // Initialize header states
    bool header_received = false;
    bool header_zero_received = false;
    bool header_one_received = false;
    
    // initialize counter variables
    int bit_counter=0;
    int header_counter_zero=0;
    int header_counter_one=0;
    int consecutive_zeros = 0;
    
    int last_bit = 1;
    
    // msg variables
    const int msg_length = 6;
    msg_recvd = false;
    
    // Bit queue
    set<int> recv_byte_set;
    
    // return integer
    int ret_int;
    
    
    // Enable Signal Process Programming
    signal(SIGINT, intrrupt_handler);
    
    
    //Register the timer to receive the signal bits at proper timing
    cout << "registering timer" << endl;
    struct itimerval it_val;  // set itimer
    if(!handler_registered)
    {
        if (signal(SIGALRM, (void (*)(int)) sig_handler_recv) == SIG_ERR)
        {
            perror("Unable to catch SIGALRM");
            return 0;
        }
        handler_registered = true;
    }
    
    // Set the timer values
    it_val.it_value.tv_sec = INTERVAL/1000;
    it_val.it_value.tv_usec = (INTERVAL*1000) % 1000000;
    it_val.it_interval = it_val.it_value;
    
    if (setitimer(ITIMER_REAL, &it_val, NULL) == -1)
    {
        perror("error calling setitimer()");
        return 0;
    }
    cout << "registering timer finished..." << endl;
    
    // The following code will allow the code to check for the received signal
    while(!msg_recvd)	// wait until a signal is received
    {
        while(!recv_queue.empty())	// go through queue of message until the the queue is empty
        {
            uint8_t reading = recv_queue.front();	// get bit
            recv_queue.pop();	// pop it off the queue
            if(header_received)	// if the header has been received start to process the address and data
            {
                msg_bits.push((int) reading); // Add the info bit to the queue
                cout << (int) reading << endl;
                bit_counter++;  // Incriment bit counter to keep track of bits being received
                
                if (bit_counter >= msg_length) //Info bits received
                {
                    // Will break out of reading loop and stop collecting bits
                    msg_recvd = true;
                    break;
                }
            }
            
            ////////////////////////////
            // This will attempt to find the header of the message
            ///////////////////////////
            
            else	// Header is not received (read the header 0's and 1's)
            {
                //check 10 consecutive '0's and '1's
                if(reading == 1)    // reading header 1's
                {
                    if(last_bit == 1)
                        header_counter_zero++;
                    else
                        header_counter_zero = 1;
                    if(header_counter_zero == 10)
                        header_zero_received = true;
                    consecutive_zeros = 0;
                }
                else
                { // reading header 0's
                    if(last_bit == 1 && header_zero_received)
                    {
                        header_counter_one = 1;
                    }
                    else if(last_bit == 0 && header_zero_received)
                    {
                        header_counter_one++;
                    }
                    else
                    {
                        header_zero_received = false;
                        if (last_bit == 0)
                        {
                            consecutive_zeros++;
                        }
                        
                        if (consecutive_zeros >250)
                        {
                            signal(SIGALRM, (void (*)(int)) sig_handler_void);
                            return 69;
                        }
                    }
                    
                    if(header_counter_one == 10)
                    {
                        cout << "Header detected\n";
                        header_received = true;
                    }
                }
                last_bit = reading;
            }
        }
    }
    
    // Set signal handler to void function
    signal(SIGALRM, (void (*)(int)) sig_handler_void);
    // Set the timer values
    getitimer(ITIMER_REAL, &it_val);
    it_val.it_value.tv_sec = 0;
    it_val.it_value.tv_usec = 0;
    
    if (setitimer(ITIMER_REAL, &it_val, NULL) == -1)
    {
        perror("error calling setitimer()");
        return 0;
    }
    
    
    
    
    ret_int = BinaryToInt(msg_bits);
    
    // Empty the contents of all queues
    while (!recv_queue.empty())
    {
        recv_queue.pop();
    }
    while (!msg_bits.empty())
    {
        msg_bits.pop();
    }
    
    return ret_int;
    
}

/*
 int main()
 {
 IR_receive receiver(26);
 
 
 cout << endl << "Beacon Number: " << (int)receiver.recv() << endl;
 }
 
 
 
 */















