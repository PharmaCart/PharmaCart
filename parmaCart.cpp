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


//function to reject invalid input
bool parmaCart::validBeacon(int ID)
{
    if (currentBeacon->distanceToOthers[ID] == 1)
        return true;
    else if(currentBeacon->beaconID == ID)
        return true;
    else
        return false;
}


void parmaCart::setCurrentBeacon(int ID)
{
    //disregard invalid inputs
    if (ID > 6 || ID < 0)
        return;
    if (validBeacon(ID)) {
        //update current beacon and position
        previousBeacon =currentBeacon;
        currentBeacon = allBeacons[ID];
        x = currentBeacon->x;
        y = currentBeacon->y;
        setDirection(previousBeacon->beaconID, currentBeacon->beaconID);
        //update next beacon
        if (route.empty() == 0 && ID == route.front()->beaconID)
        {
            route.pop_front();
            nextBeacon = route.front();
        }
        //update map
        updateMap();
    }
}

void parmaCart::setDirection(int previous, int current)
{
    if (((current == 1 || current ==2) && previous > current)|| (current == 0 && previous ==1) || ((current == 4 || current == 5) && previous > current) || (current == 5 && previous == 0) || (current == 3 && previous == 4))
    {
        direction = 1;
    }
    else if (current == 6 && (previous == 0 || previous == 5))
    {
        direction = 2;
    }
    else if (current == 6 && (previous == 2 || previous == 3))
    {
        direction = 3;
    }
    else
    {
        direction = 0;
    }
}


void parmaCart::setRoute(int ID) {
    
    //return if invalid input
    if (ID > 6 || ID < 0)
        return;
    destination = allBeacons[ID];
    
    //return if already at the destination
    if (currentBeacon->beaconID == ID)
    {
        return;
    }
    //set route
    beacon *current = currentBeacon;
    while (current->beaconID != ID)
    {
        
        if (current->westBeacon->distanceToOthers[ID] == min({currentBeacon->westBeacon->distanceToOthers[ID], currentBeacon->eastBeacon->distanceToOthers[ID], currentBeacon->northBeacon->distanceToOthers[ID], currentBeacon->southBeacon->distanceToOthers[ID]}))
        {
            route.push_back(current->westBeacon);
            current = current->westBeacon;
        }
        
        else if(current->eastBeacon->distanceToOthers[ID] == min({currentBeacon->westBeacon->distanceToOthers[ID], currentBeacon->eastBeacon->distanceToOthers[ID], currentBeacon->northBeacon->distanceToOthers[ID], currentBeacon->southBeacon->distanceToOthers[ID]}))
        {
            route.push_back(current->eastBeacon);
            current = current->eastBeacon;
        }
        else if(current->northBeacon->distanceToOthers[ID] == min({currentBeacon->westBeacon->distanceToOthers[ID], currentBeacon->eastBeacon->distanceToOthers[ID], currentBeacon->northBeacon->distanceToOthers[ID], currentBeacon->southBeacon->distanceToOthers[ID]}))
        {
            route.push_back(current->northBeacon);
            current = current->northBeacon;
        }
        else if(current->southBeacon->distanceToOthers[ID] == min({currentBeacon->westBeacon->distanceToOthers[ID], currentBeacon->eastBeacon->distanceToOthers[ID], currentBeacon->northBeacon->distanceToOthers[ID], currentBeacon->southBeacon->distanceToOthers[ID]}))
        {
            route.push_back(current->southBeacon);
            current = current->southBeacon;
        }
        cout << current->beaconID<<"\n"; //this is for debugging purposes, can remove it after
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

void parmaCart::initializeMap()
{
    for (int i = 0; i < 13; i++)
    {
        for (int j = 0; j < 25; j++)
        {
            Map[i][j] = '-';
        }
    }
    

    Map[12 - 0][6] = '0';
    Map[12 - 6][0] = '1';
    Map[12 - 12][6] = '2';
    Map[12 - 12][18] = '3';
    Map[12 - 6][24] = '4';
    Map[12 - 0][18] = '5';
    Map[12 - 6][12] = '6';

}

//Update Map with cart's location
void parmaCart::updateMap()
{
    initializeMap();
    Map[12 - y][x] = 'A';
    Map[12 - y2][x2] = 'B';
}

//Print Map
void parmaCart::printMap()
{
    cout <<"\n";
    for (int i = 0; i < 13; ++i)
    {
        for (int j = 0; j < 25; ++j)
        {
            cout << Map[i][j]<< " ";
        }
        cout << "\n";
    }
    cout <<"\n";
}

void parmaCart::collisionAvoidance(int otherCurrentBeacon, int otherNextBeacon, int otherDestination)
{
    //situation in which carts need to through eachother. Switch destinations
    if (currentBeacon->beaconID == otherNextBeacon && nextBeacon->beaconID == otherCurrentBeacon)
    {
        setRoute(otherDestination);
    }
    
    //situation at junction where one cart must wait. In this case we do nothing, but for the other cart change this function to make the cart wait until the other cart reaches its next beacon
    if (currentBeacon->beaconID == otherNextBeacon && nextBeacon->beaconID != otherCurrentBeacon)
    {
        return;
    }
}

bool parmaCart::turnAround()
{
    if ((currentBeacon->beaconID == 6 && direction == 3 && (nextBeacon->beaconID == 2 || nextBeacon->beaconID == 3)) || (currentBeacon->beaconID == 6 && direction == 2 && (nextBeacon->beaconID == 0 || nextBeacon->beaconID == 5)))
    {
        return true;
    }
    else if ((currentBeacon->beaconID == 0 || currentBeacon->beaconID == 1 || currentBeacon->beaconID == 2 || currentBeacon->beaconID == 3 || currentBeacon->beaconID == 4 || currentBeacon->beaconID == 5) && ((nextBeacon->beaconID == (currentBeacon->beaconID + 1) % 6 && direction == 1) || (nextBeacon->beaconID == (currentBeacon->beaconID - 1) % 6 && direction == 0)))
    {
        return true;
    }
    else if ((currentBeacon->beaconID == 2 || currentBeacon->beaconID == 5) && nextBeacon->beaconID == 6 && direction == 1)
    {
        return true;
    }
    else if ((currentBeacon->beaconID == 3 || currentBeacon->beaconID == 0) && nextBeacon->beaconID == 6 && direction == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
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


