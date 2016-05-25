//
//  recv.h
//  
//
//  Created by Mitch Camacho on 4/8/16.
//
//

#ifndef recv_h
#define recv_h

#include <stdio.h>


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
    
    void set_msg_recvd(bool recvd);
    void set_handler_registered(bool regstrd);
    
    // Signal Functions
    int recv();
    
private:
    bool handler_registered;	// used for confirmations
    
    
    
};




#endif /* recv_h */
