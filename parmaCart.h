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
#include "beacon.h"
#include <mraa/gpio.h>



using namespace std;

class parmaCart {
    
public:
    
    parmaCart(int pin);
    //objects in class
    int x,y; //cart location
    //GPIO receiver;
    mraa_gpio_context receiver;
    beacon *currentBeacon; //beacon that the cart has just passed
    beacon *nextBeacon; //next beacon on the cart's route
    beacon *destination; //destination
    deque <beacon*> route; //will store the path to destination
    beacon *allBeacons[8]; //pointers to all beacons
    //   deque <item> inventory;
    char Map[11][11];
    int rising_count;
    
    //functions
    bool validBeacon(int ID); //checks if the IR int recieved is valid
    void updateCoordinates(); //call this function when a "tick" is passed
    void setCurrentBeacon(int ID); //call this function when cart passes a beacon
    void setRoute(int ID); //call this function to set route
    void getRoute(deque <beacon*> updatedRoute); //this function will overwrite the route
    void updateRoute(); //Extraneous, this is done by setCurrentBeacon
    void printRoute();
    void reportLocation();
    void initializeMap();
    void updateMap();
    void printMap();
    int get_beacon_ID(int IR_count);
    int get_pin();
    
private:
    int gpio_pin;
    //int period;
    
};

#endif /* parmaCart_h */
