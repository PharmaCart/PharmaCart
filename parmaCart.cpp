//
//  parmaCart.cpp
//  Total
//
//  Created by David Mathew on 4/5/16.
//  Copyright © 2016 David Mathew. All rights reserved.
//

#include <stdio.h>
#include "parmaCart.h"

using namespace std;

parmaCart::parmaCart(int pin)
{
    //receiver = GPIO130;
    gpio_pin = pin;
    //period = 5;			// Declare the period to read signal
    rising_count = 0;		// Set the rising_count to 0
    receiver = mraa_gpio_init(gpio_pin);
    //gpio_init(receiver);	// Initialize Receiver
    //     Set direction of GPIO
    //gpio_direction(receiver, INPUT);	//Make as an input
    mraa_gpio_dir(receiver, MRAA_GPIO_IN);
}

int parmaCart::get_pin()
{
    return gpio_pin;
}


//IMPORTANT: This is only valid for square setup
bool parmaCart::validBeacon(int ID) {
    if (abs(ID - currentBeacon->beaconID) %6 ==1)
        return true;
    else
        return false;
}

//function is called after every tick, if statement ensures correct coordinate is increased
void parmaCart::updateCoordinates() {
    
    if (nextBeacon == NULL)
        return;
    
    if (currentBeacon->x == nextBeacon->x && nextBeacon->y > currentBeacon->y) {
        y++;
    }
    else if (currentBeacon->x == nextBeacon->x && nextBeacon->y < currentBeacon->y)
        y--;
    else if (currentBeacon->y == nextBeacon->y && nextBeacon->x > currentBeacon->x)
        x++;
    else
        x--;
    
    //check to see if cart has passed a beacon w/o receiving transmission
    for (int i = 0; i < 8; i++)
    {
        if (x == allBeacons[i]->x && y == allBeacons[i]->y)
            setCurrentBeacon(i);
    }
    
    updateMap();
}

void parmaCart::setCurrentBeacon(int ID) {
    //disregard invalid inputs
    if (ID > 7 || ID < 0)
        return;
    if (validBeacon(ID)) {
        //update current beacon and position
        currentBeacon = allBeacons[ID];
        x = currentBeacon->x;
        y = currentBeacon->y;
        
        //update next beacon
        if (route.empty() == 0 && ID == route.front()->beaconID)
        {
            route.pop_front();
            nextBeacon = route.front();
        }
        //update map
        updateMap();
    }
    ///Incorporate code to make the cart stop and wait for a retransmission.
}


void parmaCart::setRoute(int ID) {
    
    //return if invalid input
    if (ID > 7 || ID < 0)
        return;
    destination = allBeacons[ID];
    
    //return if already at the destination
    if (currentBeacon->beaconID == ID){
        return;
    }
    //set route
    beacon *current = currentBeacon;
    while (current->beaconID != ID) {
        
        if (current->westBeacon->distanceToOthers[ID] <= current->eastBeacon->distanceToOthers[ID] && current->westBeacon->distanceToOthers[ID] <= current->northBeacon->distanceToOthers[ID] && current->westBeacon->distanceToOthers[ID] <= current->southBeacon->distanceToOthers[ID]) {
            route.push_back(current->westBeacon);
            current = current->westBeacon;
        }
        
        else if(current->eastBeacon->distanceToOthers[ID] <= current->westBeacon->distanceToOthers[ID] && current->eastBeacon->distanceToOthers[ID] <= current->northBeacon->distanceToOthers[ID] && current->eastBeacon->distanceToOthers[ID] <= current->southBeacon->distanceToOthers[ID]) {
            route.push_back(current->eastBeacon);
            current = current->eastBeacon;
        }
        else if(current->northBeacon->distanceToOthers[ID] <= current->westBeacon->distanceToOthers[ID] && current->northBeacon->distanceToOthers[ID] <= current->eastBeacon->distanceToOthers[ID] && current->northBeacon->distanceToOthers[ID] <= current->southBeacon->distanceToOthers[ID]) {
            route.push_back(current->northBeacon);
            current = current->northBeacon;
        }
        else if(current->southBeacon->distanceToOthers[ID] <= current->westBeacon->distanceToOthers[ID] && current->southBeacon->distanceToOthers[ID] <= current->northBeacon->distanceToOthers[ID] && current->southBeacon->distanceToOthers[ID] <= current->eastBeacon->distanceToOthers[ID]) {
            route.push_back(current->southBeacon);
            current = current->southBeacon;
        }
        cout << current->beaconID<<"\n"; //this is for debugging purposes
    }
    
    nextBeacon = route.front();
}

void parmaCart::printRoute() {
    
    for (int i = 0; i < route.size(); i++) {
        cout << route[i]->beaconID;
    }
}


void parmaCart::updateRoute() {
    if (currentBeacon->beaconID == route.front()->beaconID){
        route.pop_front();
        nextBeacon = route.front();
    }
}

void parmaCart::reportLocation() {
    
    cout << "Location: (" << x << "," << y << ")" << "\n";
}

void parmaCart::initializeMap() {
    for (int i = 0; i < 11; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            Map[i][j] = '-';
        }
    }
    
    
    for (int i = 0; i < 8; i++)
    {
        string number = to_string(i);
        Map[10 - allBeacons[i]->y][allBeacons[i]->x] = number[0];
    }
    
}

void parmaCart::updateMap() {
    initializeMap();
    Map[10 - y][x] = 'P';
}

void parmaCart::printMap() {
    cout <<"\n";
    for (int i = 0; i < 11; ++i)
    {
        for (int j = 0; j < 11; ++j)
        {
            cout << Map[i][j]<< " ";
        }
        cout << "\n";
    }
    cout <<"\n";
}




int parmaCart::get_beacon_ID(int count)
{
    
    if(count % 16 == 0)
        return count / 16;
    else if((count + 1) % 4 == 0)
        return (count+1)/16;
    else if((count - 1) % 4 == 0)
        return (count - 1)/16;
    else if((count - 2) % 4 == 0)
        return (count-2)/16;
    else if((count + 2) % 4 == 0)
        return (count+2)/16;
    else if((count - 3) % 4 == 0)
        return (count - 3)/16;
    else if((count + 3) % 4 == 0)
        return (count+3)/16;
    else if((count + 4) % 4 == 0)
        return (count+4)/16;
    else if((count - 4) % 4 == 0)
        return (count - 4)/16;
    else if((count - 5) % 4 == 0)
        return (count-5)/16;
    
    else if((count + 5) % 4 == 0)
        return (count+5)/16;
    else if((count - 6) % 4 == 0)
        return (count - 6)/16;
    else if((count + 6) % 4 == 0)
        return (count+6)/16;
    else if((count + 7) % 4 == 0)
        return (count+7)/16;
    else if((count - 7) % 4 == 0)
        return (count - 7)/16;
    else if((count - 8) % 4 == 0)
        return (count -8)/16;
    else
        return 0;
    
}


