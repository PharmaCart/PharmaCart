//
//  parmaCart.h
//  Total
//
//  Created by David Mathew on 4/5/16.
//  Copyright © 2016 David Mathew. All rights reserved.
//

#ifndef parmaCart_h
#define parmaCart_h
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <deque>
#include <string>
#include <algorithm>
#include "beacon.h"
#include <mraa/gpio.h>



using namespace std;

class parmaCart {
    
public:
    
    parmaCart(int pin);
    //objects in class
    int x,y; //cart location
    int x2, y2; //other cart's location
    int direction; //0: clockwise in a loop, 1: counterclockwise in a looop, 2: up at 6, 3: down at 6
    //GPIO receiver;
    mraa_gpio_context receiver;
    beacon *currentBeacon; //beacon that the cart has just passed
    beacon *nextBeacon; //next beacon on the cart's route
    beacon *previousBeacon; // previous beacon in order to obtain direction
    beacon *destination; //
    beacon *otherDestination; //other cart's destination
    deque <beacon*> route; //will store the path to destination
    beacon *allBeacons[7]; //pointers to all beacons
    //   deque <item> inventory;
    char Map[13][25];
    int rising_count;
    
    //functions
    bool validBeacon(int ID); //checks if the IR int recieved is valid
    void setCurrentBeacon(int ID); //call this function when cart passes a beacon
    void setRoute(int ID); //call this function to set route
    void updateRoute();
    void printRoute();
    void reportLocation();
    void initializeMap();
    void updateMap();
    void printMap();
    void collisionAvoidance(int otherCurrentBeacon, int otherNextBeacon, int otherDestination);
    bool turnAround();
    int get_beacon_ID(int IR_count);
    int get_pin();
    
private:
    int gpio_pin;
    //int period;
    
};

#endif /* parmaCart_h */
