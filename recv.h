/*

This file is created by mitch camacho -- mittycamacho@gmail.com
April 21, 2016

It serves as the header file for the infrared receiver code

*/


#ifndef recv_hpp
#define recv_hpp




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





// IR Receive class initialization and declaration
class IR_receive
{
public:
    
    // Constructor and Destructor
    IR_receive(int pin);
    ~IR_receive();
    
    // Accessors
    bool get_msg_recvd();
    bool get_handler_registered();
    
    // Modifiers
    void push_recv_queue(uint8_t gpio_value);
    void set_msg_recvd(bool recvd);
    void set_handler_registered(bool regstrd);
    
    // Signal Functions
    int recv();
    void sig_handler_recv(int signum);
    
private:
    mraa_gpio_context gpio;	// declares gpio class
    bool msg_recvd;			// used for keeping track of received msgs
    bool handler_registered;	// used for confirmations
    queue<uint8_t> recv_queue; 	//queue for all of received bits
    queue<int> msg_bits;    // queue for information bits

    
};
