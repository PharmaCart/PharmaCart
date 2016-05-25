#!/bin/sh

#  Cart_code_make.sh
#  
#
#  Created by Forrest Lee on 3/16/16.
#
gcc -o client_connect PharmaCart_Client.c
g++ -std=c++11 -o test robot_skeleton_com_test.cpp PharmaCart_command_mgmt.cpp
