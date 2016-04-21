#include "recv.h"


// Constants
const int max_msg_size = 5;


// Non-member Functions

void sig_handler_void(int signum){
}






// This interrupt is triggerred by the itimer
void intrrupt_handler(int signum)
{
    signal(SIGALRM, SIG_DFL);
    looping = 0;
}




// Converts a binary queue to integer value
int BinaryToInt(queue<int> binary_queue)
{
    int size = binary_queue.size();
    int current_bit;
    int integer = 0;
    while(size > 1)
    {
        size = binary_queue.size();
        current_bit = binary_queue.front();
        binary_queue.pop();
    }
    return integer;
    
}




// Constructor
IR_receive::IR_receive(int pin)
{
    gpio = mraa_gpio_init(pin);		// intitalize gpio as argument pin
    mraa_gpio_dir(gpio, MRAA_GPIO_IN);	// Set GPIO to input setting
    msg_recvd = false;
    handler_registered = false;
}



// IR_device destructor (close gpio ports)
IR_receive::~IR_receive()
{
    mraa_gpio_close(gpio); 	// close gpio port
}

void IR_receive::push_recv_queue(uint8_t gpio_value)
{
    recv_queue.push(gpio_value);
}

bool IR_receive::get_msg_recvd()
{
    return msg_recvd;
}

// This function is called at the timer when the signal handler reads the gpio value
void IR_receive::sig_handler_recv(int signum){
    
    if(!msg_recvd)
    {
        push_recv_queue((uint8_t) !mraa_gpio_read(gpio));
        //recv_queue.push((uint8_t) !mraa_gpio_read(gpio));
        //cout << !mraa_gpio_read(gpio) << endl;
    }
}


void IR_receive::set_msg_recvd(bool recvd)
{
    msg_recvd = recvd;
}

bool IR_receive::get_handler_registered()
{
    return handler_registered;
}

void IR_receive::set_handler_registered(bool regstrd)
{
    handler_registered = regstrd;
}


/*
 This program will be used to receive an int from the sending IR_sensor function
 it will first set a timer to keep track of the signals then it will receive bits
 in a queue first the header than bits of data which will be converted to an int
 */


int IR_receive::recv()
{
    // set local variables
    bool header_received = false;
    bool header_zero_received = false;
    bool header_one_received = false;
    // bool addr_received = false;
    int bit_counter=0;		//keep track of bits received per message
    int header_counter_zero=0;
    int header_counter_one=0;
    int last_bit = 1;
    const int msg_length = 6;
    msg_recvd = false;
    set<int> recv_byte_set;
    int ret_int;
    
    // Enable Signal Process Programming
    signal(SIGINT, intrrupt_handler);
    
    /*
     Register the timer to receive the signal bits at proper timing
     */
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
    
    // Find and set the timer values
    it_val.it_value.tv_sec = INTERVAL/1000;
    it_val.it_value.tv_usec = (INTERVAL*1000) % 1000000;
    it_val.it_interval = it_val.it_value;
    
    if (setitimer(ITIMER_REAL, &it_val, NULL) == -1)
    {
        perror("error calling setitimer()");
        return 0;
    }
    
    cout << "registering timer finished..." << endl;
    
    
    /*
     The following code will allow the code to check for the received signal
     */
    
    while(!msg_recvd)	// wait until a signal is received
    {
        //cout << "msg not received";
        //cout << recv_queue.empty()<< endl;
        while(!recv_queue.empty())	// go through queue of message until the the queue is empty
        {
            uint8_t reading = recv_queue.front();	// get bit
            recv_queue.pop();	// pop it off the queue
            if(header_received)	// if the header has been received start to process the address and data
            {
                //cout << (int) reading;
                msg_bits.push((int) reading); // Add the info bit to the queue
                bit_counter++;  // Incriment bit counter to keep track of bits being received
                //cout << reading;
                if (bit_counter >= msg_length) //Info bits received
                {
                    // Will break out of reading loop and stop collecting bits
                    msg_recvd = true;
                    break;
                }
                /*
                 
                 
                 Still need to clear the remainding cntents of the recv_queue
                 
                 */
            }
            
            /*
             This will attempt to find the header of the message
             */
            else	// Header is not received (read the header 0's and 1's)
            {
                //check 10 consecutive '0's and '1's
                if(reading == 1)    // reading header 0's
                {
                    if(last_bit == 1)
                        header_counter_zero++;
                    else
                        header_counter_zero = 1;
                    if(header_counter_zero == 10)
                        header_zero_received = true;
                }
                else
                { // reading header 1's
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
    
    
    /*
     bit queue(binary) to int processing
     */
    ret_int = BinaryToInt(msg_bits);
    return ret_int;
    
}
