//
//  twoSquareSetup.cpp
//  LPS
//
//  Created by David Mathew on 3/9/16.
//  Copyright © 2016 David Mathew. All rights reserved.
//

#include "twoSquareSetup.hpp"

void twoSquareSetup(beacon &b0, beacon &b1, beacon &b2, beacon &b3, beacon &b4, beacon &b5, beacon &b6, parmaCart &cart)
{
    
    //set cart's pointers to all beacons
    cart.allBeacons[0] = &b0;
    cart.allBeacons[1] = &b1;
    cart.allBeacons[2] = &b2;
    cart.allBeacons[3] = &b3;
    cart.allBeacons[4] = &b4;
    cart.allBeacons[5] = &b5;
    cart.allBeacons[6] = &b6;
    

    
    //beacon ID's
    b0.beaconID = 0; b1.beaconID = 1; b2.beaconID = 2; b3.beaconID = 3; b4.beaconID = 4; b5.beaconID = 5; b6.beaconID = 6;
    
    //setting up map 
    b0.x = 6, b0.y = 0;
    b1.x = 0, b1.y = 6;
    b2.x = 6, b2.y = 12;
    b3.x = 18, b3.y = 12;
    b4.x = 24, b4.y = 6;
    b5.x = 18, b5.y = 0;
    b6.x = 12, b6.y = 6;

    
    //setting beacon0 pointers/distance to others
    b0.westBeacon = &b1;
    b0.eastBeacon = &b5;
    b0.northBeacon = &b6;
    b0.southBeacon =  &b0;
    setDistance(b0);
                        
    //setting beacon1 pointer/distance to others
    b1.southBeacon = &b0;
    b1.northBeacon = &b2;
    b1.westBeacon = b1.eastBeacon = &b1;
    setDistance(b1);
    
    //beacon 2
    b2.southBeacon = &b6;
    b2.eastBeacon = &b3;
    b2.westBeacon = &b1;
    b2.northBeacon = &b2;
    setDistance(b2);
    
    //beacon 3
    b3.westBeacon = &b2;
    b3.eastBeacon = &b4;
    b3.southBeacon = &b6;
    b3.northBeacon = &b3;
    setDistance(b3);
    
    //beacon 4
    b4.northBeacon = &b3;
    b4.southBeacon = &b5;
    b4.westBeacon = b4.eastBeacon = &b4;
    setDistance(b4);
    
    //beacon 5
    b5.eastBeacon = &b4;
    b5.northBeacon = &b6;
    b5.westBeacon = &b0;
    b5.southBeacon = &b5;
    setDistance(b5);
    
    //beacon 6
    b6.northBeacon = &b2;
    b6.westBeacon = &b3;
    b6.eastBeacon = &b0;
    b6.southBeacon = &b5;
    int middleDistances[7] = {1,2,1,1,2,1,0};
    for (int i = 0; i <7; i++)
    {
        b6.distanceToOthers[i] = middleDistances[i];
    }

    
    //initialize cart's map
    cart.initializeMap();
    
    //have cart start at b0 aka the yarging station
    cart.x = b0.x;
    cart.y = b0.y;
    cart.currentBeacon = cart.previousBeacon = &b0;

    
    cart.updateMap();
    
}

void setDistance(beacon &b) {
    
    int distance[6] = {0,1,2,3,2,1};
    for (int i = 0; i < 6; i++)
    {
        b.distanceToOthers[(b.beaconID +i) % 6] = distance[i];
    }
    
    int dist[6] = {1,2,1,1,2,1};

    b.distanceToOthers[6] = dist[b.beaconID];
}


