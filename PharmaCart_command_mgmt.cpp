//
//  PharmaCart_command_mgmt.cpp
//  
//
//  Created by Forrest Lee on 2/29/16.
//
//

#include "PharmaCart_command_mgmt.h"
#include <stdio.h>
#include <string>
#include <cstring>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

using namespace std;

queue<std::string> update_command_tokens(std::string queue_name);
bool valid_string(string command);


Robot_Communication::Robot_Communication(string ip_address)
{
    server_ip = ip_address;
    com_request_str = "./client_connect ";
    com_request_str = com_request_str + ip_address;
    status = system(com_request_str.c_str());
    com_q_tokens = update_command_tokens("command.queue");
};

Robot_Communication::~Robot_Communication()
{
    update_command_tokens("command.queue");
    if (!com_q_tokens.empty())
    {
        cout << "remaining commands: \n ";
        //debugging: print queue
        while (!com_q_tokens.empty())
        {
            cout << com_q_tokens.front() << endl;
            com_q_tokens.pop();
        }
    }
}


void Robot_Communication::request_commands()
{
    status = system(com_request_str.c_str());
    com_q_tokens = update_command_tokens("command.queue");
}

void Robot_Communication::update_location(int i)
{
    string loc_update = com_request_str + " " + to_string(i);
    status = system(loc_update.c_str());
}

string Robot_Communication::get_next_command()
{
    string command;
    com_q_tokens = update_command_tokens("command.queue");
    if (com_q_tokens.empty())
        return "end of file";
    
    // get rid of invalid commands
    while (!valid_string(com_q_tokens.front()))
        com_q_tokens.pop();
    
    if (com_q_tokens.empty())
        return "end of file";
    // get command at front of queue
    command = com_q_tokens.front();
    com_q_tokens.pop();
    
    
    string popped_queue;
    // put queue into string form
    while (!com_q_tokens.empty())
    {
        popped_queue = popped_queue + "\n" + com_q_tokens.front();
        com_q_tokens.pop();
    }
    popped_queue = popped_queue + "\n";
    
    // rewrite into file
    char resave_queue[1024];
    sprintf(resave_queue, "echo '%s' >> command.queue", popped_queue.c_str() );
    status = system("rm command.queue");
    status = system(resave_queue);
    com_q_tokens = update_command_tokens("command.queue");
    return command;
}
void Robot_Communication::print_command_queue()
{
    //debugging: print queue
    queue<string> temp (com_q_tokens);
    while (!temp.empty())
    {
        cout << temp.front() << endl;
        temp.pop();
    }
}


bool valid_string(string command)
{
    // check if return to base command
    size_t validator = command.find("return");
    
    if (validator!=string::npos)
        return true;
    
    // check if command is integer
    bool validity = true;
    for (int i = 0; i < command.length(); i++)
        if (!isdigit(command.at(i)))
            validity = false;
    
    return validity;
}

queue<string> update_command_tokens(string queue_name)
{
    std::ifstream com_queue(queue_name.c_str());
    string command;
    bool eof_reached = false;
    int counter = 0;
    queue<string> com_q_tokens;
    
    if (com_queue.is_open())
    {
        bool valid = false;
        //com_queue.seekg(position);
        
        while (!eof_reached)
        {
            getline(com_queue, command);
            if (command.length() > 0)
                com_q_tokens.push(command);
            
            if (com_queue.eof())
                eof_reached = true;
        }
    }
    else
        cout << "couldn't open file\n";
    
    com_queue.close();
    
    return com_q_tokens;
}
/*
int main(int argc, char* argv[])
{
    Robot_Communication r_com(argv[1]);
    
    while (1)
    {
        string command;
        while ((command = r_com.get_next_command()) != "")
        {
            cout << command << endl;
            sleep(1);
        }
        cout << "out of commands" << endl;
        r_com.request_commands();
        sleep(1);
    }
    return 0;
}

*/



