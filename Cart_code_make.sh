#!/bin/sh

#  Cart_code_make.sh
#  
#
#  Created by Forrest Lee on 3/16/16.
#
gcc -o client_connect parma_com/PharmaCart_Client_2.c
g++ -std=c++11 -o test parma_com/robot_skeleton_com_test.cpp parma_com/PharmaCart_command_mgmt.cpp
