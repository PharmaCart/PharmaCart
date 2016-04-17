all:
  g++ -lmraa -std=c++11 -o testtotal testtotal.cpp gpio.cpp lego_robot.cpp PharmaCart_command_mgmt.cpp parmaCart.cpp SquareSetup.cpp
  gcc -o client_connect PharmaCart_Client.c
