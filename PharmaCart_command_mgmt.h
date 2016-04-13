//
//  PharmaCart_command_mgmt.h
//  
//
//  Created by Forrest Lee on 3/15/16.
//
//

#ifndef PharmaCart_command_mgmt_h
#define PharmaCart_command_mgmt_h
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <queue>

using namespace std;

class Robot_Communication
{
public:
    Robot_Communication(string ip_address);
    ~Robot_Communication();
    
    std::string get_next_command();
    void request_commands();
    void update_location(int i);
    void print_command_queue(); //debugging
private:
    std::queue<std::string> com_q_tokens;
    std::string server_ip;
    std::string com_request_str;
    int status; // for bash commands

};

#endif /* PharmaCart_command_mgmt_h */
